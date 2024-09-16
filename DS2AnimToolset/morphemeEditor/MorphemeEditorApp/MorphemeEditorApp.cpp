#include "MorphemeEditorApp.h"
#include "extern.h"
#include "RenderManager/RenderManager.h"
#include "WorkerThread/WorkerThread.h"
#include "FBXTranslator/FBXTranslator.h"
#include "XMDTranslator/XMDTranslator.h"
#include "FromSoftware/TimeAct/TaeExport/TaeExport.h"
#include "FromSoftware/TimeAct/TaeTemplate/TaeTemplateXML/TaeTemplateXML.h"
#include <thread>

namespace
{
	float calculateOptimalCameraDistance(Camera* camera, Character* character)
	{
		FlverModel* model = character->getCharacterModelCtrl()->getModel();

		if (model == nullptr)
			return 5.f;

		float aabbDiagonalLenght = Vector3(Vector3::Transform(model->getBoundingBoxMax(), model->getWorldMatrix()) - Vector3::Transform(model->getBoundingBoxMin(), model->getWorldMatrix())).Length();

		if (aabbDiagonalLenght > (FLT_MAX / 2))
			return 5.f;

		return (aabbDiagonalLenght / 2.f) / std::tanf(camera->getFov() / 2.f);
	}

	void exportFlverToMorphemeBoneMap(FlverModel* model, std::wstring path)
	{
		std::ofstream out(path, std::ios::out);
		
		std::vector<int> boneMap = model->getFlverToMorphemeBoneMap();

		char line[256];
		for (size_t i = 0; i < boneMap.size(); i++)
		{
			sprintf_s(line, "%d -> %d (%s -> %s)\n", i, boneMap[i], model->getFlverBoneName(i).c_str(), model->getMorphemeBoneName(boneMap[i]).c_str());
			out << line;
		}

		out.close();
	}

	void exportNetworkDefFnTables(MR::NetworkDef* netDef, std::wstring path)
	{
		std::ofstream out(path, std::ios::out);

		char tableBuf[256];
		char lineBuf[256];

		int numTables = netDef->getTaskQueuingFnTables()->getNumTaskFnTables();
		for (int i = 0; i < numTables; i++)
		{
			const MR::SharedTaskFnTables::SharedTaskFn* table = netDef->getTaskQueuingFnTables()->getTaskFnTable(i);

			sprintf_s(tableBuf, "TaskQueuingFnTable_%d\n", i);
			out << tableBuf;

			for (int sem = 0; sem < MR::Manager::getInstance().getNumRegisteredAttribSemantics(); sem++)
			{
				sprintf_s(lineBuf, "\t%s\t\t\t\t:\t%s\n", MR::Manager::getInstance().getAttributeSemanticName(sem), MR::Manager::getInstance().getTaskQueuingFnName(MR::QueueAttrTaskFn(table[sem])));
				out << lineBuf;
			}
		}

		out.close();
	}

	bool exportModelToFbx(std::wstring path, Character* character)
	{
		bool status = true;

		g_appLog->debugMessage(MsgLevel_Info, "Exporting model to FBX for character %s\n", character->getCharacterName().c_str());

		FbxExporter* pExporter = FbxExporter::Create(g_pFbxManager, "Flver Exporter");
		pExporter->SetFileExportVersion(FBX_2014_00_COMPATIBLE);

		std::string modelOutPath = RString::toNarrow(path) + RString::toNarrow(character->getCharacterName()) + ".fbx";

		try
		{
			if (!pExporter->Initialize(modelOutPath.c_str()), g_pFbxManager->GetIOPluginRegistry()->GetNativeWriterFormat(), g_pFbxManager->GetIOSettings())
				return false;
		}
		catch (const std::exception& e)
		{
			g_appLog->panicMessage(e.what());
		}

		FbxScene* pScene = FbxScene::Create(g_pFbxManager, RString::toNarrow(character->getCharacterName()).c_str());
		pScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::eMax);
		pScene->GetGlobalSettings().SetSystemUnit(FbxSystemUnit::m);

		FBXTranslator::createLightNode(pScene, FbxVector4(1000.f, 0.f, 0.f), "Forward");
		FBXTranslator::createLightNode(pScene, FbxVector4(-1000.f, 0.f, 0.f), "Backward");

		FBXTranslator::createLightNode(pScene, FbxVector4(0.f, 1000.f, 0.f), "Up");
		FBXTranslator::createLightNode(pScene, FbxVector4(0.f, -1000.f, 0.f), "Down");

		FBXTranslator::createLightNode(pScene, FbxVector4(0.f, 0.f, 1000.f), "Left");
		FBXTranslator::createLightNode(pScene, FbxVector4(0.f, 0.f, -1000.f), "Right");

		FbxPose* pBindPoses = FbxPose::Create(g_pFbxManager, "BindPoses");
		pBindPoses->SetIsBindPose(true);

		std::vector<FbxNode*> morphemeRig = FBXTranslator::createFbxMorphemeSkeleton(pScene, character->getMorphemeCharacter()->getNetwork()->getRig(0), pBindPoses);

		FlverModel* model = character->getCharacterModelCtrl()->getModel();

		if (!FBXTranslator::createFbxModel(pScene, model, RString::toNarrow(character->getCharacterName()), pBindPoses, morphemeRig, modelOutPath, model->getFlverToMorphemeBoneMap()))
		{
			g_appLog->debugMessage(MsgLevel_Error, "Failed to create FBX model/skeleton (chr=%ws)\n", character->getCharacterName().c_str());
			status = false;
		}

		pScene->AddPose(pBindPoses);

		//Shutdown
		pExporter->Export(pScene);
		pScene->Destroy(true);
		pExporter->Destroy(true);
	}

	bool exportModelToXmd(std::wstring path, Character* character)
	{
		bool status = true;

		g_appLog->debugMessage(MsgLevel_Info, "Exporting model to XMD for character %ws\n", character->getCharacterName().c_str());

		FlverModel* model = character->getCharacterModelCtrl()->getModel();
		MR::AnimRigDef* rig = character->getRig(0);

		XMD::XModel* modelXmd = XMDTranslator::createModel(rig, model, RString::toNarrow(model->getFileOrigin()).c_str(), true);
		XMDTranslator::createBindPoseAnimCycle(modelXmd, rig);

		if (modelXmd->Save(RString::toNarrow(path) + "\\" + RString::toNarrow(character->getCharacterName()) + ".xmd") != XMD::XFileError::Success)
			status = false;

		delete modelXmd;

		return status;
	}

	bool exportAnimationToFbx(std::wstring path, Character* character, int animIdx, bool addModel)
	{
		bool status = true;

		MorphemeCharacterDef* characterDef = character->getMorphemeCharacterDef();
		AnimObject* anim = characterDef->getAnimation(animIdx);

		int animId = anim->getAnimID();

		if (characterDef == nullptr)
			throw("characterDef was nullptr\n");

		std::string animName = RString::removeExtension(characterDef->getAnimFileLookUp()->getSourceFilename(animId));

		g_appLog->debugMessage(MsgLevel_Info, "Exporting animation %s to FBX (%ws)\n", animName.c_str(), character->getCharacterName().c_str());

		FbxExporter* pExporter = FbxExporter::Create(g_pFbxManager, "Flver Exporter");
		pExporter->SetFileExportVersion(FBX_2014_00_COMPATIBLE);

		std::string outPath = RString::toNarrow(path) + animName + ".fbx";

		try
		{
			if (!pExporter->Initialize(outPath.c_str()), g_pFbxManager->GetIOPluginRegistry()->GetNativeWriterFormat(), g_pFbxManager->GetIOSettings())
			{
				return false;
			}
		}
		catch (const std::exception& e)
		{
			g_appLog->panicMessage(e.what());
		}

		FbxScene* pScene = FbxScene::Create(g_pFbxManager, animName.c_str());
		pScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::eMax);
		pScene->GetGlobalSettings().SetSystemUnit(FbxSystemUnit::m);

		FBXTranslator::createLightNode(pScene, FbxVector4(1000.f, 0.f, 0.f), "Forward");
		FBXTranslator::createLightNode(pScene, FbxVector4(-1000.f, 0.f, 0.f), "Backward");

		FBXTranslator::createLightNode(pScene, FbxVector4(0.f, 1000.f, 0.f), "Up");
		FBXTranslator::createLightNode(pScene, FbxVector4(0.f, -1000.f, 0.f), "Down");

		FBXTranslator::createLightNode(pScene, FbxVector4(0.f, 0.f, 1000.f), "Left");
		FBXTranslator::createLightNode(pScene, FbxVector4(0.f, 0.f, -1000.f), "Right");

		FbxPose* pBindPoses = FbxPose::Create(g_pFbxManager, "BindPoses");
		pBindPoses->SetIsBindPose(true);

		std::vector<FbxNode*> morphemeRig = FBXTranslator::createFbxMorphemeSkeleton(pScene, character->getMorphemeCharacter()->getNetwork()->getRig(0), pBindPoses);

		if (addModel)
		{
			FlverModel* model = character->getCharacterModelCtrl()->getModel();

			if (!FBXTranslator::createFbxModel(pScene, model, RString::toNarrow(character->getCharacterName()), pBindPoses, morphemeRig, outPath, model->getFlverToMorphemeBoneMap()))
			{
				g_appLog->debugMessage(MsgLevel_Error, "Failed to create FBX model/skeleton (%ws)\n", character->getCharacterName().c_str());
				status = false;
			}
		}

		if (!FBXTranslator::createFbxTake(pScene, morphemeRig, characterDef->getAnimationById(animId), characterDef->getAnimFileLookUp()->getTakeName(animId)))
		{
			g_appLog->debugMessage(MsgLevel_Error, "Failed to create FBX take (%ws)\n", character->getCharacterName().c_str());
			status = false;
		}

		pScene->AddPose(pBindPoses);

		//Shutdown
		pExporter->Export(pScene);
		pScene->Destroy(true);
		pExporter->Destroy(true);

		return status;
	}

	bool exportAnimationToXmd(std::wstring path, Character* character, int animIdx)
	{
		bool status = true;

		MorphemeCharacterDef* characterDef = character->getMorphemeCharacterDef();
		AnimObject* anim = characterDef->getAnimation(animIdx);

		int animId = anim->getAnimID();

		if (anim->getHandle() == nullptr)
			return false;

		MR::AnimRigDef* rig = character->getRig(0);

		std::string animName = RString::removeExtension(characterDef->getAnimFileLookUp()->getSourceFilename(animId));

		g_appLog->debugMessage(MsgLevel_Info, "Exporting animation %s to XMD (%ws)\n", animName.c_str(), character->getCharacterName().c_str());

		XMD::XModel* xmd = XMDTranslator::createModel(rig, character->getCharacterModelCtrl()->getModel(), characterDef->getAnimFileLookUp()->getSourceFilename(animId), false);
		XMDTranslator::createAnimCycle(xmd, anim, characterDef->getAnimFileLookUp()->getTakeName(animId));

		if (xmd->Save(RString::toNarrow(path) + animName + ".xmd") != XMD::XFileError::Success)
			status = false;

		delete xmd;

		return status;
	}

	void addEventGroupToTimeActTrack(TimeAct::TimeActTrack* dst, TimeAct::TaeExport::TimeActGroupExportXML* groupXML)
	{
		TimeAct::EventGroup* group = dst->addGroup(groupXML->getGroupId());

		for (size_t eventIdx = 0; eventIdx < groupXML->getNumEvents(); eventIdx++)
		{
			TimeAct::TaeExport::TimeActEventExportXML* eventXML = groupXML->getEvent(eventIdx);

			TimeAct::TimeActEvent* event = group->addEvent(eventXML->getStartTime(), eventXML->getEndTime(), eventXML->getEventId(), g_taeTemplate);

			TimeAct::TaeExport::TimeActArgumentListExportXML* argListXML = eventXML->getArgumentList();

			if (argListXML->getNumArguments() != event->getNumArguments())
				throw("Argument count mismatch\n");

			for (size_t argIdx = 0; argIdx < argListXML->getNumArguments(); argIdx++)
			{
				TimeAct::TaeExport::TimeActArgumentExportXML* argXML = argListXML->getArgument(argIdx);
				TimeAct::Argument* arg = event->getArgument(argIdx);

				TimeAct::DataType type = argXML->getType();

				switch (type)
				{
				case TimeAct::kBool:
					arg->setValue(argXML->getAsBool());
					break;
				case TimeAct::kByte:
					arg->setValue(argXML->getAsByte());
					break;
				case TimeAct::kUByte:
					arg->setValue(argXML->getAsUByte());
					break;
				case TimeAct::kShort:
					arg->setValue(argXML->getAsShort());
					break;
				case TimeAct::kUShort:
					arg->setValue(argXML->getAsUShort());
					break;
				case TimeAct::kInt:
					arg->setValue(argXML->getAsInt());
					break;
				case TimeAct::kUInt:
					arg->setValue(argXML->getAsUInt());
					break;
				case TimeAct::kFloat:
					arg->setValue(argXML->getAsFloat());
					break;
				case TimeAct::kInt64:
					arg->setValue(argXML->getAsInt64());
					break;
				case TimeAct::kUInt64:
					arg->setValue(argXML->getAsUInt64());
					break;
				}
			}
		}
	}

	std::wstring findGamePath(std::wstring current_path)
	{
		std::filesystem::path gamepath = current_path;

		do
		{
			std::wstring parent_path = gamepath.parent_path();
			gamepath = parent_path;

			int lastDirPos = parent_path.find_last_of(L"\\");

			std::wstring folder = parent_path.substr(lastDirPos, parent_path.length());

			if (folder.compare(L"\\") == 0)
				return L"";

			if (folder.compare(L"\\Game") == 0)
				return gamepath;

		} while (true);

		return L"";
	}

	int getEquipIDByFilename(std::wstring filename)
	{
		size_t first = filename.find_first_of(L"_") + 1;
		size_t second = filename.find_last_of(L"_");

		std::wstring modelIdStr = filename.substr(first, second - first);

		return std::stoi(modelIdStr);
	}

	int getFgIDByFilename(std::wstring filename, FgPartType type)
	{
		int baseId = 0;

		switch (type)
		{
		case kFgFace:
			baseId = 1000;
			break;
		case kFgHead:
			baseId = 2000;
			break;
		case kFgEyes:
			baseId = 3000;
			break;
		case kFgEyeBrows:
			baseId = 4000;
			break;
		case kFgBeard:
			baseId = 5000;
			break;
		case kFgHair:
			baseId = 7000;
			break;
		default:
			break;
		}

		int id = getEquipIDByFilename(filename);

		return id - baseId;
	}

	void fillFlverResources(FlverResources* dst, std::wstring gamePath)
	{
		std::filesystem::path shield_path = gamePath + L"\\model\\parts\\shield";
		std::filesystem::path weapon_path = gamePath + L"\\model\\parts\\weapon";

		std::filesystem::path head_path = gamePath + L"\\model\\parts\\head";
		std::filesystem::path face_path = gamePath + L"\\model\\parts\\face";
		std::filesystem::path body_path = gamePath + L"\\model\\parts\\body";
		std::filesystem::path arm_path = gamePath + L"\\model\\parts\\arm";
		std::filesystem::path leg_path = gamePath + L"\\model\\parts\\leg";

		dst->weaponModelResources.addEntry(L"None", -1);

		for (const auto& entry : std::filesystem::directory_iterator(weapon_path))
		{
			if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && (entry.path().stem().string().back() != 'a') && (entry.path().stem().string().back() != 'l'))
			{
				std::wstring filepath = entry.path();
				int id = getEquipIDByFilename(filepath);

				dst->weaponModelResources.addEntry(filepath, id);
			}
		}

		dst->shieldModelResources.addEntry(L"None", -1);

		for (const auto& entry : std::filesystem::directory_iterator(shield_path))
		{
			if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && (entry.path().stem().string().back() != 'a') && (entry.path().stem().string().back() != 'l'))
			{
				std::wstring filepath = entry.path();
				int id = getEquipIDByFilename(filepath);

				dst->shieldModelResources.addEntry(filepath, id);
			}
		}

		dst->headModelResources.addEntry(L"None", -1);

		for (const auto& entry : std::filesystem::directory_iterator(head_path))
		{
			if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && (entry.path().stem().string().back() != 'l'))
			{
				std::wstring filepath = entry.path();
				int id = getEquipIDByFilename(filepath);

				dst->headModelResources.addEntry(filepath, id);
			}
		}

		dst->faceModelResources.addEntry(L"None", -1);

		for (const auto& entry : std::filesystem::directory_iterator(face_path))
		{
			if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && (entry.path().stem().string().back() != 'l') && (entry.path().stem().string().substr(0, 2) == "fc"))
			{
				std::wstring filepath = entry.path();
				int id = getEquipIDByFilename(filepath);

				dst->faceModelResources.addEntry(filepath, id);
			}
		}

		dst->bodyModelResources.addEntry(L"None", -1);

		for (const auto& entry : std::filesystem::directory_iterator(body_path))
		{
			if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && (entry.path().stem().string().back() != 'l'))
			{
				std::wstring filepath = entry.path();
				int id = getEquipIDByFilename(filepath);

				dst->bodyModelResources.addEntry(filepath, id);
			}
		}

		dst->armModelResources.addEntry(L"None", -1);

		for (const auto& entry : std::filesystem::directory_iterator(arm_path))
		{
			if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && (entry.path().stem().string().back() != 'l'))
			{
				std::wstring filepath = entry.path();
				int id = getEquipIDByFilename(filepath);

				dst->armModelResources.addEntry(filepath, id);
			}
		}

		dst->legModelResources.addEntry(L"None", -1);

		for (const auto& entry : std::filesystem::directory_iterator(leg_path))
		{
			if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && (entry.path().stem().string().back() != 'l'))
			{
				std::wstring filepath = entry.path();
				int id = getEquipIDByFilename(filepath);

				dst->legModelResources.addEntry(filepath, id);
			}
		}

		dst->fgFaceModelResources.addEntry(L"None", -1);

		for (const auto& entry : std::filesystem::directory_iterator(face_path))
		{
			if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && ((entry.path().stem().string().back() == 'f') || (entry.path().stem().string().back() == 'm')) && (entry.path().stem().string().substr(0, 4) == "fg_1"))
			{
				std::wstring filepath = entry.path();
				int id = getFgIDByFilename(filepath, kFgFace);

				dst->fgFaceModelResources.addEntry(filepath, id);
			}
		}

		dst->fgHeadModelResources.addEntry(L"None", -1);

		for (const auto& entry : std::filesystem::directory_iterator(face_path))
		{
			if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && ((entry.path().stem().string().back() == 'f') || (entry.path().stem().string().back() == 'm')) && (entry.path().stem().string().substr(0, 4) == "fg_2"))
			{
				std::wstring filepath = entry.path();
				int id = getFgIDByFilename(filepath, kFgHead);

				dst->fgHeadModelResources.addEntry(filepath, id);
			}
		}

		dst->fgEyesModelResources.addEntry(L"None", -1);

		for (const auto& entry : std::filesystem::directory_iterator(face_path))
		{
			if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && ((entry.path().stem().string().back() == 'a')) && (entry.path().stem().string().substr(0, 4) == "fg_3"))
			{
				std::wstring filepath = entry.path();
				int id = getFgIDByFilename(filepath, kFgEyes);

				dst->fgEyesModelResources.addEntry(filepath, id);
			}
		}

		dst->fgEyeBrowsModelResources.addEntry(L"None", -1);

		for (const auto& entry : std::filesystem::directory_iterator(face_path))
		{
			if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && ((entry.path().stem().string().back() == 'm')) && (entry.path().stem().string().substr(0, 4) == "fg_4"))
			{
				std::wstring filepath = entry.path();
				int id = getFgIDByFilename(filepath, kFgEyeBrows);

				dst->fgEyeBrowsModelResources.addEntry(filepath, id);
			}
		}

		dst->fgBeardModelResources.addEntry(L"None", -1);

		for (const auto& entry : std::filesystem::directory_iterator(face_path))
		{
			if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && ((entry.path().stem().string().back() == 'm')) && (entry.path().stem().string().substr(0, 4) == "fg_5"))
			{
				std::wstring filepath = entry.path();
				int id = getFgIDByFilename(filepath, kFgBeard);

				dst->fgBeardModelResources.addEntry(filepath, id);
			}
		}

		dst->fgHairModelResources.addEntry(L"None", -1);

		for (const auto& entry : std::filesystem::directory_iterator(face_path))
		{
			if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && ((entry.path().stem().string().back() == 'f') || (entry.path().stem().string().back() == 'm')) && (entry.path().stem().string().substr(0, 4) == "fg_7"))
			{
				std::wstring filepath = entry.path();
				int id = getFgIDByFilename(filepath, kFgHair);

				dst->fgHairModelResources.addEntry(filepath, id);
			}
		}
	}
}

MorphemeEditorApp* MorphemeEditorApp::getInstance()
{
	if (_instance == nullptr)
		_instance = new MorphemeEditorApp();

	return _instance;
}

void MorphemeEditorApp::initialise()
{
	MorphemeSystem::initMorpheme();

	this->m_flverResources = new FlverResources;

	this->m_timeActEditor = TrackEditor::TimeActEditor::create(TrackEditor::kEditorEditAll | TrackEditor::kEditorChangeFrame | TrackEditor::kEditorMarkActiveEvents | TrackEditor::kEditorHighlightSelectedEvent, TrackEditor::kSeconds, g_taeTemplate);
	this->m_eventTrackEditor = TrackEditor::EventTrackEditor::create(TrackEditor::kEditorEditAll | TrackEditor::kEditorRenameTrack | TrackEditor::kEditorChangeFrame | TrackEditor::kEditorMarkActiveEvents | TrackEditor::kEditorHighlightSelectedEvent, TrackEditor::kSeconds);

	this->m_eventTrackEditor->registerListener(this->m_timeActEditor);
	this->m_timeActEditor->registerListener(this->m_eventTrackEditor);

	this->loadSettings();
	this->loadPlayerModelPreset();

	this->m_initialised = true;
}

void MorphemeEditorApp::update(float dt)
{
	if (!this->m_initialised)
		throw("Called update() without calling initialise() first\n");

	if (this->m_animPlayer)
	{
		this->m_animPlayer->update(dt);

		if (this->m_eventTrackEditor && this->m_eventTrackEditor->getSource())
		{
			if (!this->m_animPlayer->isPaused())
				this->m_eventTrackEditor->setCurrentTime(this->m_animPlayer->getTime());
			else
				this->m_animPlayer->setTime(this->m_eventTrackEditor->getCurrentTime());
		}

		if (this->m_timeActEditor && this->m_timeActEditor->getSource())
		{
			if (!this->m_animPlayer->isPaused())
				this->m_timeActEditor->setCurrentTime(this->m_animPlayer->getTime());
			else
				this->m_animPlayer->setTime(this->m_timeActEditor->getCurrentTime());
		}
	}

	if (this->m_character)
	{
		this->m_character->update(dt);
		
		CharacterModelCtrl* modelCtrl = this->m_character->getCharacterModelCtrl();

		if (modelCtrl)
		{
			modelCtrl->setDisplayMode(this->m_previewFlags.displayMode);
			modelCtrl->setShowDummies(this->m_previewFlags.drawDummies);
			modelCtrl->setDrawBones(this->m_previewFlags.drawBones);
			modelCtrl->setDrawBoundingBox(this->m_previewFlags.drawBoundingBoxes);
		}
	}

	if (this->m_camera)
	{
		int width = RenderManager::getInstance()->getWidth();
		int height = RenderManager::getInstance()->getHeight();

		this->m_camera->update(float(width), float(height), dt);

		if (this->m_character)
		{
			FlverModel* model = this->m_character->getCharacterModelCtrl()->getModel();

			if (model != nullptr)
			{
				Matrix characterRoot = (*model->getFlverRootBoneTransform()) * model->getWorldMatrix();

				this->m_camera->setTarget(Vector3::Transform(Vector3::Zero, characterRoot));
			} 
		}

		if (this->m_taskFlags.resetCamera)
		{
			this->m_taskFlags.resetCamera = false;

			this->m_camera->setOffset(Vector3::Zero);
			this->m_camera->setRadius(calculateOptimalCameraDistance(this->m_camera, this->m_character));
		}
	}

	if (this->m_timeActEditor)
		this->m_timeActEditor->update(dt);

	if (this->m_eventTrackEditor)
		this->m_eventTrackEditor->update(dt);

	if (this->m_taskFlags.loadFile)
	{
		this->m_taskFlags.loadFile = false;

		this->loadFile();
	}

	if (this->m_taskFlags.saveFile)
	{
		this->m_taskFlags.saveFile = false;

		this->saveFile();
	}

	if (this->m_taskFlags.exportTaeTemplateXml)
	{
		this->m_taskFlags.exportTaeTemplateXml = false;

		g_workerThread.load()->startThread("Export TimeAct Template", &MorphemeEditorApp::exportTaeTemplateXML, this);
	}

	if (this->m_taskFlags.exportAll)
	{
		this->m_taskFlags.exportAll = false;

		g_workerThread.load()->startThread("Export All", &MorphemeEditorApp::exportAll, this);
	}

	if (this->m_taskFlags.exportTae)
	{
		this->m_taskFlags.exportTae = false;

		wchar_t exportPath[256];
		swprintf_s(exportPath, L"Export\\%ws\\", this->m_character->getCharacterName().c_str());

		std::filesystem::create_directories(exportPath);

		g_workerThread.load()->startThread("Export TimeAct", &MorphemeEditorApp::compileAndExportTae, this, std::wstring(exportPath));
	}

	if (this->m_taskFlags.exportModel)
	{
		this->m_taskFlags.exportModel = false;

		if (this->m_character != nullptr)
		{
			wchar_t exportPath[256];
			swprintf_s(exportPath, L"Export\\%ws\\", this->m_character->getCharacterName().c_str());

			std::filesystem::create_directories(exportPath);

			g_workerThread.load()->startThread("Export Model" ,&MorphemeEditorApp::exportModel, this, std::wstring(exportPath));
		}
		else
		{
			g_appLog->alertMessage(MsgLevel_Error, "No character is loaded");
		}
	}

	if (this->m_taskFlags.exportNetwork)
	{
		this->m_taskFlags.exportNetwork = false;

		if (this->m_character != nullptr)
		{
			wchar_t exportPath[256];
			swprintf_s(exportPath, L"Export\\%ws\\", this->m_character->getCharacterName().c_str());

			std::filesystem::create_directories(exportPath);

			g_workerThread.load()->startThread("Export Network", &MorphemeEditorApp::exportNetwork, this, std::wstring(exportPath));
		}
		else
		{
			g_appLog->alertMessage(MsgLevel_Error, "No character is loaded");
		}
	}

	if (this->m_taskFlags.exportAnimations)
	{
		this->m_taskFlags.exportAnimations = false;

		if (this->m_character != nullptr && this->m_character->getMorphemeCharacter() != nullptr)
		{
			wchar_t exportPath[256];
			swprintf_s(exportPath, L"Export\\%ws\\", this->m_character->getCharacterName().c_str());
			
			g_workerThread.load()->startThread("Export Animations", &MorphemeEditorApp::exportAnimationsAndMarkups, this, std::wstring(exportPath));
		}
		else
		{
			g_appLog->alertMessage(MsgLevel_Error, "No character is loaded");
		}
	}

	if (this->m_taskFlags.compileTaes)
	{
		this->m_taskFlags.compileTaes = false;

		wchar_t exportPath[256];
		swprintf_s(exportPath, L"Export\\%ws\\", this->m_character->getCharacterName().c_str());

		std::filesystem::create_directories(exportPath);

		g_workerThread.load()->startThread("Compile TimeAct", &MorphemeEditorApp::compileTimeActFiles, this, std::wstring(exportPath));
	}
}

void MorphemeEditorApp::shutdown()
{
	this->saveSettings();
	this->savePlayerModelPreset();

	MorphemeSystem::termMorpheme();

	if (this->m_flverResources)
	{
		delete this->m_flverResources;
		this->m_flverResources = nullptr;
	}

	if (this->m_animPlayer)
	{
		this->m_animPlayer->clear();
		delete this->m_animPlayer;
	}

	if (this->m_character)
		this->m_character->destroy();

	if (this->m_camera)
		delete this->m_camera;

	if (this->m_timeActEditor)
		this->m_timeActEditor->destroy();

	if (this->m_eventTrackEditor)
		this->m_eventTrackEditor->destroy();

	if (this->_instance)
		delete this->_instance;
}

void MorphemeEditorApp::loadSettings()
{
	RINI* settings = RINI::open("Data\\res\\settings.ini");

	if (settings == nullptr)
	{
		throw("Failed to open settings.ini");
		return;
	}

	this->m_taskFlags.exportFormat = (ExportFormat)settings->getInt("Export", "export_format", 0);
	
	this->m_previewFlags.drawDummies = settings->getBool("ModelViewer", "draw_dummies", false);
	this->m_previewFlags.drawBones = settings->getBool("ModelViewer", "draw_bones", true);
	this->m_previewFlags.displayMode = (DisplayMode)settings->getInt("ModelViewer", "model_disp_mode", 0);

	this->m_timeActEditor->setTimeCodeFormat((TrackEditor::TimeCodeFormat)settings->getInt("TimeActEditor", "time_code_format", 0));
	this->m_timeActEditor->setTimeCodeFormat((TrackEditor::TimeCodeFormat)settings->getInt("EventTrackEditor", "time_code_format", 0));

	settings->destroy();
}

void MorphemeEditorApp::saveSettings()
{
	RINI* settings = RINI::open("Data\\res\\settings.ini");

	if (settings == nullptr)
		settings = RINI::create("Data\\res\\settings.ini");

	settings->setInt("Export", "export_format", this->m_taskFlags.exportFormat);

	settings->setBool("ModelViewer", "draw_dummies", this->m_previewFlags.drawDummies);
	settings->setBool("ModelViewer", "draw_bones", this->m_previewFlags.drawBones);
	settings->setInt("ModelViewer", "model_disp_mode", this->m_previewFlags.displayMode);

	settings->setInt("TimeActEditor", "time_code_format", this->m_timeActEditor->getTimeCodeFormat());
	settings->setInt("EventTrackEditor", "time_code_format", this->m_eventTrackEditor->getTimeCodeFormat());

	settings->write();

	settings->destroy();
}

void MorphemeEditorApp::loadPlayerModelPreset()
{
	this->m_playerModelPreset = PlayerModelPreset::loadFromFile("Data\\res\\c0001.ini");

	if (this->m_playerModelPreset == nullptr)
		g_appLog->panicMessage("Failed to read player model preset at Data\\res\\c0001.ini\n");
}

void MorphemeEditorApp::savePlayerModelPreset()
{
	this->m_playerModelPreset->save();
}

MorphemeEditorApp::MorphemeEditorApp() : Application()
{
	this->m_animPlayer = new AnimPlayer();
	this->m_camera = new Camera();
}

MorphemeEditorApp::~MorphemeEditorApp()
{
}

void MorphemeEditorApp::loadFile()
{
	COMDLG_FILTERSPEC ComDlgFS[] = { {L"Morpheme Network Binary", L"*.nmb"}, {L"TimeAct", L"*.tae"}, {L"All Files",L"*.*"} };

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);

	if (SUCCEEDED(hr))
	{
		IFileOpenDialog* pFileOpen = NULL;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			pFileOpen->SetFileTypes(3, ComDlgFS);

			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);

				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						std::filesystem::path filepath = std::wstring(pszFilePath);

						if (this->m_character)
							this->m_character->destroy();

						if (this->m_eventTrackEditor)
							this->m_eventTrackEditor->reset();

						if (this->m_timeActEditor)
							this->m_timeActEditor->reset();

						this->m_timeActFileList.clear();

						this->m_gamePath = findGamePath(filepath);

						if (filepath.extension() == ".nmb")
							this->m_character = Character::createFromNmb(this->m_timeActFileList, RString::toNarrow(filepath).c_str());
						else if (filepath.extension() == ".tae")
							this->m_character = Character::createFromTimeAct(RString::toNarrow(filepath).c_str());

						if ((this->m_character != nullptr) && (this->m_character->getCharacterId() == 1))
						{
							if (this->m_gamePath.compare(L"") != 0)
								fillFlverResources(this->getFlverResources(), this->m_gamePath);
						}

						this->m_animPlayer->setCharacter(this->m_character);

						this->m_camera->setOffset(Vector3::Zero);
						this->m_camera->setRadius(calculateOptimalCameraDistance(this->m_camera, this->m_character));
					}
					pItem->Release();
				}
				else
					g_appLog->alertMessage(MsgLevel_Error, "Failed to open file");
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
}

void MorphemeEditorApp::saveFile()
{
	COMDLG_FILTERSPEC ComDlgFS[] = { {L"Morpheme Network Binary", L"*.nmb"}, {L"TimeAct", L"*.tae"}, {L"All Files",L"*.*"} };

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);

	if (SUCCEEDED(hr))
	{
		IFileOpenDialog* pFileSave = NULL;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
			IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

		if (SUCCEEDED(hr))
		{
			pFileSave->SetFileTypes(3, ComDlgFS);

			// Show the Open dialog box.
			hr = pFileSave->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				hr = pFileSave->GetResult(&pItem);

				if (SUCCEEDED(hr))
				{
					PWSTR pszOutFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszOutFilePath);

					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						std::filesystem::path filepath = std::wstring(pszOutFilePath);

						if (filepath.extension() == ".nmb")
						{	
						}
						else if (filepath.extension() == ".tae")
						{
						}
					}
					pItem->Release();
				}
				else
					MessageBoxW(NULL, L"Failed to save file", L"Application.cpp", MB_ICONERROR);
			}
			pFileSave->Release();
		}
		CoUninitialize();
	}
}

bool MorphemeEditorApp::exportTimeAct(std::wstring path)
{
	if (this->m_character == nullptr)
		return false;

	TimeAct::TaeExport::TimeActExportXML* tae = this->m_character->getTimeAct();

	if (tae == nullptr)
		return false;

	wchar_t filename[256];
	swprintf_s(filename, L"%ws_tae.xml", RString::toWide(tae->getName()).c_str());

	tae->setDstFileName(RString::toNarrow(path + filename));

	tae->save();
}

bool MorphemeEditorApp::exportNetwork(std::wstring path)
{
	if (this->m_character->getMorphemeCharacter() == nullptr)
		return false;

	MorphemeCharacterDef* characterDef = this->m_character->getMorphemeCharacterDef();
	std::wstring chrName = this->m_character->getCharacterName();

	if (characterDef == nullptr)
		throw("characterDef was nullptr\n");

	g_workerThread.load()->addProcess("Export morpheme bundles", 5);
	g_workerThread.load()->setProcessStepName("Exporting character controllers");

	std::vector<ME::CharacterControllerExportXML*> controllerExports;
	controllerExports.reserve(characterDef->getNumCharacterControllers());

	g_workerThread.load()->addProcess("Export character controllers", characterDef->getNumCharacterControllers());

	for (size_t i = 0; i < characterDef->getNumCharacterControllers(); i++)
	{
		wchar_t filename[256];
		swprintf_s(filename, L"%ws_%d.mrctrl", chrName.c_str(), i);

		g_appLog->debugMessage(MsgLevel_Info, "Exporting character controller %d for %ws\n", i, chrName.c_str());

		ME::CharacterControllerExportXML* characterControllerExport = MorphemeExport::exportCharacterController(characterDef->getCharacterController(i), path + filename);
	
		char processName[256];
		sprintf_s(processName, "Exporting character controller for anim set %d", i);

		g_workerThread.load()->setProcessStepName(processName);

		if (!characterControllerExport->write())
			g_appLog->debugMessage(MsgLevel_Error, "Failed to export character controller for %ws (animSet=%d)\n", chrName.c_str(), i);
	
		controllerExports.push_back(characterControllerExport);

		g_workerThread.load()->increaseProgressStep();
	}

	g_workerThread.load()->increaseProgressStep();
	g_workerThread.load()->setProcessStepName("Exporting animation rigs");

	std::vector<ME::RigExportXML*> rigExports;
	rigExports.reserve(characterDef->getNetworkDef()->getNumAnimSets());

	g_workerThread.load()->addProcess("Export animation rigs", characterDef->getNumCharacterControllers());

	for (size_t i = 0; i < characterDef->getNetworkDef()->getNumAnimSets(); i++)
	{
		wchar_t filename[256];
		swprintf_s(filename, L"%ws_%d.mrarig", chrName.c_str(), i);

		g_appLog->debugMessage(MsgLevel_Info, "Exporting rig for animation set %d for %ws\n", i, chrName.c_str());

		ME::RigExportXML* rigExport = MorphemeExport::exportRig(characterDef->getNetworkDef(), characterDef->getNetworkDef()->getRig(i), path + filename);

		char processName[256];
		sprintf_s(processName, "Exporting rig for anim set %d", i);

		g_workerThread.load()->setProcessStepName(processName);

		if (!rigExport->write())
			g_appLog->debugMessage(MsgLevel_Error, "Failed to export rig for %ws (animSet=%d)\n", chrName.c_str(), i);

		rigExports.push_back(rigExport);

		g_workerThread.load()->increaseProgressStep();
	}

	g_workerThread.load()->increaseProgressStep();
	g_workerThread.load()->setProcessStepName("Exporting animation library");

	g_workerThread.load()->addProcess("Export animation library", 1);

	wchar_t libraryFilename[256];
	swprintf_s(libraryFilename, L"%ws_Library.xml", chrName.c_str());

	g_appLog->debugMessage(MsgLevel_Info, "Exporting animation library for %ws\n", chrName.c_str());

	ME::AnimationLibraryXML* animLibraryExport = MorphemeExport::exportAnimLibrary(characterDef->getAnimFileLookUp(), characterDef->getNetworkDef(), rigExports, controllerExports, chrName.c_str(), path + libraryFilename);

	if (!animLibraryExport->write())
		g_appLog->debugMessage(MsgLevel_Error, "Failed to export animation library for %ws\n", chrName.c_str());

	g_workerThread.load()->increaseProgressStep();

	g_workerThread.load()->increaseProgressStep();
	g_workerThread.load()->setProcessStepName("Exporting message preset library");

	g_workerThread.load()->addProcess("Export message preset library", 1);

	wchar_t messagePresetFilename[256];
	swprintf_s(messagePresetFilename, L"%ws_Preset.xml", chrName.c_str());

	g_appLog->debugMessage(MsgLevel_Info, "Exporting message preset library for %ws\n", chrName.c_str());

	ME::MessagePresetLibraryExportXML* messagePresetExport = MorphemeExport::exportMessagePresetLibrary(characterDef->getNetworkDef(), chrName, path + messagePresetFilename);

	if (!messagePresetExport->write())
		g_appLog->debugMessage(MsgLevel_Error, "Failed to export message library for %ws\n", chrName.c_str());

	g_workerThread.load()->increaseProgressStep();
	g_workerThread.load()->increaseProgressStep();
	g_workerThread.load()->setProcessStepName("Exporting network definition");

	g_workerThread.load()->addProcess("Export network definition", 1);

	wchar_t networkFilename[256];
	swprintf_s(networkFilename, L"%ws.xml", chrName.c_str());

	MR::NetworkDef* netDef = characterDef->getNetworkDef();

	exportNetworkDefFnTables(netDef, path + L"fnTables.txt");

	g_appLog->debugMessage(MsgLevel_Info, "Exporting networkDef for c%04d\n", chrName);

	ME::NetworkDefExportXML* netDefExport = MorphemeExport::exportNetwork(netDef, animLibraryExport, messagePresetExport, chrName, path + networkFilename);

	if (!netDefExport->write())
		g_appLog->debugMessage(MsgLevel_Error, "Failed to export networkDef for c%04d\n", chrName);

	g_workerThread.load()->increaseProgressStep();
	g_workerThread.load()->increaseProgressStep();

	return true;
}

bool MorphemeEditorApp::compileAndExportTae(std::wstring path)
{
	bool status = true;

	g_workerThread.load()->addProcess("Exporting and compiling TimeAct files...", 2);

	g_workerThread.load()->setProcessStepName("Exporting TimeAct to XML...");
	status = this->exportTimeAct(path);
	g_workerThread.load()->increaseProgressStep();

	g_workerThread.load()->setProcessStepName("Compiling TimeAct files...");
	status = this->compileTimeActFiles(path);
	g_workerThread.load()->increaseProgressStep();

	return status;
}

bool MorphemeEditorApp::exportAll()
{
	wchar_t exportPath[256];

	if (this->m_character != nullptr)
	{
		g_workerThread.load()->addProcess("Exporting all", 5);

		g_workerThread.load()->setProcessStepName("Exporting model");

		swprintf_s(exportPath, L"Export\\%ws\\", this->m_character->getCharacterName().c_str());

		std::filesystem::create_directories(exportPath);

		this->exportModel(exportPath);

		g_workerThread.load()->increaseProgressStep();

		swprintf_s(exportPath, L"Export\\%ws\\", this->m_character->getCharacterName().c_str());

		std::filesystem::create_directories(exportPath);

		g_workerThread.load()->setProcessStepName("Exporting network");

		this->exportNetwork(exportPath);

		g_workerThread.load()->increaseProgressStep();

		swprintf_s(exportPath, L"Export\\%ws\\", this->m_character->getCharacterName().c_str());

		g_workerThread.load()->setProcessStepName("Exporting animations and markups");

		this->exportAnimationsAndMarkups(exportPath);

		g_workerThread.load()->increaseProgressStep();

		swprintf_s(exportPath, L"Export\\%ws\\", this->m_character->getCharacterName().c_str());

		std::filesystem::create_directories(exportPath);

		g_workerThread.load()->setProcessStepName("Exporting TimeAct");

		this->exportTimeAct(exportPath);

		g_workerThread.load()->increaseProgressStep();

		swprintf_s(exportPath, L"Export\\%ws\\", this->m_character->getCharacterName().c_str());

		std::filesystem::create_directories(exportPath);

		g_workerThread.load()->setProcessStepName("Compiling TimeAct files");

		this->compileTimeActFiles(exportPath);

		g_workerThread.load()->increaseProgressStep();
	}
	else
	{
		g_appLog->alertMessage(MsgLevel_Error, "No character is loaded");

		return false;
	}

	return true;
}

bool MorphemeEditorApp::exportAnimations(std::wstring path)
{
	MorphemeCharacterDef* characterDef = this->m_character->getMorphemeCharacterDef();

	int numAnims = characterDef->getNumAnims();

	g_workerThread.load()->addProcess("Exporting animations", numAnims);

	for (size_t i = 0; i < numAnims; i++)
	{
		std::string animName = RString::removeExtension(characterDef->getAnimationById(i)->getAnimName());
		g_workerThread.load()->setProcessStepName(animName);

		this->exportAnimation(path, i);

		g_workerThread.load()->increaseProgressStep();
	}

	return true;
}

bool MorphemeEditorApp::exportAnimMarkups(std::wstring path)
{
	MorphemeCharacterDef* characterDef = this->m_character->getMorphemeCharacterDef();

	const int numAnims = characterDef->getNumAnims();

	g_workerThread.load()->addProcess("Exporting anim markup", numAnims);

	for (size_t i = 0; i < numAnims; i++)
	{
		std::string animName = RString::removeExtension(characterDef->getAnimationById(i)->getAnimName());
		g_workerThread.load()->setProcessStepName(animName);

		this->exportAnimMarkup(path, i);

		g_workerThread.load()->increaseProgressStep();
	}

	return true;
}

void MorphemeEditorApp::exportAnimationsAndMarkups(std::wstring path)
{
	g_workerThread.load()->addProcess("Export animations", 2);

	std::wstring markupExportPath = path + L"morphemeMarkup\\";
	std::filesystem::create_directories(markupExportPath);

	g_workerThread.load()->setProcessStepName("Exporting anim markups...");
	this->exportAnimMarkups(markupExportPath);

	g_workerThread.load()->increaseProgressStep();

	std::wstring animExportPath;

	switch (this->m_taskFlags.exportFormat)
	{
	case kFbx:
		animExportPath = path + L"FBX\\";
		break;
	case kXmd:
		animExportPath = path + L"motion_xmd\\";
		break;
	default:
		break;
	}

	std::filesystem::create_directories(animExportPath);

	g_workerThread.load()->setProcessStepName("Exporting animations...");

	this->exportAnimations(animExportPath);

	g_workerThread.load()->increaseProgressStep();
}

bool MorphemeEditorApp::exportModel(std::wstring path)
{
	FlverModel* model = this->m_character->getCharacterModelCtrl()->getModel();

	if (model == nullptr)
		return false;

	g_workerThread.load()->addProcess("Exporting model", 1);

	exportFlverToMorphemeBoneMap(model, path + L"bone_map.txt");

	switch (this->m_taskFlags.exportFormat)
	{
	case MorphemeEditorApp::kFbx:
		exportModelToFbx(path, this->m_character);
		break;
	case MorphemeEditorApp::kXmd:
		exportModelToXmd(path, this->m_character);
	}

	g_workerThread.load()->increaseProgressStep();
}

bool MorphemeEditorApp::compileTimeActFiles(std::wstring path)
{
	if (this->m_character == nullptr)
		return false;

	TimeAct::TaeExport::TimeActExportXML* taeXML = this->m_character->getTimeAct();

	if (taeXML == nullptr)
		return false;

	g_workerThread.load()->addProcess("Compiling TimeAct files", 3);

	g_appLog->debugMessage(MsgLevel_Info, "Compiling TimeAct files\n");

	TimeAct::TimeAct* taePl = TimeAct::TimeAct::create(taeXML->getFileId(), taeXML->getSibFileId(), taeXML->getSkeletonId(), RString::toWide(taeXML->getSibName()), RString::toWide(taeXML->getSkeletonName()), g_taeTemplate);
	TimeAct::TimeAct* taeSfx = TimeAct::TimeAct::create(taeXML->getFileId(), taeXML->getSibFileId(), taeXML->getSkeletonId(), RString::toWide(taeXML->getSibName()), RString::toWide(taeXML->getSkeletonName()), g_taeTemplate);
	TimeAct::TimeAct* taeSnd = TimeAct::TimeAct::create(taeXML->getFileId(), taeXML->getSibFileId(), taeXML->getSkeletonId(), RString::toWide(taeXML->getSibName()), RString::toWide(taeXML->getSkeletonName()), g_taeTemplate);
	
	TimeAct::TaeExport::TimeActTrackListExportXML* trackList = taeXML->getTrackList();

	for (size_t trackIdx = 0; trackIdx < trackList->getNumTracks(); trackIdx++)
	{
		TimeAct::TaeExport::TimeActTrackExportXML* trackXML = trackList->getTrack(trackIdx);

		TimeAct::TimeActTrack* trackPl = taePl->addTimeActTrack(trackXML->getTrackId(), trackXML->getFrameCount(), trackXML->getFps());
		TimeAct::TimeActTrack* trackSfx = taeSfx->addTimeActTrack(trackXML->getTrackId(), trackXML->getFrameCount(), trackXML->getFps());
		TimeAct::TimeActTrack* trackSnd = taeSnd->addTimeActTrack(trackXML->getTrackId(), trackXML->getFrameCount(), trackXML->getFps());

		for (size_t groupIdx = 0; groupIdx < trackXML->getNumEventGroups(); groupIdx++)
		{
			TimeAct::TaeExport::TimeActGroupExportXML* groupXML = trackXML->getEventGroup(groupIdx);
			int groupId = groupXML->getGroupId();

			switch (groupId)
			{
			case 200:
				addEventGroupToTimeActTrack(trackSnd, groupXML);
				break;
			case 2100:
				addEventGroupToTimeActTrack(trackSfx, groupXML);
				break;
			default:
				addEventGroupToTimeActTrack(trackPl, groupXML);
				break;
			}
		}
	}

	std::wstring taeName = RString::toWide(taeXML->getName());

	g_workerThread.load()->setProcessStepName("pl");
	taePl->save(path + taeName + L"_pl.tae");
	g_workerThread.load()->increaseProgressStep();

	g_workerThread.load()->setProcessStepName("sfx");
	taeSfx->save(path + taeName + L"_sfx.tae");
	g_workerThread.load()->increaseProgressStep();

	g_workerThread.load()->setProcessStepName("snd");
	taeSnd->save(path + taeName + L"_snd.tae");
	g_workerThread.load()->increaseProgressStep();

	return true;
}

bool MorphemeEditorApp::exportAnimation(std::wstring path, int animId)
{
	switch (this->m_taskFlags.exportFormat)
	{
	case MorphemeEditorApp::kFbx:
		return exportAnimationToFbx(path, this->m_character, animId, true);
		break;
	case MorphemeEditorApp::kXmd:
		return exportAnimationToXmd(path, this->m_character, animId);
	default:
		return false;
	}
}

bool MorphemeEditorApp::exportAnimMarkup(std::wstring path, int animId)
{
	MorphemeCharacterDef* characterDef = this->m_character->getMorphemeCharacterDef();
	
	if (characterDef == nullptr)
		throw("characterDef was nullptr\n");

	ME::TakeListXML* takeListXML = characterDef->getAnimationById(animId)->getTakeList();
	std::string animName = RString::removeExtension(characterDef->getAnimationById(animId)->getAnimName());

	if (takeListXML)
	{
		g_appLog->debugMessage(MsgLevel_Info, "Exporting animation markup for animation %s (%ws)\n", animName.c_str(), this->m_character->getCharacterName().c_str());

		if (!takeListXML->write())
		{
			g_appLog->debugMessage(MsgLevel_Error, "Failed to export take list for animation %d\n", animId);

			return false;
		}
	}

	return true;
}

void MorphemeEditorApp::exportTaeTemplateXML()
{
	TimeAct::TaeTemplateXML* templateXML = TimeAct::TaeTemplateXML::create();

	for (size_t groupIdx = 0; groupIdx < g_taeTemplate->groups.size(); groupIdx++)
	{
		TimeAct::TaeTemplate::Group group = g_taeTemplate->groups[groupIdx];

		TimeAct::TaeTemplateEventGroupXML* groupTemplateXML = templateXML->addEventGroup(group.name, group.id);

		for (size_t eventIdx = 0; eventIdx < group.events.size(); eventIdx++)
		{
			TimeAct::TaeTemplate::Group::Event event = group.events[eventIdx];

			TimeAct::TaeTemplateEventXML* eventTemplateXML = groupTemplateXML->addEventTemplate(event.name, event.id);

			for (size_t argIdx = 0; argIdx < event.args.size(); argIdx++)
			{
				TimeAct::TaeTemplate::Group::Event::Arg arg = event.args[argIdx];

				TimeAct::TaeTemplateEventArgumentXML* argTemplateXML = eventTemplateXML->addArgumentTemplate(arg.name, arg.type);

				for (size_t valueIdx = 0; valueIdx < arg.values.size(); valueIdx++)
				{
					TimeAct::TaeTemplate::Group::Event::Arg::ArgEnumValue enumValue = arg.values[valueIdx];

					TimeAct::TaeTemplateArgEnumXML* argEnumXML = argTemplateXML->addEnumValue(enumValue.name, enumValue.value);
				}
			}
		}
	}

	templateXML->setDstFileName("Export\\TimeActTemplate.xml");
	templateXML->save();
}