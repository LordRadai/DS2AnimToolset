#include "MorphemeEditorApp.h"
#include "extern.h"
#include "RenderManager/RenderManager.h"
#include "WorkerThread/WorkerThread.h"
#include "FileTranslator/FileTranslator.h"
#include "FromSoftware/TimeAct/TaeExport/TaeExport.h"
#include "FromSoftware/TimeAct/TaeTemplate/TaeTemplateXML/TaeTemplateXML.h"
#include "MorphemeSystem/MorphemeDecompiler/Node/NodeUtils.h"
#include "utils/utils.h"
#include <thread>

#ifndef _DEBUG
#define ASSET_COMPILER_EXE "morphemeAssetCompiler.exe"
#else
#define ASSET_COMPILER_EXE "morphemeAssetCompiler_debug.exe"
#endif

namespace
{
	std::string getExeRootDir()
	{
		char filePath[256];
		GetModuleFileNameA(NULL, filePath, 256);

		return std::filesystem::path(filePath).parent_path().string();
	}

	float calcTimeActEditorCurrentTime(TrackEditor::EventTrackEditor* eventTrackEditor, float timeActTrackLenght, float fallbackTimeVal)
	{
		TrackEditor::Track* timeActTrack = nullptr;

		for (size_t i = 0; i < eventTrackEditor->getNumTracks(); i++)
		{
			TrackEditor::Track* track = eventTrackEditor->getTrack(i);
			
			if ((track->userData == 1000) && (strcmp(track->name, "TimeAct") == 0))
			{
				timeActTrack = track;
				break;
			}
		}

		if (timeActTrack)
		{
			const float currentTime = eventTrackEditor->getCurrentTime();

			const float startTime = RMath::frameToTime(timeActTrack->events[0]->frameStart, eventTrackEditor->getFps());
			const float endTime = RMath::frameToTime(timeActTrack->events[0]->frameEnd, eventTrackEditor->getFps());
			float eventFraction = 0.f;

			if (currentTime < startTime)
				eventFraction = 0.f;
			else if (currentTime > endTime)
				eventFraction = 1.f;
			else
				eventFraction = (currentTime - startTime) / (endTime - startTime);

			return eventFraction * timeActTrackLenght;
		}

		return fallbackTimeVal;
	}

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

	void dumpNetworkTaskQueuingFnTables(MR::NetworkDef* netDef, std::wstring path)
	{
		std::ofstream out(path, std::ios::out);

		char tableBuf[256];
		char lineBuf[256];

		int numTables = netDef->getTaskQueuingFnTables()->getNumTaskFnTables();
		for (int i = 0; i < numTables; i++)
		{
			const MR::SharedTaskFnTables::SharedTaskFn* table = netDef->getTaskQueuingFnTables()->getTaskFnTable(i);

			sprintf_s(tableBuf, "TaskQueuingFnTable_%d:\n", i);
			out << tableBuf;

			for (int sem = 0; sem < MR::Manager::getInstance().getNumRegisteredAttribSemantics(); sem++)
			{
				const char* fnName = MR::Manager::getInstance().getTaskQueuingFnName(MR::QueueAttrTaskFn(table[sem]));
				int fnID = -1;

				if (fnName)
					fnID = MR::Manager::getInstance().getTaskQueuingFnID(fnName);

				sprintf_s(lineBuf, "\t%s (%d) -> %s (%d)\n", MR::Manager::getInstance().getAttributeSemanticName(sem), sem, fnName, fnID);
				out << lineBuf;
			}

			out << "\n";
		}

		out.close();
	}

	void dumpNetworkOutputCPTasksFnTables(MR::NetworkDef* netDef, std::wstring path)
	{
		std::ofstream out(path, std::ios::out);

		char tableBuf[256];
		char lineBuf[256];

		int numTables = netDef->getOutputCPTaskFnTables()->getNumTaskFnTables();
		for (int i = 0; i < numTables; i++)
		{
			const MR::SharedTaskFnTables::SharedTaskFn* table = netDef->getOutputCPTaskFnTables()->getTaskFnTable(i);

			sprintf_s(tableBuf, "OutputCPTaskFnTable_%d:\n", i);
			out << tableBuf;

			for (int sem = 0; sem < MR::Manager::getInstance().getNumRegisteredAttribSemantics(); sem++)
			{
				const char* fnName = MR::Manager::getInstance().getOutputCPTaskName(MR::OutputCPTask(table[sem]));
				int fnID = -1;

				if (fnName)
					fnID = MR::Manager::getInstance().getTaskQueuingFnID(fnName);

				sprintf_s(lineBuf, "\t%s (%d) -> %s (%d)\n", MR::Manager::getInstance().getAttributeSemanticName(sem), sem, fnName, fnID);
				out << lineBuf;
			}

			out << "\n";
		}

		out.close();
	}

	void printNodeFnTables(MR::NetworkDef* netDef, MR::NodeID nodeID, std::ofstream& file)
	{
		const MR::NodeDef* nodeDef = netDef->getNodeDef(nodeID);

		char lineBuf[256];
		sprintf_s(lineBuf, "Node_%d (taskQueuingFnTablesID=%d, outputCPTasksFnTablesID=%d)\n", nodeID, nodeDef->getTaskQueuingFnsID(), nodeDef->getOutputCPTasksID());

		file << lineBuf;
	}

	tinyxml2::XMLElement* createNodeXML(MR::NodeDef* nodeDef, tinyxml2::XMLElement* parent, std::string nodeName, bool activeState)
	{
		tinyxml2::XMLElement* nodeXML = parent->InsertNewChildElement("Node");

		if (nodeDef != nullptr)
		{
			nodeXML->SetAttribute("Name", nodeName.c_str());
			nodeXML->SetAttribute("NodeID", nodeDef->getNodeID());
			nodeXML->SetAttribute("NodeFlags", nodeDef->getNodeFlags());
			nodeXML->SetAttribute("TypeID", nodeDef->getNodeTypeID());
			nodeXML->SetAttribute("ParentID", nodeDef->getParentNodeID());

			return nodeXML;
		}

		if (activeState)
		{
			nodeXML->SetAttribute("Name", "ActiveState");
			nodeXML->SetAttribute("NodeID", -1);
			nodeXML->SetAttribute("NodeFlags", 0);
			nodeXML->SetAttribute("TypeID", 0);
			nodeXML->SetAttribute("ParentID", 0);

			return nodeXML;
		}

		nodeXML->SetAttribute("Name", "None");
		nodeXML->SetAttribute("NodeID", -1);
		nodeXML->SetAttribute("NodeFlags", 0);
		nodeXML->SetAttribute("TypeID", 0);
		nodeXML->SetAttribute("ParentID", 0);

		return nodeXML;
	}

	void addNodeToXML(MR::NetworkDef* netDef, MR::NodeID nodeID, ME::AnimationLibraryExport* animLibrary, tinyxml2::XMLElement* parent, bool isInput)
	{
		MR::NodeDef* nodeDef = netDef->getNodeDef(nodeID);

		const bool isContainer = ((nodeDef->getNodeTypeID() == NODE_TYPE_STATE_MACHINE) || (nodeDef->getNodeTypeID() == NODE_TYPE_NETWORK));

		std::string nodeName = MD::NodeUtils::buildNodeName(netDef, nodeDef, animLibrary);

		tinyxml2::XMLElement* nodeXML = createNodeXML(nodeDef, parent, nodeName, false);

		if (!isContainer)
		{
			const int numChildNodes = nodeDef->getNumChildNodes();
			const int numInputCPs = nodeDef->getNumInputCPConnections();

			if (numChildNodes > 0)
			{
				tinyxml2::XMLElement* inputNodes = nodeXML->InsertNewChildElement("InputNodes");

				for (int i = 0; i < numChildNodes; i++)
				{
					if (nodeDef->getChildNodeID(i) != MR::INVALID_NODE_ID)
					{
						std::string name = MD::NodeUtils::buildNodeName(netDef, nodeDef->getChildNodeDef(i), animLibrary);
						createNodeXML(nodeDef->getChildNodeDef(i), inputNodes, name, false);
					}
					else
					{
						if ((nodeDef->getNodeTypeID() == NODE_TYPE_TRANSIT) || (nodeDef->getNodeTypeID() == NODE_TYPE_TRANSIT_SYNC_EVENTS))
							createNodeXML(nullptr, inputNodes, "", true);
						else
							createNodeXML(nullptr, inputNodes, "", false);
					}
				}
			}

			if (numInputCPs > 0)
			{
				tinyxml2::XMLElement* inputCPs = nodeXML->InsertNewChildElement("InputCPs");

				for (int i = 0; i < numInputCPs; i++)
				{
					if (nodeDef->getInputCPConnectionSourceNodeID(i) != MR::INVALID_NODE_ID)
					{
						std::string name = MD::NodeUtils::buildNodeName(netDef, nodeDef->getInputCPConnectionSourceNodeDef(i), animLibrary);
						createNodeXML(nodeDef->getInputCPConnectionSourceNodeDef(i), inputCPs, name, false);
					}
					else
					{
						createNodeXML(nullptr, inputCPs, "", false);
					}
				}
			}
		}
		else
		{
			const int numChildNodes = nodeDef->getNumChildNodes();

			if (numChildNodes > 0)
			{
				tinyxml2::XMLElement* childNodes = nodeXML->InsertNewChildElement("Children");

				for (int i = 0; i < numChildNodes; i++)
				{
					if (nodeDef->getChildNodeID(i) != MR::INVALID_NODE_ID)
					{
						std::string name = MD::NodeUtils::buildNodeName(netDef, nodeDef->getChildNodeDef(i), animLibrary);
						createNodeXML(nodeDef->getChildNodeDef(i), childNodes, name, false);
					}
					else
					{
						createNodeXML(nullptr, childNodes, "", false);
					}
				}
			}
		}
	}

	void addNamedNodeToXML(MR::NetworkDef* netDef, MR::NodeID nodeID, ME::AnimationLibraryExport* animLibrary, tinyxml2::XMLElement* parent, std::vector<MR::NodeID>& processedNodes)
	{
		for (size_t i = 0; i < processedNodes.size(); i++)
		{
			if (nodeID == processedNodes[i])
				return;
		}

		processedNodes.push_back(nodeID);

		MR::NodeDef* nodeDef = netDef->getNodeDef(nodeID);

		std::string nodeName = netDef->getNodeNameFromNodeID(nodeID);

		if (nodeName == "")
			return;

		tinyxml2::XMLElement* nodeXML = createNodeXML(nodeDef, parent, netDef->getNodeNameFromNodeID(nodeID), false);

		const int numChildNodes = nodeDef->getNumChildNodes();
		const int numInputCPs = nodeDef->getNumInputCPConnections();

		if (numChildNodes > 0)
		{
			for (int i = 0; i < numChildNodes; i++)
			{
				if (nodeDef->getChildNodeID(i) != MR::INVALID_NODE_ID)
					addNamedNodeToXML(netDef, nodeDef->getChildNodeID(i), animLibrary, nodeXML, processedNodes);
			}
		}
	}

	void printNode(MR::NetworkDef* netDef, MR::NodeID nodeID, ME::AnimationLibraryExport* animLibrary, std::ofstream& file, int numIndents, std::unordered_map<MR::NodeID, std::string>& nodeMap, bool isInput)
	{
		MR::NodeDef* nodeDef = netDef->getNodeDef(nodeID);

		std::string indendationString = "";

		for (size_t i = 0; i < numIndents; i++)
			indendationString += "\t";

		bool alreadyExported = false;

		if (nodeMap.find(nodeID) != nodeMap.end())
		{
			alreadyExported = true;

			if (!isInput)
				file << indendationString + nodeMap[nodeID];
			else
				file << indendationString + "-" + nodeMap[nodeID];
		}

		const bool isContainer = ((nodeDef->getNodeTypeID() == NODE_TYPE_STATE_MACHINE) || (nodeDef->getNodeTypeID() == NODE_TYPE_NETWORK));

		if (!alreadyExported)
		{
			std::string nodeName = MD::NodeUtils::buildNodeName(netDef, nodeDef, animLibrary);

			char nodeBuf[256];

			if ((nodeDef->getNodeTypeID() == NODE_TYPE_TRANSIT) || (nodeDef->getNodeTypeID() == NODE_TYPE_TRANSIT_SYNC_EVENTS))
			{
				sprintf_s(nodeBuf, "%s\"%s\" (src=%d, dst=%d)\n", indendationString.c_str(), nodeName.c_str(), nodeDef->getChildNodeID(0), nodeDef->getChildNodeID(1));
				file << nodeBuf;

				return;
			}

			if (!isInput)
				sprintf_s(nodeBuf, "\"%s\" (parentNode=%d, typeID=%d)\n", nodeName.c_str(), nodeDef->getParentNodeID(), nodeDef->getNodeTypeID());
			else
				sprintf_s(nodeBuf, "\"%s\" (parentNode=%d, typeID=%d)\n", nodeName.c_str(), nodeDef->getParentNodeID(), nodeDef->getNodeTypeID());

			nodeMap.insert(std::make_pair(nodeID, std::string(nodeBuf)));

			if (!isInput)
				file << indendationString + nodeBuf;
			else
				file << indendationString + "-" + nodeBuf;
		}
		
		if (isContainer)
			file << indendationString + "{\n";

		for (uint32_t i = 0; i < nodeDef->getNumChildNodes(); i++)
		{
			if (isContainer)
				printNode(netDef, nodeDef->getChildNodeID(i), animLibrary, file, numIndents + 1, nodeMap, false);
			else
				printNode(netDef, nodeDef->getChildNodeID(i), animLibrary, file, numIndents + 1, nodeMap, true);
		}

		for (uint32_t i = 0; i < nodeDef->getNumInputCPConnections(); i++)
			printNode(netDef, nodeDef->getInputCPConnectionSourceNodeID(i), animLibrary, file, numIndents + 1, nodeMap, true);

		if (isContainer)
			file << indendationString + "}\n";
	}

	void dumpNetworkNodes(MR::NetworkDef* netDef, ME::AnimationLibraryExport* animLibrary, std::wstring path)
	{
		std::ofstream nodeDumpExt(L"nodeFnTables.txt", std::ios::out);

		tinyxml2::XMLDocument xmlDoc;
		tinyxml2::XMLElement* root = xmlDoc.NewElement("Nodes");

		const int numNodes = netDef->getNumNodeDefs();

		for (int i = 0; i < numNodes; i++)
		{
			addNodeToXML(netDef, i, animLibrary, root, false);
			printNodeFnTables(netDef, i, nodeDumpExt);
		}

		xmlDoc.InsertEndChild(root);
		xmlDoc.SaveFile(RString::toNarrow(path).c_str());
	}

	void dumpNamedNodes(MR::NetworkDef* netDef, ME::AnimationLibraryExport* animLibrary, std::wstring path)
	{
		tinyxml2::XMLDocument xmlDoc;
		tinyxml2::XMLElement* root = xmlDoc.NewElement("Nodes");

		const int numNodes = netDef->getNumNodeDefs();

		std::vector<MR::NodeID> processedNodes;

		for (int i = 0; i < numNodes; i++)
			addNamedNodeToXML(netDef, i, animLibrary, root, processedNodes);

		xmlDoc.InsertEndChild(root);
		xmlDoc.SaveFile(RString::toNarrow(path).c_str());
	}

	void dumpNodeIDNamesTable(MR::NetworkDef* netDef, ME::AnimationLibraryExport* animLibrary, std::wstring path)
	{
		tinyxml2::XMLDocument xmlDoc;
		tinyxml2::XMLElement* root = xmlDoc.NewElement("NodeIDNamesTable");

		const int numNodes = netDef->getNumNodeDefs();

		const NMP::IDMappedStringTable* nodeIDNamesTable = netDef->getNodeIDNamesTable();

		for (size_t i = 0; i < nodeIDNamesTable->getNumEntries(); i++)
		{
			const MR::NodeDef* nodeDef = netDef->getNodeDef(nodeIDNamesTable->getEntryID(i));

			tinyxml2::XMLElement* elem = root->InsertNewChildElement("Entry");
			elem->SetAttribute("NodeID", nodeIDNamesTable->getEntryID(i));
			elem->SetAttribute("NodeTypeID", nodeDef->getNodeTypeID());
			elem->SetAttribute("Name", nodeIDNamesTable->getEntryString(i));
		}

		xmlDoc.InsertEndChild(root);
		xmlDoc.SaveFile(RString::toNarrow(path).c_str());
	}

	void exportAssetCompilerCommand(const char* command, std::wstring path)
	{
		std::ofstream out(path, std::ios::out);
		out << command;
		out.close();
	}

	bool exportModelToFbx(Character* character)
	{
		bool status = true;

		g_appLog->debugMessage(MsgLevel_Info, "Exporting model to FBX for character %s\n", character->getCharacterName().c_str());

		FbxExporter* pExporter = FbxExporter::Create(g_pFbxManager, "Flver Exporter");
		pExporter->SetFileExportVersion(FBX_2014_00_COMPATIBLE);

		std::string modelOutPath = RString::toNarrow(character->getCharacterName()) + ".fbx";

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

	bool exportModelToXmd(Character* character)
	{
		bool status = true;

		g_appLog->debugMessage(MsgLevel_Info, "Exporting model to XMD for character %ws\n", character->getCharacterName().c_str());

		FlverModel* model = character->getCharacterModelCtrl()->getModel();
		MR::AnimRigDef* rig = character->getRig(0);

		XMD::XModel* modelXmd = XMDTranslator::createModel(rig, model, RString::toNarrow(model->getFileOrigin()).c_str(), true);
		XMDTranslator::createBindPoseAnimCycle(modelXmd, rig);

		if (modelXmd->Save(RString::toNarrow(character->getCharacterName()) + ".xmd") != XMD::XFileError::Success)
			status = false;

		delete modelXmd;

		return status;
	}

	bool exportModelToGltf(Character* character)
	{
		FlverModel* model = character->getCharacterModelCtrl()->getModel();
		MR::AnimRigDef* rig = character->getRig(0);

		tinygltf::Model* gltfModel = GLTFTranslator::createModel(rig, model, true);

		tinygltf::Scene scene;
		scene.nodes.push_back(0); // Root node of the model

		gltfModel->scenes.push_back(scene);
		gltfModel->defaultScene = 0;

		std::string modelOutPath = RString::toNarrow(character->getCharacterName()) + ".gltf";

		tinygltf::TinyGLTF gltfWriter;
		bool success = gltfWriter.WriteGltfSceneToFile(gltfModel, modelOutPath, false, true, true, false);

		delete gltfModel;

		return success;
	}

	bool exportAnimationToFbx(std::wstring path, Character* character, int animSetIdx, int animIdx, int fps, bool addModel)
	{
		bool status = true;

		MorphemeCharacterDef* characterDef = character->getMorphemeCharacterDef();
		AnimObject* anim = characterDef->getAnimation(0, animIdx);

		int animId = anim->getAnimID();

		if (characterDef == nullptr)
			throw("characterDef was nullptr\n");

		std::string animName = RString::toNarrow(path) + RString::removeExtension(characterDef->getAnimFileLookUp()->getSourceFilename(animId));

		g_appLog->debugMessage(MsgLevel_Info, "\tExporting animation \"%s\" to FBX (%ws)\n", animName.c_str(), character->getCharacterName().c_str());

		FbxExporter* pExporter = FbxExporter::Create(g_pFbxManager, "Flver Exporter");
		pExporter->SetFileExportVersion(FBX_2014_00_COMPATIBLE);

		std::string outPath = animName + ".fbx";

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

		if (!FBXTranslator::createFbxTake(pScene, morphemeRig, characterDef->getAnimationById(animSetIdx, animId), characterDef->getAnimFileLookUp()->getTakeName(animId), fps))
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

	bool exportAnimationToXmd(std::wstring path, Character* character, int animSetIdx, int animIdx, int fps)
	{
		bool status = true;

		MorphemeCharacterDef* characterDef = character->getMorphemeCharacterDef();
		AnimObject* anim = characterDef->getAnimation(animSetIdx, animIdx);

		int animId = anim->getAnimID();

		if (!anim->isLoaded())
			anim = characterDef->getAnimation(0, 0);

		if (!anim->isLoaded())
			return false;

		MR::AnimRigDef* rig = character->getRig(0);

		std::string animName = RString::toNarrow(path) + RString::removeExtension(characterDef->getAnimFileLookUp()->getSourceFilename(animId)) + ".xmd";

		g_appLog->debugMessage(MsgLevel_Info, "\tExporting animation \"%s\" to XMD (%ws)\n", animName.c_str(), character->getCharacterName().c_str());

		XMD::XModel* xmd = XMDTranslator::createModel(rig, character->getCharacterModelCtrl()->getModel(), characterDef->getAnimFileLookUp()->getFilename(animId), false);
		XMDTranslator::createAnimCycle(xmd, anim, characterDef->getAnimFileLookUp()->getTakeName(animId), fps);

		if (xmd->Save(animName) != XMD::XFileError::Success)
			status = false;

		delete xmd;

		return status;
	}

	bool exportAnimationToGltf(std::wstring path, Character* character, int animSetIdx, int animIdx, int fps)
	{
		return false;
	}

	void addEventGroupToTimeActTrack(TimeAct::TimeActTrack* dst, TimeAct::TaeExport::TimeActGroupExportXML* groupXML)
	{
		TimeAct::EventGroup* group = dst->addGroup(groupXML->getGroupId());

		for (size_t eventIdx = 0; eventIdx < groupXML->getNumEvents(); eventIdx++)
		{
			TimeAct::TaeExport::TimeActEventExportXML* eventXML = groupXML->getEvent(eventIdx);

			TimeAct::TimeActEvent* event = group->addEvent(eventXML->getStartTime(), eventXML->getEndTime(), eventXML->getEventId(), g_taeTemplate);

			if (eventXML->getNumArguments() != event->getNumArguments())
				throw("Argument count mismatch\n");

			for (size_t argIdx = 0; argIdx < eventXML->getNumArguments(); argIdx++)
			{
				TimeAct::TaeExport::TimeActArgumentExportXML* argXML = eventXML->getArgument(argIdx);
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

	ME::EventTrackExport* getExportedTrack(std::vector<ME::EventTrackExport*>& exportedTracks, ME::EventTrackExport* track)
	{
		for (size_t i = 0; i < exportedTracks.size(); i++)
		{
			if ((exportedTracks[i]->getEventTrackType() == track->getEventTrackType()) &&
				(exportedTracks[i]->getUserData() == track->getUserData()) &&
				(exportedTracks[i]->getEventTrackChannelID() == track->getEventTrackChannelID()) &&
				(exportedTracks[i]->getNumEvents() == track->getNumEvents()) &&
				(strcmp(exportedTracks[i]->getName(), track->getName()) == 0))
			{
				ME::EventTrackExport* targetTrack = exportedTracks[i];

				switch (track->getEventTrackType())
				{
				case ME::EventTrackExport::EVENT_TRACK_TYPE_DISCRETE:
				{
					ME::DiscreteEventTrackExport* trackDisc = static_cast<ME::DiscreteEventTrackExport*>(track);
					ME::DiscreteEventTrackExport* exportedTrack = static_cast<ME::DiscreteEventTrackExport*>(exportedTracks[i]);

					for (size_t eventIdx = 0; eventIdx < exportedTrack->getNumEvents(); eventIdx++)
					{
						ME::DiscreteEventExport* exportedEvent = exportedTrack->getEvent(eventIdx);
						ME::DiscreteEventExport* event = trackDisc->getEvent(eventIdx);

						if ((exportedEvent->getNormalisedTime() != event->getNormalisedTime()) ||
							(exportedEvent->getUserData() != event->getUserData()))
						{
							targetTrack = nullptr;
						}
					}
					break;
				}
				case ME::EventTrackExport::EVENT_TRACK_TYPE_DURATION:
				{
					ME::DurationEventTrackExport* trackDisc = static_cast<ME::DurationEventTrackExport*>(track);
					ME::DurationEventTrackExport* exportedTrack = static_cast<ME::DurationEventTrackExport*>(exportedTracks[i]);

					for (size_t eventIdx = 0; eventIdx < exportedTrack->getNumEvents(); eventIdx++)
					{
						ME::DurationEventExport* exportedEvent = exportedTrack->getEvent(eventIdx);
						ME::DurationEventExport* event = trackDisc->getEvent(eventIdx);

						if ((exportedEvent->getNormalisedStartTime() != event->getNormalisedStartTime()) ||
							(exportedEvent->getNormalisedDuration() != event->getNormalisedDuration()) ||
							(exportedEvent->getUserData() != event->getUserData()))
						{
							targetTrack = nullptr;
						}
					}
					break;
				}
				case ME::EventTrackExport::EVENT_TRACK_TYPE_CURVE:
				{
					ME::CurveEventTrackExport* trackDisc = static_cast<ME::CurveEventTrackExport*>(track);
					ME::CurveEventTrackExport* exportedTrack = static_cast<ME::CurveEventTrackExport*>(exportedTracks[i]);

					for (size_t eventIdx = 0; eventIdx < exportedTrack->getNumEvents(); eventIdx++)
					{
						ME::CurveEventExport* exportedEvent = exportedTrack->getEvent(eventIdx);
						ME::CurveEventExport* event = trackDisc->getEvent(eventIdx);

						if ((exportedEvent->getNormalisedStartTime() != event->getNormalisedStartTime()) ||
							(exportedEvent->getFloatValue() != event->getFloatValue()) ||
							(exportedEvent->getUserData() != event->getUserData()))
						{
							targetTrack = nullptr;
						}
					}
					break;
				}
				}

				if (targetTrack)
					return targetTrack;
			}
		}

		return nullptr;
	}

	ME::TakeListXML* createOptimisedTakeList(ME::TakeListXML* takeList, std::vector<ME::EventTrackExport*>& exportedTracks)
	{
		ME::ExportFactoryXML exportFactory;
		ME::TakeListXML* optimisedTakeList = static_cast<ME::TakeListXML*>(exportFactory.createTakeList(RString::toWide(takeList->getSourceAnimFilename()).c_str(), RString::toWide(takeList->getDestFilename()).c_str()));
	
		for (size_t takeIdx = 0; takeIdx < takeList->getNumTakes(); takeIdx++)
		{
			ME::TakeExport* originalTake = takeList->getTake(takeIdx);
			ME::TakeExportXML* optimisedTake = static_cast<ME::TakeExportXML*>(optimisedTakeList->createTake(RString::toWide(originalTake->getName()).c_str(), originalTake->getCachedTakeSecondsDuration(), originalTake->getCachedTakeFPS(), originalTake->getLoop(), originalTake->getClipStart(), originalTake->getClipEnd()));
		
			for (size_t eventTrackIdx = 0; eventTrackIdx < originalTake->getNumEventTracks(); eventTrackIdx++)
			{	
				switch (originalTake->getEventTrack(eventTrackIdx)->getEventTrackType())
				{
				case ME::EventTrackExport::EVENT_TRACK_TYPE_DISCRETE:
				{
					ME::DiscreteEventTrackExport* originalEventTrack = static_cast<ME::DiscreteEventTrackExport*>(originalTake->getEventTrack(eventTrackIdx));
					std::string guid = originalEventTrack->getGUID();

					ME::EventTrackExport* targetTrack = getExportedTrack(exportedTracks, originalEventTrack);
					//ME::EventTrackExport* targetTrack = nullptr;

					if (targetTrack)
						guid = targetTrack->getGUID();

					ME::DiscreteEventTrackExportXML* optimisedTrack = static_cast<ME::DiscreteEventTrackExportXML*>(optimisedTake->createEventTrack(originalEventTrack->getEventTrackType(), guid.c_str(), RString::toWide(originalEventTrack->getName()).c_str(), originalEventTrack->getEventTrackChannelID(), originalEventTrack->getUserData()));

					for (size_t eventIdx = 0; eventIdx < originalEventTrack->getNumEvents(); eventIdx++)
					{
						ME::DiscreteEventExport* originalEvent = originalEventTrack->getEvent(eventIdx);
						optimisedTrack->createEvent(originalEvent->getIndex(), originalEvent->getNormalisedTime(), originalEvent->getUserData());
					}

					if (targetTrack == nullptr)
						exportedTracks.push_back(originalEventTrack);

					break;
				}
				case ME::EventTrackExport::EVENT_TRACK_TYPE_DURATION:
				{
					ME::DurationEventTrackExport* originalEventTrack = static_cast<ME::DurationEventTrackExport*>(originalTake->getEventTrack(eventTrackIdx));
					std::string guid = originalEventTrack->getGUID();

					ME::EventTrackExport* targetTrack = getExportedTrack(exportedTracks, originalEventTrack);

					if (targetTrack)
						guid = targetTrack->getGUID();

					ME::DurationEventTrackExportXML* optimisedTrack = static_cast<ME::DurationEventTrackExportXML*>(optimisedTake->createEventTrack(originalEventTrack->getEventTrackType(), guid.c_str(), RString::toWide(originalEventTrack->getName()).c_str(), originalEventTrack->getEventTrackChannelID(), originalEventTrack->getUserData()));

					for (size_t eventIdx = 0; eventIdx < originalEventTrack->getNumEvents(); eventIdx++)
					{
						ME::DurationEventExport* originalEvent = originalEventTrack->getEvent(eventIdx);
						optimisedTrack->createEvent(originalEvent->getIndex(), originalEvent->getNormalisedStartTime(), originalEvent->getNormalisedDuration(), originalEvent->getUserData());
					}

					if (targetTrack == nullptr)
						exportedTracks.push_back(originalEventTrack);

					break;
				}
				case ME::EventTrackExport::EVENT_TRACK_TYPE_CURVE:
				{
					ME::CurveEventTrackExport* originalEventTrack = static_cast<ME::CurveEventTrackExport*>(originalTake->getEventTrack(eventTrackIdx));
					std::string guid = originalEventTrack->getGUID();

					ME::EventTrackExport* targetTrack = getExportedTrack(exportedTracks, originalEventTrack);

					if (targetTrack)
						guid = targetTrack->getGUID();

					ME::CurveEventTrackExportXML* optimisedTrack = static_cast<ME::CurveEventTrackExportXML*>(optimisedTake->createEventTrack(originalEventTrack->getEventTrackType(), guid.c_str(), RString::toWide(originalEventTrack->getName()).c_str(), originalEventTrack->getEventTrackChannelID(), originalEventTrack->getUserData()));

					for (size_t eventIdx = 0; eventIdx < originalEventTrack->getNumEvents(); eventIdx++)
					{
						ME::CurveEventExport* originalEvent = originalEventTrack->getEvent(eventIdx);
						optimisedTrack->createEvent(originalEvent->getIndex(), originalEvent->getNormalisedStartTime(), originalEvent->getFloatValue(), originalEvent->getUserData());
					}

					if (targetTrack == nullptr)
						exportedTracks.push_back(originalEventTrack);

					break;
				}				
				default:
					break;
				}
			}
		}

		return optimisedTakeList;
	}

	const char* getAnimFormatName(uint8_t animFormat)
	{
		switch (animFormat)
		{
		case ANIM_TYPE_MBA:
			return "mba";
		case ANIM_TYPE_ASA:
			return "asa";
		case ANIM_TYPE_NSA:
			return "nsa";
		case ANIM_TYPE_QSA:
			return "qsa";
		default:
			return "nsa";
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

			this->m_eventTrackEditor->setTimeCodeFormat(this->m_timeActEditor->getTimeCodeFormat());
		}

		const float timeActEditorTime = calcTimeActEditorCurrentTime(this->m_eventTrackEditor, RMath::frameToTime(this->m_timeActEditor->getFrameMax(), this->m_timeActEditor->getFps()), this->m_animPlayer->getTime());

		if (this->m_timeActEditor && this->m_timeActEditor->getSource())
		{
			if (!this->m_animPlayer->isPaused())
				this->m_timeActEditor->setCurrentTime(timeActEditorTime);
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
			modelCtrl->setDrawDummies(this->m_previewFlags.drawDummies);
			modelCtrl->setDrawMeshes(this->m_previewFlags.drawMeshes);
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

	if (this->m_taskFlags.exportAll)
	{
		this->m_taskFlags.exportAll = false;

		wchar_t exportPath[256];
		swprintf_s(exportPath, L"Export\\%ws", this->m_character->getCharacterName().c_str());

		std::filesystem::create_directories(exportPath);

		g_workerThread.load()->startThread("Export All", &MorphemeEditorApp::exportAll, this, std::wstring(exportPath));
	}

	if (this->m_taskFlags.exportAndProcess)
	{
		this->m_taskFlags.exportAndProcess = false;

		wchar_t exportPath[256];
		swprintf_s(exportPath, L"Export\\%ws", this->m_character->getCharacterName().c_str());

		std::filesystem::create_directories(exportPath);

		g_workerThread.load()->startThread("Export and Process", &MorphemeEditorApp::exportAndProcess, this, std::wstring(exportPath));
	}

	if (this->m_taskFlags.exportTae)
	{
		this->m_taskFlags.exportTae = false;

		wchar_t exportPath[256];
		swprintf_s(exportPath, L"Export\\%ws", this->m_character->getCharacterName().c_str());

		std::filesystem::create_directories(exportPath);

		g_workerThread.load()->startThread("Export TimeAct", &MorphemeEditorApp::exportAndCompileTae, this, std::wstring(exportPath));
	}

	if (this->m_taskFlags.exportModel)
	{
		this->m_taskFlags.exportModel = false;

		if (this->m_character != nullptr)
		{
			wchar_t exportPath[256];
			swprintf_s(exportPath, L"Export\\%ws", this->m_character->getCharacterName().c_str());

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
			swprintf_s(exportPath, L"Export\\%ws", this->m_character->getCharacterName().c_str());

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
			swprintf_s(exportPath, L"Export\\%ws", this->m_character->getCharacterName().c_str());
			
			std::filesystem::create_directories(exportPath);

			g_workerThread.load()->startThread("Export Animations", &MorphemeEditorApp::exportAnimationsAndMarkups, this, std::wstring(exportPath));
		}
		else
		{
			g_appLog->alertMessage(MsgLevel_Error, "No character is loaded");
		}
	}

	if (this->m_taskFlags.compileNetwork)
	{
		this->m_taskFlags.compileNetwork = false;

		wchar_t exportPath[256];
		swprintf_s(exportPath, L"Export\\%ws", this->m_character->getCharacterName().c_str());

		std::filesystem::create_directories(exportPath);

		g_workerThread.load()->startThread("Compile Morpheme Assets", &MorphemeEditorApp::compileMorphemeAssets, this, std::wstring(exportPath));
	}

	if (this->m_taskFlags.compileTaes)
	{
		this->m_taskFlags.compileTaes = false;

		wchar_t exportPath[256];
		swprintf_s(exportPath, L"Export\\%ws", this->m_character->getCharacterName().c_str());

		std::filesystem::create_directories(exportPath);

		g_workerThread.load()->startThread("Compile TimeAct", &MorphemeEditorApp::compileTimeActFiles, this, std::wstring(exportPath));
	}

#ifdef DEBUG
	if (this->m_taskFlags.exportTaeTemplateXml)
	{
		this->m_taskFlags.exportTaeTemplateXml = false;

		g_workerThread.load()->startThread("Export TimeAct Template", &MorphemeEditorApp::exportTaeTemplateXML, this);
	}
#endif
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

	this->m_exportSettings.exportFormat = (ExportFormat)settings->getInt("Export", "export_format", 0);
	this->m_exportSettings.compressionFormat = settings->getInt("Export", "compression_format", 2);
	this->m_exportSettings.useSourceSampleFrequency = settings->getBool("Export", "compression_use_source_sample_frequency", true);
	this->m_exportSettings.sampleFrequency = settings->getInt("Export", "compression_sample_frequency", 30);

	this->m_previewFlags.drawDummies = settings->getBool("ModelViewer", "draw_dummies", false);
	this->m_previewFlags.drawMeshes = settings->getBool("ModelViewer", "draw_meshes", true);
	this->m_previewFlags.drawBones = settings->getBool("ModelViewer", "draw_bones", true);
	this->m_previewFlags.displayMode = (DisplayMode)settings->getInt("ModelViewer", "model_disp_mode", 0);

	this->m_timeActEditor->setTimeCodeFormat((TrackEditor::TimeCodeFormat)settings->getInt("TimeActEditor", "time_code_format", 0));
	this->m_eventTrackEditor->setTimeCodeFormat((TrackEditor::TimeCodeFormat)settings->getInt("EventTrackEditor", "time_code_format", 0));

	settings->destroy();
}

void MorphemeEditorApp::saveSettings()
{
	RINI* settings = RINI::open("Data\\res\\settings.ini");

	if (settings == nullptr)
		settings = RINI::create("Data\\res\\settings.ini");

	settings->setInt("Export", "export_format", this->m_exportSettings.exportFormat);
	settings->setInt("Export", "compression_format", this->m_exportSettings.compressionFormat);
	settings->setBool("Export", "compression_use_source_sample_frequency", this->m_exportSettings.useSourceSampleFrequency);
	settings->setInt("Export", "compression_sample_frequency", this->m_exportSettings.sampleFrequency);

	settings->setBool("ModelViewer", "draw_dummies", this->m_previewFlags.drawDummies);
	settings->setBool("ModelViewer", "draw_meshes", this->m_previewFlags.drawMeshes);
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

						this->m_gamePath = utils::findGamePath(filepath);

						if (filepath.extension() == ".nmb")
							this->m_character = Character::createFromNmb(this->m_timeActFileList, RString::toNarrow(filepath).c_str(), false);
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

	std::filesystem::current_path(path);

	TimeAct::TaeExport::TimeActExportXML* tae = this->m_character->getTimeAct();

	if (tae == nullptr)
		return false;

	wchar_t filename[256];
	swprintf_s(filename, L"%ws_tae.xml", RString::toWide(tae->getName()).c_str());

	tae->setDstFileName(RString::toNarrow(filename));
	tae->save();

	std::filesystem::current_path(getExeRootDir());

	return true;
}

bool MorphemeEditorApp::exportNetwork(std::wstring path)
{
	if (this->m_character->getMorphemeCharacter() == nullptr)
		return false;

	std::filesystem::current_path(path);

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

		g_appLog->debugMessage(MsgLevel_Info, "Exporting character controller %d for %ws (%ws)\n", i, chrName.c_str(), filename);

		ME::CharacterControllerExportXML* characterControllerExport = MD::exportCharacterController(characterDef->getCharacterController(i), filename);
	
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

		g_appLog->debugMessage(MsgLevel_Info, "Exporting rig for animation set %d for %ws (%ws)\n", i, chrName.c_str(), filename);

		ME::RigExportXML* rigExport = MD::exportRig(characterDef->getNetworkDef(), characterDef->getNetworkDef()->getRig(i), i, filename);

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

	g_appLog->debugMessage(MsgLevel_Info, "Exporting animation library for %ws (%ws)\n", chrName.c_str(), libraryFilename);

	ME::AnimationLibraryXML* animLibraryExport = MD::exportAnimLibrary(characterDef->getAnimFileLookUp(), characterDef->getNetworkDef(), rigExports, controllerExports, chrName.c_str(), libraryFilename, getAnimFormatName(this->m_exportSettings.compressionFormat), !this->m_exportSettings.useSourceSampleFrequency, this->m_exportSettings.sampleFrequency);

	if (!animLibraryExport->write())
		g_appLog->debugMessage(MsgLevel_Error, "Failed to export animation library for %ws\n", chrName.c_str());

	g_workerThread.load()->increaseProgressStep();

	g_workerThread.load()->increaseProgressStep();
	g_workerThread.load()->setProcessStepName("Exporting message preset library");

	g_workerThread.load()->addProcess("Export message preset library", 1);

	wchar_t messagePresetFilename[256];
	swprintf_s(messagePresetFilename, L"%ws_Preset.xml", chrName.c_str());

	g_appLog->debugMessage(MsgLevel_Info, "Exporting message preset library for %ws (%ws)\n", chrName.c_str(), messagePresetFilename);

	ME::MessagePresetLibraryExportXML* messagePresetExport = MD::exportMessagePresetLibrary(characterDef->getNetworkDef(), chrName, messagePresetFilename);

	if (!messagePresetExport->write())
		g_appLog->debugMessage(MsgLevel_Error, "Failed to export message library for %ws\n", chrName.c_str());

	g_workerThread.load()->increaseProgressStep();
	g_workerThread.load()->increaseProgressStep();
	g_workerThread.load()->setProcessStepName("Exporting network definition");

	g_workerThread.load()->addProcess("Export network definition", 1);

	wchar_t networkFilename[256];
	swprintf_s(networkFilename, L"%ws.xml", chrName.c_str());

	MR::NetworkDef* netDef = characterDef->getNetworkDef();

#ifdef _DEBUG
	dumpNodeIDNamesTable(netDef, animLibraryExport, L"NodeIDNamesTable.xml");
	dumpNetworkNodes(netDef, animLibraryExport, L"nodes.xml");
	dumpNetworkTaskQueuingFnTables(netDef, L"taskQueuingFnTables.txt");
	dumpNetworkOutputCPTasksFnTables(netDef, L"outputCPTasksFnTables.txt");
#endif

	g_appLog->debugMessage(MsgLevel_Info, "Exporting networkDef for %ws (%ws):\n", chrName.c_str(), networkFilename);

	ME::NetworkDefExportXML* netDefExport = MD::exportNetwork(netDef, animLibraryExport, messagePresetExport, chrName, networkFilename);

	if (!netDefExport->write())
		g_appLog->debugMessage(MsgLevel_Error, "Failed to export networkDef for c%04d\n", chrName);

	g_workerThread.load()->increaseProgressStep();
	g_workerThread.load()->increaseProgressStep();

	std::filesystem::current_path(getExeRootDir());

	return true;
}

bool MorphemeEditorApp::exportAndCompileTae(std::wstring path)
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

bool MorphemeEditorApp::exportAll(std::wstring path)
{
	if (this->m_character != nullptr)
	{
		g_workerThread.load()->addProcess("Exporting all", 4);
		g_workerThread.load()->setProcessStepName("Exporting model");

		this->exportModel(path);

		g_workerThread.load()->increaseProgressStep();
		g_workerThread.load()->setProcessStepName("Exporting animations and markups");

		this->exportAnimationsAndMarkups(path);

		g_workerThread.load()->increaseProgressStep();
		g_workerThread.load()->setProcessStepName("Exporting network");

		this->exportNetwork(path);

		g_workerThread.load()->increaseProgressStep();
		g_workerThread.load()->setProcessStepName("Exporting TimeAct");

		this->exportTimeAct(path);

		g_workerThread.load()->increaseProgressStep();
	}
	else
	{
		g_appLog->alertMessage(MsgLevel_Error, "No character is loaded\n");

		return false;
	}

	return true;
}

bool MorphemeEditorApp::exportAndProcess(std::wstring path)
{
	bool status = true;

	g_workerThread.load()->addProcess("Exporting and processing assets", 3);
	g_workerThread.load()->setProcessStepName("Exporting assets");

	if (!this->exportAll(path))
	{
		g_appLog->alertMessage(MsgLevel_Error, "Failed to process assets\n");
		return false;
	}

	g_workerThread.load()->increaseProgressStep();
	g_workerThread.load()->setProcessStepName("Compiling TimeAct files");

	if (!this->compileTimeActFiles(path))
	{
		g_appLog->alertMessage(MsgLevel_Error, "Failed to compile TimeAct files\n");
		status = false;
	}

	g_workerThread.load()->increaseProgressStep();
	g_workerThread.load()->setProcessStepName("Compiling morpheme assets");

	if (!this->compileMorphemeAssets(path))
	{
		g_appLog->alertMessage(MsgLevel_Error, "Failed to compile TimeAct files\n");
		return false;
	}

	g_workerThread.load()->increaseProgressStep();

	return status;
}

bool MorphemeEditorApp::exportAnimations(std::wstring path)
{
	MorphemeCharacterDef* characterDef = this->m_character->getMorphemeCharacterDef();

	const int animSetIdx = this->m_character->getMorphemeNetwork()->getActiveAnimSetIndex();
	const int numAnims = characterDef->getNumAnims(animSetIdx);

	g_workerThread.load()->addProcess("Exporting animations", numAnims);
	g_appLog->debugMessage(MsgLevel_Info, "Exporting animations:\n");

	for (size_t i = 0; i < numAnims; i++)
	{
		std::string animName = RString::removeExtension(characterDef->getAnimationById(animSetIdx, i)->getAnimName());
		g_workerThread.load()->setProcessStepName(animName);

		this->exportAnimation(path, animSetIdx, i);

		g_workerThread.load()->increaseProgressStep();
	}

	return true;
}

bool MorphemeEditorApp::exportAnimMarkups(std::wstring path)
{
	MorphemeCharacterDef* characterDef = this->m_character->getMorphemeCharacterDef();

	const int animSetIdx = this->m_character->getMorphemeNetwork()->getActiveAnimSetIndex();
	const int numAnims = characterDef->getNumAnims(animSetIdx);

	g_workerThread.load()->addProcess("Exporting anim markup", numAnims);
	g_appLog->debugMessage(MsgLevel_Info, "Exporting animation markups:\n");

	std::vector<ME::EventTrackExport*> exportedTracks;
	for (size_t i = 0; i < numAnims; i++)
	{
		std::string animName = RString::removeExtension(characterDef->getAnimationById(animSetIdx, i)->getAnimName());
		g_workerThread.load()->setProcessStepName(animName);

		this->exportAnimMarkup(path, animSetIdx, i, exportedTracks);

		g_workerThread.load()->increaseProgressStep();
	}

	return true;
}

void MorphemeEditorApp::exportAnimationsAndMarkups(std::wstring path)
{
	std::filesystem::current_path(path);

	g_workerThread.load()->addProcess("Export animations", 2);

	std::wstring markupExportPath = L"morphemeMarkup\\";
	std::filesystem::create_directories(markupExportPath);

	g_workerThread.load()->setProcessStepName("Exporting anim markups...");
	this->exportAnimMarkups(markupExportPath);

	g_workerThread.load()->increaseProgressStep();

	std::wstring animExportPath;

	switch (this->m_exportSettings.exportFormat)
	{
	case kFbx:
		animExportPath = L"FBX\\";
		break;
	case kXmd:
		animExportPath = L"motion_xmd\\";
		break;
	default:
		break;
	}

	std::filesystem::create_directories(animExportPath);

	g_workerThread.load()->setProcessStepName("Exporting animations...");

	this->exportAnimations(animExportPath);

	g_workerThread.load()->increaseProgressStep();

	std::filesystem::current_path(getExeRootDir());
}

bool MorphemeEditorApp::exportModel(std::wstring path)
{
	FlverModel* model = this->m_character->getCharacterModelCtrl()->getModel();

	if (model == nullptr)
		return false;

	g_workerThread.load()->addProcess("Exporting model", 1);

	std::filesystem::current_path(path);

#ifdef _DEBUG
	exportFlverToMorphemeBoneMap(model, path + L"morpheme_bone_map.txt");
#endif

	switch (this->m_exportSettings.exportFormat)
	{
	case MorphemeEditorApp::kFbx:
		exportModelToFbx(this->m_character);
		break;
	case MorphemeEditorApp::kXmd:
		exportModelToXmd(this->m_character);
		break;
	case MorphemeEditorApp::kGltf:
		exportModelToGltf(this->m_character);
		break;
	}

	g_workerThread.load()->increaseProgressStep();

	std::filesystem::current_path(getExeRootDir());
}

bool MorphemeEditorApp::compileMorphemeAssets(std::wstring path)
{
	if (this->m_character == nullptr)
	{
		g_appLog->debugMessage(MsgLevel_Info, "Skipping assets compilation because no character file was loaded\n");

		return false;
	}

	char networkFileName[256];
	sprintf_s(networkFileName, "%ws.xml", this->m_character->getCharacterName().c_str());

	char exePath[MAX_PATH];
	GetModuleFileNameA(NULL, exePath, MAX_PATH);
	std::string exeParentPath = std::filesystem::path(exePath).parent_path().string();

	std::string fullPath = exeParentPath + std::string("\\") + RString::toNarrow(path);

	const int successCode = 1;
	const int failureCode = -1;

	std::string assetCompilerName = std::string("\"") + exeParentPath + "\\" + std::string(ASSET_COMPILER_EXE) + std::string("\"");
	std::string assetPath = "-asset " + std::string("\"") + fullPath + "\\" + std::string(networkFileName) + std::string("\"");
	std::string baseDir = "-basedir " + std::string("\"") + fullPath + std::string("\"");
	std::string cacheDir = "-cacheDir " + std::string("\"") + fullPath + "\\cache" + std::string("\"");
	std::string outputDir = "-outputdir " + std::string("\"") + fullPath + "\\runtimeBinary" + std::string("\"");
	std::string logFile = "-logFile " + std::string("\"") + fullPath + "\\assetCompiler.log" + std::string("\"");
	std::string errFile = "-errFile " + std::string("\"") + fullPath + "\\assetCompilerError.log" + std::string("\"");

	std::string successCodeStr = "-successCode " + std::to_string(successCode);
	std::string failureCodeStr = "-failureCode " + std::to_string(failureCode);

	std::string assetCompilerCommand = assetCompilerName + " " + "-successCode 1 -failureCode -1" + " " + assetPath + " " + baseDir + " " + cacheDir + " " + outputDir + " " + logFile + " " + errFile;
	
#ifdef _DEBUG
	exportAssetCompilerCommand(assetCompilerCommand.c_str(), path + L"\\assetCompilerCommand.txt");
#endif

	g_appLog->debugMessage(MsgLevel_Info, "Invoking asset compiler with command %s\n", assetCompilerCommand.c_str());

	//If it exists, delete any previous output in this folder toa void cluttering
	if (std::filesystem::exists(fullPath + "\\runtimeBinary"))
		std::filesystem::remove_all(fullPath + "\\runtimeBinary");

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (CreateProcess(nullptr, LPWSTR(RString::toWide(assetCompilerCommand).c_str()), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi))
	{
		WaitForSingleObject(pi.hProcess, INFINITE);

		DWORD exitCode;
		GetExitCodeProcess(pi.hProcess, &exitCode);

		g_appLog->debugMessage(MsgLevel_Info, "\tAsset compiler exited with code %d\n", exitCode);

		if (exitCode == failureCode)
			g_appLog->alertMessage(MsgLevel_Error, "There were errors while compiling assets. Check assetCompilerError.log");

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		g_appLog->debugMessage(MsgLevel_Info, "\tPerforming post-compilation cleanup\n", exitCode);

		//Delete the cache folder
		std::filesystem::remove_all(fullPath + "\\cache");

		return true;
	}

	g_appLog->alertMessage(MsgLevel_Error, "Failed to create assetCompiler process\n");

	return false;
}

bool MorphemeEditorApp::compileTimeActFiles(std::wstring path)
{
	if (this->m_character == nullptr)
	{
		g_appLog->alertMessage(MsgLevel_Info, "Skipping TimeAct compilation because no character was loaded\n");

		return false;
	}

	TimeAct::TaeExport::TimeActExportXML* taeXML = this->m_character->getTimeAct();

	if (taeXML == nullptr)
	{
		g_appLog->debugMessage(MsgLevel_Info, "Skipping TimeAct compilation because no TimeAct file was loaded\n");

		return false;
	}

	std::filesystem::current_path(path);

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
	taePl->save(taeName + L"_pl.tae");
	g_workerThread.load()->increaseProgressStep();

	g_workerThread.load()->setProcessStepName("sfx");
	taeSfx->save(taeName + L"_sfx.tae");
	g_workerThread.load()->increaseProgressStep();

	g_workerThread.load()->setProcessStepName("snd");
	taeSnd->save(taeName + L"_snd.tae");
	g_workerThread.load()->increaseProgressStep();

	std::filesystem::current_path(getExeRootDir());

	return true;
}

bool MorphemeEditorApp::exportAnimation(std::wstring path, int animSetIdx, int animId)
{
	const int fps = 30;

	switch (this->m_exportSettings.exportFormat)
	{
	case MorphemeEditorApp::kFbx:
		return exportAnimationToFbx(path, this->m_character, animSetIdx, animId, fps, false);
	case MorphemeEditorApp::kXmd:
		return exportAnimationToXmd(path, this->m_character, animSetIdx, animId, fps);
	case MorphemeEditorApp::kGltf:
		return exportAnimationToGltf(path, this->m_character, animSetIdx, animId, fps);
	default:
		return false;
	}
}

bool MorphemeEditorApp::exportAnimMarkup(std::wstring path, int animSetIdx, int animId, std::vector<ME::EventTrackExport*>& exportedTracks)
{
	MorphemeCharacterDef* characterDef = this->m_character->getMorphemeCharacterDef();
	
	if (characterDef == nullptr)
		throw("characterDef was nullptr\n");

	AnimObject* anim = characterDef->getAnimationById(animSetIdx, animId);

	ME::TakeListXML* takeListXML = anim->getTakeList();
	std::string animName = RString::removeExtension(anim->getAnimName());

	if (takeListXML)
	{
		g_appLog->debugMessage(MsgLevel_Info, "\tExporting animation markup for animation \"%s\" (%ws)\n", animName.c_str(), this->m_character->getCharacterName().c_str());

		ME::TakeListXML* optimisedTakeList = createOptimisedTakeList(takeListXML, exportedTracks);

		if (!optimisedTakeList->write())
		{
			g_appLog->debugMessage(MsgLevel_Error, "\tFailed to export take list for animation %d\n", animId);

			return false;
		}

		delete optimisedTakeList;
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