#include "Application.h"
#include "extern.h"
#include "framework.h"
#include "Scene/Scene.h"
#include "morpheme/AnimSource/mrAnimSourceNSA.h"
#include "utils/NMDX/NMDX.h"
#include <Shlwapi.h>
#include <filesystem>
#include "FBXTranslator/FBXTranslator.h"
#include "IconsFontAwesome6.h"
#include "INI/INI.h"

bool IsEquipShield(std::wstring filename)
{
	size_t first = filename.find_first_of(L"_");

	std::wstring category = filename.substr(0, first);

	if (category == L"sd")
		return true;

	return false;
}

bool IsEquipSameGenderAs(std::wstring filename, bool female)
{
	size_t first = filename.find_last_of(L"_");

	std::wstring gender = std::filesystem::path(filename.substr(first + 1, first + 2)).stem();

	bool is_female = false;

	if (gender == L"f")
		is_female = true;
	else if (gender == L"m")
		is_female = false;
	else
		return true;

	return (female == is_female);
}

int GetEquipIDByFilename(std::wstring filename)
{
	size_t first = filename.find_first_of(L"_") + 1;
	size_t second = filename.find_last_of(L"_");

	std::wstring modelIdStr = filename.substr(first, second - first);

	return std::stoi(modelIdStr);
}

int GetFgIDByFilename(std::wstring filename, FgPartType type)
{
	int baseId = 0;

	switch (type)
	{
	case FaceGen_Face:
		baseId = 1000;
		break;
	case FaceGen_Head:
		baseId = 2000;
		break;
	case FaceGen_Eyes:
		baseId = 3000;
		break;
	case FaceGen_EyeBrows:
		baseId = 4000;
		break;
	case FaceGen_Beard:
		baseId = 5000;
		break;
	case FaceGen_Hair:
		baseId = 7000;
		break;
	default:
		break;
	}

	int id = GetEquipIDByFilename(filename);

	return id - baseId;
}

void LoadPartsFaceGenBnd(Application* pApplication, std::wstring root, FgPartType type, int id, bool female)
{
	std::wstring filepath = root.c_str();
	wchar_t modelName[255] = { 0 };
	int fullId = 0;

	switch (type)
	{
	case FaceGen_Face:
		if (female)
			swprintf_s(modelName, L"\\face\\fg_1001_f.bnd");
		else
			swprintf_s(modelName, L"\\face\\fg_1001_m.bnd");
		break;
	case FaceGen_Head:
		if (female)
			swprintf_s(modelName, L"\\face\\fg_2001_f.bnd");
		else
			swprintf_s(modelName, L"\\face\\fg_2001_m.bnd");
		break;
	case FaceGen_Eyes:
		swprintf_s(modelName, L"\\face\\fg_3001_a.bnd");
		break;
	case FaceGen_EyeBrows:
		fullId = 4000 + id;

		swprintf_s(modelName, L"\\face\\fg_%d_m.bnd", fullId);
		break;
	case FaceGen_Beard:
		fullId = 5000 + id;

		if (!female)
			swprintf_s(modelName, L"\\face\\fg_%d_m.bnd", fullId);
		break;
	case FaceGen_Hair:
		fullId = 7000 + id;

		if (female)
			swprintf_s(modelName, L"\\face\\fg_%d_f.bnd", fullId);
		else
			swprintf_s(modelName, L"\\face\\fg_%d_m.bnd", fullId);

		break;
	default:
		break;
	}

	filepath += modelName;

	FlverModel* model = FlverModel::CreateFromBnd(filepath);

	if (model)
	{
		pApplication->m_animPlayer->SetModelPartFacegen(type, model);
		pApplication->m_animPlayer->GetModelPartFacegen(type)->CreateFlverToMorphemeBoneMap(pApplication->m_morphemeSystem->GetCharacterDef()->getNetworkDef()->getRig(0));
	}
}

void LoadWeaponBnd(Application* pApplication, std::wstring root, PartType type, int id, bool shield)
{
	std::wstring filepath = root.c_str();
	wchar_t modelName[255];

	if (shield)
		swprintf_s(modelName, L"\\shield\\sd_%d_m.bnd", id);
	else
		swprintf_s(modelName, L"\\weapon\\wp_%d_m.bnd", id);

	filepath += modelName;

	FlverModel* model = FlverModel::CreateFromBnd(filepath);

	if (model)
		pApplication->m_animPlayer->SetModelPart(type, model);
}

void LoadPartsBnd(Application* pApplication, std::wstring root, PartType type, int id, bool female)
{
	std::wstring filepath = root.c_str();
	wchar_t modelName[255] = { 0 };

	switch (type)
	{
	case Parts_Head:
		if (female)
			swprintf_s(modelName, L"\\head\\hd_%d_f.bnd", id);
		else
			swprintf_s(modelName, L"\\head\\hd_%d_m.bnd", id);
		break;
	case Parts_Face:
		if (female)
			swprintf_s(modelName, L"\\face\\fc_%d_f.bnd", id);
		else
			swprintf_s(modelName, L"\\face\\fc_%d_m.bnd", id);
		break;
	case Parts_Body:
		if (female)
			swprintf_s(modelName, L"\\body\\bd_%d_f.bnd", id);
		else
			swprintf_s(modelName, L"\\body\\bd_%d_m.bnd", id);
		break;
	case Parts_Arm:
		if (female)
			swprintf_s(modelName, L"\\arm\\am_%d_f.bnd", id);
		else
			swprintf_s(modelName, L"\\arm\\am_%d_m.bnd", id);
		break;
	case Parts_Leg:
		if (female)
			swprintf_s(modelName, L"\\leg\\lg_%d_f.bnd", id);
		else
			swprintf_s(modelName, L"\\leg\\lg_%d_m.bnd", id);
		break;
	default:
		break;
	}

	filepath += modelName;

	FlverModel* model = FlverModel::CreateFromBnd(filepath);

	if (model)
	{
		pApplication->m_animPlayer->SetModelPart(type, model);
		pApplication->m_animPlayer->GetModelPart(type)->CreateFlverToMorphemeBoneMap(pApplication->m_morphemeSystem->GetCharacterDef()->getNetworkDef()->getRig(0));
	}
}

void LoadPlayerModelParts(Application* application, std::wstring parts_path)
{
	application->m_animPlayer->ClearModelParts();

	bool female = application->m_playerModelPreset->GetBool("Gender", "is_female", false);

	int rightId = application->m_playerModelPreset->GetInt("Right", "id", -1);
	bool rightShield = application->m_playerModelPreset->GetBool("Right", "is_shield", false);

	LoadWeaponBnd(application, parts_path, Parts_WeaponRight, rightId, rightShield);

	int leftId = application->m_playerModelPreset->GetInt("Left", "id", -1);
	bool leftShield = application->m_playerModelPreset->GetBool("Left", "is_shield", false);

	LoadWeaponBnd(application, parts_path, Parts_WeaponLeft, leftId, leftShield);

	int headId = application->m_playerModelPreset->GetInt("Armor", "head", -1);
	int faceId = application->m_playerModelPreset->GetInt("Armor", "face", -1);
	int bodyId = application->m_playerModelPreset->GetInt("Armor", "body", -1);
	int armId = application->m_playerModelPreset->GetInt("Armor", "arm", -1);
	int legId = application->m_playerModelPreset->GetInt("Armor", "leg", -1);

	LoadPartsBnd(application, parts_path, Parts_Head, headId, female);
	LoadPartsBnd(application, parts_path, Parts_Face, faceId, female);
	LoadPartsBnd(application, parts_path, Parts_Body, bodyId, female);
	LoadPartsBnd(application, parts_path, Parts_Arm, armId, female);
	LoadPartsBnd(application, parts_path, Parts_Leg, legId, female);

	int fgFaceId = application->m_playerModelPreset->GetInt("FaceGen", "face", -1);
	int fgHeadId = application->m_playerModelPreset->GetInt("FaceGen", "head", -1);
	int fgEyeId = application->m_playerModelPreset->GetInt("FaceGen", "eye", -1);
	int fgEyeBrowsId = application->m_playerModelPreset->GetInt("FaceGen", "eye_brows", -1);
	int fgBeard = application->m_playerModelPreset->GetInt("FaceGen", "beard", -1);
	int fgHair = application->m_playerModelPreset->GetInt("FaceGen", "hair", -1);

	LoadPartsFaceGenBnd(application, parts_path, FaceGen_Face, fgFaceId, female);
	LoadPartsFaceGenBnd(application, parts_path, FaceGen_Head, fgHeadId, female);
	LoadPartsFaceGenBnd(application, parts_path, FaceGen_Eyes, fgEyeId, female);
	LoadPartsFaceGenBnd(application, parts_path, FaceGen_EyeBrows, fgEyeBrowsId, female);
	LoadPartsFaceGenBnd(application, parts_path, FaceGen_Beard, fgBeard, female);
	LoadPartsFaceGenBnd(application, parts_path, FaceGen_Hair, fgHair, female);
}

std::vector<std::wstring> getTaeFileListFromChrId(std::wstring tae_path, std::wstring m_chrId)
{
	std::vector<std::wstring> files;

	for (const auto& entry : std::filesystem::directory_iterator(tae_path))
	{
		if (entry.path().extension().compare(".tae") == 0)
		{
			std::wstring filename = entry.path().filename();
			std::wstring file_chr_id_str = filename.substr(1, 4);

			if (file_chr_id_str.compare(m_chrId) == 0)
			{
				g_appLog->DebugMessage(MsgLevel_Debug, "\t%ws\n", filename.c_str());
				files.push_back(entry.path());
			}
		}
	}

	if (files.size() == 0)
		g_appLog->AlertMessage(MsgLevel_Debug, "Could not find any TimeAct files belonging to c%d in %ws\n", m_chrId, tae_path);

	return files;
}

std::wstring getModelNameFromChrId(std::wstring model_path, std::wstring m_chrId)
{
	for (const auto& entry : std::filesystem::directory_iterator(model_path))
	{
		if (entry.path().extension().compare(".bnd") == 0)
		{
			std::wstring filename = entry.path().filename();
			std::wstring file_chr_id_str = filename.substr(1, 4);

			if (file_chr_id_str.compare(m_chrId) == 0)
				return entry.path();
		}
	}

	return L"";
}

std::wstring getModelNameFromObjId(std::wstring model_path, std::wstring obj_id)
{
	for (const auto& entry : std::filesystem::directory_iterator(model_path))
	{
		std::wstring filename = entry.path().filename();
		std::wstring file_chr_id_str = filename.substr(0, 8);

		if ((file_chr_id_str.compare(obj_id) == 0) && (entry.path().extension() == ".bnd"))
			return entry.path();
	}

	return L"";
}

FileNamePathPair::FileNamePathPair(std::wstring path, int id)
{
	this->m_path = path;
	this->m_name = RString::RemoveExtension(RString::ToNarrow(path));
	this->m_id = id;
}

void FileNameMapPairList::Clear()
{
	this->m_weaponModelPaths.clear();
	this->m_shieldModelPaths.clear();
	this->m_headModelPaths.clear();
	this->m_faceModelPaths.clear();
	this->m_bodyModelPaths.clear();
	this->m_armModelPaths.clear();
	this->m_legModelPaths.clear();

	this->m_fgFace.clear();
	this->m_fgHead.clear();
	this->m_fgEyes.clear();
	this->m_fgEyeBrows.clear();
	this->m_fgBeard.clear();
	this->m_fgHair.clear();
}

void FileNameMapPairList::Create(std::wstring gamePath)
{
	std::filesystem::path shield_path = gamePath + L"\\model\\parts\\shield";
	std::filesystem::path weapon_path = gamePath + L"\\model\\parts\\weapon";

	std::filesystem::path head_path = gamePath + L"\\model\\parts\\head";
	std::filesystem::path face_path = gamePath + L"\\model\\parts\\face";
	std::filesystem::path body_path = gamePath + L"\\model\\parts\\body";
	std::filesystem::path arm_path = gamePath + L"\\model\\parts\\arm";
	std::filesystem::path leg_path = gamePath + L"\\model\\parts\\leg";

	for (const auto& entry : std::filesystem::directory_iterator(weapon_path))
	{
		if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && (entry.path().stem().string().back() != 'a') && (entry.path().stem().string().back() != 'l'))
		{
			std::wstring filepath = entry.path();		
			int id = GetEquipIDByFilename(filepath);

			this->m_weaponModelPaths.push_back(FileNamePathPair(filepath, id));
		}
	}

	for (const auto& entry : std::filesystem::directory_iterator(shield_path))
	{
		if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && (entry.path().stem().string().back() != 'a') && (entry.path().stem().string().back() != 'l'))
		{
			std::wstring filepath = entry.path();
			int id = GetEquipIDByFilename(filepath);

			this->m_shieldModelPaths.push_back(FileNamePathPair(filepath, id));
		}
	}

	for (const auto& entry : std::filesystem::directory_iterator(head_path))
	{
		if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && (entry.path().stem().string().back() != 'l'))
		{
			std::wstring filepath = entry.path();
			int id = GetEquipIDByFilename(filepath);

			this->m_headModelPaths.push_back(FileNamePathPair(filepath, id));
		}
	}

	for (const auto& entry : std::filesystem::directory_iterator(face_path))
	{
		if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && (entry.path().stem().string().back() != 'l') && (entry.path().stem().string().substr(0, 2) == "fc"))
		{
			std::wstring filepath = entry.path();
			int id = GetEquipIDByFilename(filepath);

			this->m_faceModelPaths.push_back(FileNamePathPair(filepath, id));
		}
	}

	for (const auto& entry : std::filesystem::directory_iterator(body_path))
	{
		if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && (entry.path().stem().string().back() != 'l'))
		{
			std::wstring filepath = entry.path();
			int id = GetEquipIDByFilename(filepath);

			this->m_bodyModelPaths.push_back(FileNamePathPair(filepath, id));
		}
	}

	for (const auto& entry : std::filesystem::directory_iterator(arm_path))
	{
		if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && (entry.path().stem().string().back() != 'l'))
		{
			std::wstring filepath = entry.path();
			int id = GetEquipIDByFilename(filepath);

			this->m_armModelPaths.push_back(FileNamePathPair(filepath, id));
		}
	}

	for (const auto& entry : std::filesystem::directory_iterator(leg_path))
	{
		if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && (entry.path().stem().string().back() != 'l'))
		{
			std::wstring filepath = entry.path();
			int id = GetEquipIDByFilename(filepath);

			this->m_legModelPaths.push_back(FileNamePathPair(filepath, id));
		}
	}

	for (const auto& entry : std::filesystem::directory_iterator(face_path))
	{
		if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && ((entry.path().stem().string().back() == 'f') || (entry.path().stem().string().back() == 'm')) && (entry.path().stem().string().substr(0, 4) == "fg_1"))
		{
			std::wstring filepath = entry.path();
			int id = GetFgIDByFilename(filepath, FaceGen_Face);

			this->m_fgFace.push_back(FileNamePathPair(filepath, id));
		}
	}

	for (const auto& entry : std::filesystem::directory_iterator(face_path))
	{
		if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && ((entry.path().stem().string().back() == 'f') || (entry.path().stem().string().back() == 'm')) && (entry.path().stem().string().substr(0, 4) == "fg_2"))
		{
			std::wstring filepath = entry.path();
			int id = GetFgIDByFilename(filepath, FaceGen_Head);

			this->m_fgHead.push_back(FileNamePathPair(filepath, id));
		}
	}

	for (const auto& entry : std::filesystem::directory_iterator(face_path))
	{
		if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && ((entry.path().stem().string().back() == 'a')) && (entry.path().stem().string().substr(0, 4) == "fg_3"))
		{
			std::wstring filepath = entry.path();
			int id = GetFgIDByFilename(filepath, FaceGen_Eyes);

			this->m_fgEyes.push_back(FileNamePathPair(filepath, id));
		}
	}

	for (const auto& entry : std::filesystem::directory_iterator(face_path))
	{
		if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && ((entry.path().stem().string().back() == 'm')) && (entry.path().stem().string().substr(0, 4) == "fg_4"))
		{
			std::wstring filepath = entry.path();
			int id = GetFgIDByFilename(filepath, FaceGen_EyeBrows);

			this->m_fgEyeBrows.push_back(FileNamePathPair(filepath, id));
		}
	}

	for (const auto& entry : std::filesystem::directory_iterator(face_path))
	{
		if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && ((entry.path().stem().string().back() == 'm')) && (entry.path().stem().string().substr(0, 4) == "fg_5"))
		{
			std::wstring filepath = entry.path();
			int id = GetFgIDByFilename(filepath, FaceGen_Beard);

			this->m_fgBeard.push_back(FileNamePathPair(filepath, id));
		}
	}

	for (const auto& entry : std::filesystem::directory_iterator(face_path))
	{
		if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension().compare("bnd") && ((entry.path().stem().string().back() == 'f') || (entry.path().stem().string().back() == 'm')) && (entry.path().stem().string().substr(0, 4) == "fg_7"))
		{
			std::wstring filepath = entry.path();
			int id = GetFgIDByFilename(filepath, FaceGen_Hair);

			this->m_fgHair.push_back(FileNamePathPair(filepath, id));
		}
	}
}

Application::Application()
{
}

Application::~Application()
{
}

void Application::GUIStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowBorderSize = 1;
	style->FrameBorderSize = 0;
	style->PopupBorderSize = 1;
	style->FrameBorderSize = 0;
	style->TabBorderSize = 0;

	style->WindowRounding = 0;
	style->ChildRounding = 0;
	style->FrameRounding = 0;
	style->PopupRounding = 0;
	style->ScrollbarRounding = 12;
	style->GrabRounding = 0;
	style->TabRounding = 0;
	style->WindowMenuButtonPosition = ImGuiDir_Left;

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.51f, 0.51f, 0.59f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.47f, 0.47f, 0.47f, 0.39f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.51f, 0.51f, 0.51f, 0.39f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.f, 0.f, 0.f, 0.f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.f, 0.f, 0.f, 0.f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.f, 0.f, 0.f, 0.f);
	colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void Application::Initialise()
{
	this->m_morphemeSystem = new MorphemeSystem();
	this->m_animPlayer = new AnimPlayer();
	this->m_eventTrackEditor = new EventTrackEditor();
	this->m_timeActEditor = new TimeActEditor();
	this->m_appSettings = new INI;
	this->m_playerModelPreset = new INI;
	this->m_fileNameMapPairList = new FileNameMapPairList;

	this->m_morphemeSystem->initMorpheme();

	if (!this->m_playerModelPreset->Open(".//Data//res//c0001.ini"))
		g_appLog->PanicMessage("Failed to load c0001.ini\n");

	if (!this->m_appSettings->Open(".//Data//res//settings.ini"))
		g_appLog->PanicMessage("Failed to load settings.ini\n");
}

void Application::Shutdown()
{
	this->m_morphemeSystem->termMorpheme();

	this->m_appSettings->Write(".//Data//res//settings.ini");
	this->m_playerModelPreset->Write(".//Data//res//c0001.ini");

	delete this->m_morphemeSystem;
	delete this->m_animPlayer;
	delete this->m_eventTrackEditor;
	delete this->m_timeAct;
	delete this->m_appSettings;
	delete this->m_playerModelPreset;
	delete this->m_fileNameMapPairList;
}

void Application::Update(float delta_time)
{
	this->CheckFlags();
	this->RenderGUI("morphemeEditor");
}

std::string getTaeCategoryTooltip(int category)
{
	INI ini;

	char default_info[255];
	sprintf_s(default_info, "No specific information is known for this category\n");

	if (!ini.Open(".//Data//res//tooltip//timeact//group.ini"))
	{
		g_appLog->DebugMessage(MsgLevel_Debug, "Failed to load group.ini\n");
		return std::string(default_info);
	}

	std::string category_str = std::to_string(category);

	return ini.GetString(category_str, "tooltip", default_info);
}

std::string getTaeEventTooltip(int event_id)
{
	INI ini;

	char default_info[255];
	sprintf_s(default_info, "No specific information is known for this event\n");

	if (!ini.Open(".//Data//res//tooltip//timeact//event.ini"))
	{
		g_appLog->DebugMessage(MsgLevel_Debug, "Failed to load group.ini\n");
		return std::string(default_info);
	}

	std::string category_str = std::to_string(event_id);

	return ini.GetString(category_str, "tooltip", default_info);
}

std::string getEventTrackCategoryTooltip(int category)
{
	INI ini;

	char default_info[255];
	sprintf_s(default_info, "No specific information is known for this category\n");

	if (!ini.Open(".//Data//res//tooltip//eventrack//group.ini"))
	{
		g_appLog->DebugMessage(MsgLevel_Debug, "Failed to load group.ini\n");
		return std::string(default_info);
	}

	std::string category_str = std::to_string(category);

	return ini.GetString(category_str, "tooltip", default_info);
}

std::string getEventTrackEventTooltip(int event_id)
{
	INI ini;

	char default_info[255];
	sprintf_s(default_info, "No specific information is known for this event\n");

	if (!ini.Open(".//Data//res//tooltip//eventrack//event.ini"))
	{
		g_appLog->DebugMessage(MsgLevel_Debug, "Failed to load group.ini\n");
		return std::string(default_info);
	}

	std::string category_str = std::to_string(event_id);

	return ini.GetString(category_str, "tooltip", default_info);
}

void Application::RenderGUI(const char* title)
{
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y));
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::Begin(title, NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImGui::PopStyleVar(1);

	ImGuiID dockSpace = ImGui::GetID("MainWindowDockspace");
	ImGui::DockSpace(dockSpace, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	//Menu Bar
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open...")) { this->m_flags.m_loadFile = true; }

			ImGui::Separator();

			if (ImGui::BeginMenu("Save"))
			{
				//if (ImGui::MenuItem("Save NMB")) { this->m_flags.m_saveNmb = true; }
				if (ImGui::MenuItem("Save TAE")) { this->m_flags.m_saveTae = true; }
				//if (ImGui::MenuItem("Save All")) { this->m_flags.m_saveAll = true; }

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Export"))
			{
				if (ImGui::MenuItem("Export"))
					this->m_flags.m_exportAll = true;

				ImGui::Separator();

				if (ImGui::MenuItem("Export Animations"))
					this->m_flags.m_exportAnimations = true;

				if (ImGui::MenuItem("Export Model"))
					this->m_flags.m_exportModel = true;

				if (ImGui::MenuItem("Export Network"))
					this->m_flags.m_exportNetwork = true;

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Scene Settings", NULL, &this->m_windowStates.m_previewSettings)) { this->m_windowStates.m_previewSettings != this->m_windowStates.m_previewSettings; }

			if (ImGui::BeginMenu("Timecode"))
			{
				static bool timecode = this->m_appSettings->GetBool("AppRootWindow", "use_seconds", true);

				bool timecode_bak = timecode;

				if (ImGui::MenuItem("Seconds", NULL, timecode)) { timecode = true; }
				if (ImGui::MenuItem("Frames", NULL, !timecode)) { timecode = false; }

				this->m_eventTrackEditor->m_showTimecode = timecode;
				this->m_timeActEditor->m_showTimecode = timecode;

				if (timecode_bak != timecode)
					this->m_appSettings->SetBool("AppRootWindow", "use_seconds", timecode);

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

#ifdef _DEBUG
		if (ImGui::BeginMenu("Debug"))
		{
			ImGui::SeparatorText("Track Editor Format");

			if (ImGui::MenuItem("Style Settings", NULL, &this->m_windowStates.m_settingWindow)) { this->m_windowStates.m_settingWindow != this->m_windowStates.m_settingWindow; }
			
			ImGui::SeparatorText("Query");

			if (ImGui::MenuItem("Find TimeAct", NULL, &this->m_windowStates.m_queryTae)) { this->m_windowStates.m_queryTae != this->m_windowStates.m_queryTae; }
			if (ImGui::MenuItem("Find EventTrack", NULL, &this->m_windowStates.m_queryEventTrack)) { this->m_windowStates.m_queryEventTrack != this->m_windowStates.m_queryEventTrack; }
			
			ImGui::EndMenu();
		}
#endif

		ImGui::EndMenuBar();
	}
	ImGui::End();

	ImGui::SetNextWindowSize(ImVec2(800, 800), ImGuiCond_Appearing);
	this->ModelPreviewWindow();

	ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_Appearing);

	this->AssetsWindow();

	static int firstFrame = this->m_eventTrackEditor->m_frameMin;
	static bool expanded = true;
	int currentFrame = RMath::TimeToFrame(this->m_animPlayer->GetTime(), 60);
	static float zoomLevel = 5.f;

	if (this->m_eventTrackEditor->m_load)
		this->m_eventTrackEditor->ResetSelection();

	if (!this->m_animPlayer->IsPlaybackPaused())
		currentFrame = RMath::TimeToFrame(this->m_animPlayer->GetTime());

	ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_Appearing);

	this->EventTrackWindow(&currentFrame, &firstFrame, &zoomLevel, &expanded);

	ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_Appearing);

	this->EventTrackInfoWindow();

	static int firstFrameTae = 0;
	static bool expandedTae = true;
	static int currentFrameTae = 0;
	static float zoomLevelTae = 10.f;

	if (this->m_timeActEditor->m_load)
		this->m_timeActEditor->ResetSelection();

	zoomLevelTae = 2 * zoomLevel;

	this->SetTimeActCurrentFrameFromEventTrack(&currentFrameTae, currentFrame);

	ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_Appearing);

	this->TimeActWindow(&currentFrameTae, &firstFrameTae, &zoomLevelTae, &expandedTae);

	zoomLevel = zoomLevelTae * 0.5f;

	ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_Appearing);

	this->TimeActInfoWindow();

	this->m_animPlayer->SetTime(RMath::FrameToTime(currentFrame));
}

void Application::ModelPreviewWindow()
{
	ImGui::Begin("Preview", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar);
	CharacterDef* characterDef = this->m_morphemeSystem->GetCharacterDef();

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::MenuItem("Explorer", NULL, &this->m_sceneFlags.m_sceneExplorer)) { this->m_sceneFlags.m_sceneExplorer != this->m_sceneFlags.m_sceneExplorer; }

		if (ImGui::BeginMenu("Display Mode"))
		{
			if (ImGui::MenuItem("Normal", NULL, this->m_sceneFlags.m_displayMode == Mode_Normal)) { this->m_sceneFlags.m_displayMode = Mode_Normal; }
			if (ImGui::MenuItem("X-Ray", NULL, this->m_sceneFlags.m_displayMode == Mode_XRay)) { this->m_sceneFlags.m_displayMode = Mode_XRay; }
			if (ImGui::MenuItem("Wireframe", NULL, this->m_sceneFlags.m_displayMode == Mode_Wireframe)) { this->m_sceneFlags.m_displayMode = Mode_Wireframe; }

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Model"))
		{
			if (ImGui::MenuItem("Show Dummies", NULL, &this->m_sceneFlags.m_drawDummies)) { this->m_sceneFlags.m_drawDummies != this->m_sceneFlags.m_drawDummies; }

			bool disabled = true;

			if (characterDef && characterDef->getCharacterId() == 1)
				disabled = false;

			ImGui::BeginDisabled(disabled);

			if (ImGui::MenuItem("Player Parts", NULL, &this->m_windowStates.m_playerPartsManagerWindow)) { this->m_windowStates.m_playerPartsManagerWindow != this->m_windowStates.m_playerPartsManagerWindow; }

			ImGui::EndDisabled();
			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::Button(ICON_FA_BACKWARD_FAST))
			this->m_animPlayer->SetTime(0.f);

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_BACKWARD_STEP))
			this->m_animPlayer->StepPlay(-1.f / 30.f);

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_PAUSE))
			this->m_animPlayer->SetPause(true);

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_PLAY))
			this->m_animPlayer->SetPause(false);

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_FORWARD_STEP))
			this->m_animPlayer->StepPlay(1.f / 30.f);

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_FORWARD_FAST))
			this->m_animPlayer->SetTime(RMath::TimeToFrame(this->m_eventTrackEditor->m_frameMax));

		ImGui::Separator();

		ImGui::EndMenuBar();
	}

	ImVec2 pos = ImGui::GetWindowPos();

	int	width = ImGui::GetWindowSize().x;
	int	height = ImGui::GetWindowSize().y;

	ImGui::InvisibleButton("viewport_preview", ImVec2(width, height));

	if (ImGui::IsItemFocused() && ImGui::IsItemHovered())
	{
		g_scene->m_camera.m_registerInput = true;

		if (ImGui::IsMouseDown(0))
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

		if (ImGui::IsMouseDown(1))
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
	}

	g_scene->SetRenderResolution(width, height);

	ImGui::GetWindowDrawList()->AddImage(g_scene->m_shaderResourceViewViewport, pos, ImVec2(pos.x + width, pos.y + height));

	if (this->m_sceneFlags.m_sceneExplorer)
		this->PreviewSceneExplorerWindow();

	ImGui::End();
}

void Application::AssetsWindow()
{
	ImGui::Begin("Assets");

	CharacterDef* characterDef = this->m_morphemeSystem->GetCharacterDef();

	if (characterDef)
	{
		static int selected_tae_file_idx = -1;
		char chr_id_str[50];
		sprintf_s(chr_id_str, "%04d", characterDef->getCharacterId());

		std::string tae_popup = "Load TimeAct (c" + std::string(chr_id_str) + ")";

		if (this->m_eventTrackEditor->m_loadTae && this->m_eventTrackEditor->m_taeList.size() > 0)
		{
			ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Appearing);

			ImGui::OpenPopup(tae_popup.c_str());
		}

		ImGui::SetNextWindowPos(ImGui::GetCurrentWindow()->Pos, ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Appearing);
		if (ImGui::BeginPopupModal(tae_popup.c_str()))
		{
			static std::wstring filepath;

			if (ImGui::Button("Load") && selected_tae_file_idx != -1)
			{
				if (this->m_timeAct)
				{
					delete this->m_timeAct;
					this->m_timeAct = nullptr;
				}

				m_timeAct = new TimeActReader(PWSTR(filepath.c_str()));

				this->m_timeActEditor->m_selectedTimeActIdx = -1;

				this->m_timeActEditor->m_edited.clear();

				if (m_timeAct->m_init)
				{
					this->m_timeActEditor->m_edited.reserve(m_timeAct->m_tae.size());

					for (int i = 0; i < m_timeAct->m_tae.size(); i++)
						this->m_timeActEditor->m_edited.push_back(false);

					g_appLog->DebugMessage(MsgLevel_Debug, "Open file %ls (len=%d)\n", m_timeAct->m_filePath, m_timeAct->m_fileSize);

					this->m_eventTrackEditor->m_loadTae = false;
				}

				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();

			if (ImGui::Button("Cancel"))
			{
				this->m_eventTrackEditor->m_loadTae = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::BeginChild("file_list");
			for (int i = 0; i < this->m_eventTrackEditor->m_taeList.size(); i++)
			{
				std::filesystem::path path = this->m_eventTrackEditor->m_taeList[i];
				std::string tae_file = RString::ToNarrow(path.filename().c_str());

				bool selected = (selected_tae_file_idx == i);
				ImGui::Selectable(tae_file.c_str(), selected);

				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
				{
					selected_tae_file_idx = i;
					filepath = path.c_str();

					if (ImGui::IsMouseDoubleClicked(0))
					{
						g_appLog->DebugMessage(MsgLevel_Debug, "Selected TimeAct %s\n", tae_file.c_str());

						if (this->m_timeAct)
						{
							delete this->m_timeAct;
							this->m_timeAct = nullptr;
						}

						m_timeAct = new TimeActReader(PWSTR(filepath.c_str()));

						this->m_timeActEditor->m_selectedTimeActIdx = -1;

						this->m_timeActEditor->m_edited.clear();

						if (m_timeAct->m_init)
						{
							this->m_timeActEditor->m_edited.reserve(m_timeAct->m_tae.size());

							for (int i = 0; i < m_timeAct->m_tae.size(); i++)
								this->m_timeActEditor->m_edited.push_back(false);

							g_appLog->DebugMessage(MsgLevel_Debug, "Open file %ls (len=%d)\n", m_timeAct->m_filePath, m_timeAct->m_fileSize);

							this->m_eventTrackEditor->m_loadTae = false;
						}
						else
							g_appLog->DebugMessage(MsgLevel_Debug, "Failed to load file %s\n", tae_file.c_str());

						ImGui::CloseCurrentPopup();
					}
				}
			}
			ImGui::EndChild();

			ImGui::EndPopup();
		}
		else
			selected_tae_file_idx = -1;

		ImGui::BeginTabBar("assets");
		if (ImGui::BeginTabItem("Animations"))
		{
			CharacterDef* characterDef = this->m_morphemeSystem->GetCharacterDef();

			if (characterDef)
			{
				if (characterDef->isLoaded())
					ImGui::Text(characterDef->getFilename());

				static ImGuiTextFilter filter;
				ImGui::Text("Filter:");
				filter.Draw("##asset searchbar", 340.f);

				if (characterDef->isLoaded())
				{
					ImGui::BeginChild("anim_list");
					{
						int numAnims = characterDef->getAnimFileLookUp()->getNumAnims();

						for (int i = 0; i < numAnims; i++)
						{
							std::string anim_name = "";

							if (this->m_eventTrackEditor->m_edited[i])
								anim_name += "*";

							AnimSourceInterface* currentAnim = characterDef->getAnimation(i);

							anim_name += RString::RemoveExtension(characterDef->getAnimFileLookUp()->getSourceFilename(currentAnim->GetID()));

							bool selected = (this->m_eventTrackEditor->m_selectedAnimIdx == currentAnim->GetID());

							if (currentAnim->GetHandle() == nullptr)
								ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));

							if (filter.PassFilter(anim_name.c_str()))
							{
								ImGui::PushID(i);
								ImGui::Selectable(anim_name.c_str(), &selected);

								if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
								{
									this->m_eventTrackEditor->m_targetAnimIdx = currentAnim->GetID();
									this->m_eventTrackEditor->m_selectedAnimIdx = currentAnim->GetID();

									this->m_animPlayer->SetAnimation(currentAnim);
									this->m_eventTrackEditor->m_load = true;
								}
								ImGui::PopID();
							}

							if (currentAnim->GetHandle() == nullptr)
								ImGui::PopStyleColor();
						}
					}
					ImGui::EndChild();
				}
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("TimeAct"))
		{
			if (this->m_timeActFlags.m_addTimeAct)
			{
				this->m_timeActFlags.m_addTimeAct = false;

				ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Appearing);

				ImGui::OpenPopup("Add TimeAct");
			}

			if (this->m_timeActFlags.m_deleteTimeAct)
			{
				this->m_timeActFlags.m_deleteTimeAct = false;

				ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Appearing);

				ImGui::OpenPopup("Delete TimeAct");
			}

			ImGui::SetNextWindowPos(ImGui::GetCurrentWindow()->Pos, ImGuiCond_Appearing);
			ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Appearing);
			if (ImGui::BeginPopup("Add TimeAct"))
			{
				ImGui::Text("Add TimeAct");
				ImGui::Separator();

				ImGui::InputInt("ID", &this->m_timeActFlags.m_addTimeActId, 0, 0);
				ImGui::InputFloat("Lenght", &this->m_timeActFlags.m_addTimeActLenght, 0, 0);

				if (ImGui::Button("Add"))
				{
					if (this->m_timeAct->AddTimeAct(this->m_timeActFlags.m_addTimeActId, this->m_timeActFlags.m_addTimeActLenght) == false)
						g_appLog->AlertMessage(MsgLevel_Info, "TimeAct %d already exists\n", this->m_timeActFlags.m_addTimeActId);
					else
					{
						this->m_timeActEditor->m_edited.clear();

						if (m_timeAct->m_init)
						{
							this->m_timeActEditor->m_edited.reserve(m_timeAct->m_tae.size());

							for (int i = 0; i < m_timeAct->m_tae.size(); i++)
								this->m_timeActEditor->m_edited.push_back(false);
						}
					}

					this->m_eventTrackEditor->m_load = true;
					this->m_timeActFlags.m_addTimeAct = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				if (this->m_eventTrackEditor->m_eventTrackActionTimeActValue != -1)
				{
					if (ImGui::Button("Add Selected"))
					{
						this->m_timeActFlags.m_addTimeActId = this->m_eventTrackEditor->m_eventTrackActionTimeActValue;
						this->m_timeActFlags.m_addTimeActLenght = RMath::FrameToTime(this->m_eventTrackEditor->m_frameMax);

						if (this->m_timeAct->AddTimeAct(this->m_timeActFlags.m_addTimeActId, this->m_timeActFlags.m_addTimeActLenght) == false)
							g_appLog->AlertMessage(MsgLevel_Info, "TimeAct %d already exists\n", this->m_timeActFlags.m_addTimeActId);
						else
						{
							this->m_timeActEditor->m_edited.clear();

							if (m_timeAct->m_init)
							{
								this->m_timeActEditor->m_edited.reserve(m_timeAct->m_tae.size());

								for (int i = 0; i < m_timeAct->m_tae.size(); i++)
									this->m_timeActEditor->m_edited.push_back(false);
							}
						}

						this->m_eventTrackEditor->m_load = true;
						this->m_timeActFlags.m_addTimeAct = false;
						ImGui::CloseCurrentPopup();
					}
				}

				ImGui::EndPopup();
			}
			else
				this->m_timeActFlags.m_addTimeAct = false;

			ImGui::SetNextWindowPos(ImGui::GetCurrentWindow()->Pos, ImGuiCond_Appearing);
			ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Appearing);
			if (ImGui::BeginPopup("Delete TimeAct"))
			{
				std::string label = "Delete TimeAct (" + std::to_string(this->m_timeActFlags.m_deleteTimeActId) + ")";

				ImGui::Text(label.c_str());
				ImGui::Separator();

				if (ImGui::Button("Delete"))
				{
					if (this->m_timeAct->DeleteTimeAct(this->m_timeActFlags.m_deleteTimeActId) == false)
						g_appLog->AlertMessage(MsgLevel_Info, "Failed to delete TimeAct %d\n", this->m_timeActFlags.m_deleteTimeActId);

					this->m_eventTrackEditor->m_load = true;
					this->m_timeActFlags.m_deleteTimeAct = false;

					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
			else
				this->m_timeActFlags.m_deleteTimeAct = false;

			if (this->m_timeAct->m_init)
			{
				ImGui::Text(RString::ToNarrow(this->m_timeAct->m_fileName.c_str()).c_str());

				if (ImGui::Button("Add"))
					this->m_timeActFlags.m_addTimeAct = true;

				if (this->m_timeActEditor->m_taeId != -1)
				{
					ImGui::SameLine();

					this->m_timeActFlags.m_deleteTimeActId = this->m_timeActEditor->m_taeId;

					if (ImGui::Button("Delete"))
						this->m_timeActFlags.m_deleteTimeAct = true;
				}
			}

			static ImGuiTextFilter filter;
			ImGui::Text("Filter:");
			filter.Draw("##asset searchbar", 340.f);

			if (this->m_timeAct->m_init && !this->m_timeActEditor->m_load)
			{
				ImGui::BeginChild("TAE");
				{
					for (int i = 0; i < m_timeAct->m_header.m_taeCount; i++)
					{
						bool selected = (this->m_timeActEditor->m_selectedTimeActIdx == i);

						std::string anim_name = "";

						if (this->m_timeActEditor->m_edited[i])
							anim_name += "*";

						anim_name += std::to_string(m_timeAct->m_tae[i].m_id);

						if (filter.PassFilter(anim_name.c_str()))
						{
							ImGui::PushID(i);

							ImGui::Selectable(anim_name.c_str(), &selected);

							if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
							{
								this->m_timeActEditor->m_taeId = this->m_timeAct->m_tae[i].m_id;
								this->m_timeActEditor->m_selectedTimeActIdx = i;
								this->m_timeActEditor->m_load = true;
							}

							ImGui::PopID();
						}
					}
				}
				ImGui::EndChild();
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void Application::EventTrackWindow(int* current_frame, int* first_frame, float* zoom_level, bool* is_expanded)
{
	ImGui::Begin("EventTrack");
	{
		bool focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);

		if (this->m_morphemeSystem->GetCharacterDef() && this->m_morphemeSystem->GetCharacterDef()->isLoaded())
		{
			if (ImGui::Button("Load"))
			{
				if (this->m_eventTrackEditor->m_targetAnimIdx != -1)
				{
					this->m_eventTrackEditor->m_load = true;
					this->m_eventTrackEditor->ResetSelection();
				}
				else
					g_appLog->AlertMessage(MsgLevel_Info, "No animation is selected\n");
			}

			ImGui::SameLine();
			if (ImGui::Button("Save"))
			{
				if (this->m_eventTrackEditor->GetTrackCount() > 0)
					this->m_eventTrackEditor->m_save = true;
				else
					g_appLog->AlertMessage(MsgLevel_Info, "No Event Tracks are loaded\n");
			}

			if (this->m_eventTrackEditor->m_animIdx > -1)
				ImGui::Text(this->m_animPlayer->GetAnimation()->GetAnimName());
			else
				ImGui::Text("");

			ImGui::BeginChild("sequencer");
			ImSequencer::Sequencer(m_eventTrackEditor, current_frame, &this->m_eventTrackEditor->m_selectedTrack, &this->m_eventTrackEditor->m_selectedEvent, is_expanded, focused, first_frame, zoom_level, ImSequencer::EDITOR_EDIT_ALL | ImSequencer::EDITOR_TRACK_ADD | ImSequencer::EDITOR_TRACK_RENAME | ImSequencer::EDITOR_EVENT_ADD | ImSequencer::EDITOR_MARK_ACTIVE_EVENTS);
			ImGui::EndChild();
		}
	}
	ImGui::End();
}

void Application::EventTrackInfoWindow()
{
	ImGui::Begin("Event Data");
	{
		if ((this->m_eventTrackEditor->m_eventTracks.size() > 0) && (this->m_eventTrackEditor->m_selectedTrack != -1 && this->m_eventTrackEditor->m_selectedTrack < this->m_eventTrackEditor->m_eventTracks.size()) && (this->m_eventTrackEditor->m_selectedEvent != -1 && this->m_eventTrackEditor->m_eventTracks[this->m_eventTrackEditor->m_selectedTrack].m_numEvents))
		{
			EventTrackEditor::EventTrack* track = &this->m_eventTrackEditor->m_eventTracks[this->m_eventTrackEditor->m_selectedTrack];
			float startTime = RMath::FrameToTime(track->m_event[this->m_eventTrackEditor->m_selectedEvent].m_frameStart);
			float endTime = RMath::FrameToTime(track->m_event[this->m_eventTrackEditor->m_selectedEvent].m_duration + track->m_event[this->m_eventTrackEditor->m_selectedEvent].m_frameStart);

			ImGui::Text(track->m_name.c_str());
			ImGui::PushItemWidth(100);

			ImGui::InputInt("User Data", &track->m_event[this->m_eventTrackEditor->m_selectedEvent].m_value, 1, 0);
			if (ImGui::IsItemHovered())
			{
				ImGui::SeparatorText("Info");

				ImGui::PushTextWrapPos(ImGui::GetWindowContentWidth());
				ImGui::Text(getEventTrackEventTooltip(track->m_event[this->m_eventTrackEditor->m_selectedEvent].m_value).c_str());
				ImGui::PopTextWrapPos();
			}

			ImGui::InputFloat("Start Time", &startTime, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);

			if (!track->m_discrete)
				ImGui::InputFloat("End Time", &endTime, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);

			ImGui::PopItemWidth();

			if (this->m_eventTrackEditor->m_save)
			{
				this->m_eventTrackEditor->m_save = false;
				this->m_eventTrackEditor->SetEditedState(false);

				try
				{
					this->m_eventTrackEditor->SaveEventTracks();
				}
				catch (const std::exception& e)
				{
					g_appLog->PanicMessage(e.what());
				}
			}
		}
	}

	ImGui::End();
}

void Application::TimeActWindow(int* current_frame, int* first_frame, float* zoom_level, bool* is_expanded)
{
	ImGui::Begin("TimeAct");

	{
		bool focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);

		if (this->m_timeAct)
		{
			if (ImGui::Button("Load"))
			{
				if (this->m_timeAct->m_init)
				{
					this->m_timeActEditor->m_load = true;
					this->m_timeActEditor->ResetSelection();
				}
				else
					g_appLog->AlertMessage(MsgLevel_Info, "No TimeAct file is currently loaded\n");
			}

			ImGui::SameLine();
			if (ImGui::Button("Save"))
			{
				if (this->m_timeAct->m_init)
					this->m_timeActEditor->m_save = true;
				else
					g_appLog->AlertMessage(MsgLevel_Info, "No TimeAct track is currently loaded\n");
			}

			if (this->m_timeActEditor->m_taeId > -1)
				ImGui::Text(std::to_string(this->m_timeActEditor->m_taeId).c_str());
			else
				ImGui::Text("");

			ImGui::BeginChild("sequencer");
			ImSequencer::Sequencer(m_timeActEditor, current_frame, &this->m_timeActEditor->m_selectedTrack, &this->m_timeActEditor->m_selectedEvent, is_expanded, focused, first_frame, zoom_level, ImSequencer::EDITOR_EDIT_ALL | ImSequencer::EDITOR_TRACK_ADD | ImSequencer::EDITOR_TRACK_RENAME | ImSequencer::EDITOR_EVENT_ADD | ImSequencer::EDITOR_MARK_ACTIVE_EVENTS);
			ImGui::EndChild();
		}
	}

	ImGui::End();
}

void Application::TimeActInfoWindow()
{
	ImGui::Begin("TimeAct Data");
	{
		if ((this->m_timeActEditor->GetTrackCount() > 0) && (this->m_timeActEditor->m_selectedTrack != -1 && this->m_timeActEditor->m_selectedTrack < this->m_timeActEditor->GetTrackCount()) && (this->m_timeActEditor->m_selectedEvent != -1 && this->m_timeActEditor->m_tracks[this->m_timeActEditor->m_selectedTrack].m_count))
		{
			TimeActEditor::TimeActTrack* track = &this->m_timeActEditor->m_tracks[this->m_timeActEditor->m_selectedTrack];
			float startTime = RMath::FrameToTime(track->m_event[this->m_timeActEditor->m_selectedEvent].m_frameStart, 30);
			float endTime = RMath::FrameToTime(track->m_event[this->m_timeActEditor->m_selectedEvent].m_duration + track->m_event[this->m_timeActEditor->m_selectedEvent].m_frameStart, 30);

			ImGui::Text(m_timeActEditor->GetEventLabel(this->m_timeActEditor->m_selectedTrack, this->m_timeActEditor->m_selectedEvent, false).c_str());
			ImGui::PushItemWidth(100);

			ImGui::InputInt("Event ID", &track->m_event[this->m_timeActEditor->m_selectedEvent].m_value, 1, 0);
			if (ImGui::IsItemHovered())
			{
				ImGui::SeparatorText("Info");

				ImGui::PushTextWrapPos(ImGui::GetWindowContentWidth());
				ImGui::Text(getTaeEventTooltip(track->m_event[this->m_timeActEditor->m_selectedEvent].m_value).c_str());
				ImGui::PopTextWrapPos();
			}

			ImGui::InputFloat("Start Time", &startTime, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat("End Time", &endTime, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);

			track->m_event[this->m_timeActEditor->m_selectedEvent].m_args->ImGuiEdit();

			if (this->m_timeActEditor->m_save)
			{
				this->m_timeActEditor->m_save = false;
				this->m_timeActEditor->SetEditedState(false);

				for (int i = 0; i < this->m_timeActEditor->GetTrackCount(); i++)
					this->m_timeActEditor->m_tracks[i].SaveTimeActTrack();
			}

			ImGui::PopItemWidth();
		}
	}

	ImGui::End();
}

void Application::SettingsWindow()
{
	ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_Appearing);
	ImGui::Begin("Settings##gui", &this->m_windowStates.m_settingWindow);

	ImGui::BeginTabBar("settings");

	if (ImGui::BeginTabItem("Style"))
	{
		ImGui::ShowStyleEditor();

		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("EventTrack Editor"))
	{
		ImGui::ColorEdit4("Track", (float*)&this->m_eventTrackEditor->m_colors.m_trackColor);
		ImGui::ColorEdit4("Track Inactive", (float*)&this->m_eventTrackEditor->m_colors.m_trackColorInactive);
		ImGui::ColorEdit4("Track Active", (float*)&this->m_eventTrackEditor->m_colors.m_trackColorActive);
		ImGui::ColorEdit4("Track Bounding Box", (float*)&this->m_eventTrackEditor->m_colors.m_trackBoundingBox);
		ImGui::ColorEdit4("Track Bounding Box Active", (float*)&this->m_eventTrackEditor->m_colors.m_trackBoundingBoxActive);
		ImGui::ColorEdit4("Track Text Color", (float*)&this->m_eventTrackEditor->m_colors.m_trackTextColor);
		ImGui::ColorEdit4("Cursor Color", (float*)&this->m_eventTrackEditor->m_colors.m_cursorColor);

		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("TimeAct Editor"))
	{
		ImGui::ColorEdit4("Track", (float*)&this->m_timeActEditor->m_colors.m_trackColor);
		ImGui::ColorEdit4("Track Inactive", (float*)&this->m_timeActEditor->m_colors.m_trackColorInactive);
		ImGui::ColorEdit4("Track Active", (float*)&this->m_timeActEditor->m_colors.m_trackColorActive);
		ImGui::ColorEdit4("Track Bounding Box", (float*)&this->m_timeActEditor->m_colors.m_trackBoundingBox);
		ImGui::ColorEdit4("Track Bounding Box Active", (float*)&this->m_timeActEditor->m_colors.m_trackBoundingBoxActive);
		ImGui::ColorEdit4("Track Text Color", (float*)&this->m_timeActEditor->m_colors.m_trackTextColor);
		ImGui::ColorEdit4("Cursor Color", (float*)&this->m_timeActEditor->m_colors.m_cursorColor);

		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();

	ImGui::End();
}

void Application::PreviewDebugManagerWindow()
{
	ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_Appearing);
	ImGui::Begin("Scene Settings", &this->m_windowStates.m_previewSettings);

	ImGui::DragFloat("Grid Scale", &g_scene->m_settings.m_gridScale, 0.1f, 0.f, FLT_MAX);

	ImGui::BeginTabBar("preview_settings");
	if (ImGui::BeginTabItem("Camera"))
	{
		ImGui::SeparatorText("Parameters");

		ImGui::DragFloat("Target Distance", &g_scene->m_camera.m_radius, 0.1f, 0.1f, 9999.f);
		ImGui::InputFloat3("Target Offset", &g_scene->m_camera.m_offsets.x);
		ImGui::InputFloat3("Camera Position", &g_scene->m_camera.m_position.x);
		ImGui::InputFloat3("Camera Angles", &g_scene->m_camera.m_angles.x);
		ImGui::DragFloat("FOV", &g_scene->m_camera.m_fov, 0.1f, 0.1f, DirectX::XM_PI);
		ImGui::InputFloat("Near Plane", &g_scene->m_camera.m_nearZ);
		ImGui::InputFloat("Far Plane", &g_scene->m_camera.m_farZ);

		ImGui::InputFloat("Width", &g_scene->m_camera.m_width, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat("Height", &g_scene->m_camera.m_height, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat("Aspect Ratio", &g_scene->m_camera.m_aspectRatio, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);

		ImGui::SeparatorText("Input");

		ImGui::Checkbox("Invert X Drag", &g_scene->m_camera.m_settings.m_dragInvertX);
		ImGui::Checkbox("Invert Y Drag", &g_scene->m_camera.m_settings.m_dragInvertY);

		ImGui::Checkbox("Invert X Rotation", &g_scene->m_camera.m_settings.m_rotInvertX);
		ImGui::Checkbox("Invert Y Rotation", &g_scene->m_camera.m_settings.m_rotInvertY);

		ImGui::SeparatorText("Speed Settings");

		ImGui::DragFloat("Drag Speed", &g_scene->m_camera.m_settings.m_speedParams.m_dragSpeed, 0.1f, 0.f, 10.f);
		ImGui::DragFloat("Zoom Speed", &g_scene->m_camera.m_settings.m_speedParams.m_zoomSpeed, 0.1f, 0.f, 100.f);
	
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Background"))
	{
		ImGui::ColorEdit4("Background Color", &g_scene->m_settings.m_backgroundColor.x);

		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
	
	ImGui::End();
}

void SkeletonInspectorTreeNode(FLVER2* flv, int boneID, int& selected_id)
{
	if (boneID == -1)
		return;

	int childIndex = flv->bones[boneID].childIndex;

	if (childIndex == -1)
	{
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		if (selected_id == boneID)
			node_flags |= ImGuiTreeNodeFlags_Selected;

		ImGui::TreeNodeEx(RString::ToNarrow(flv->bones[boneID].name).c_str(), node_flags);

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			selected_id = boneID;
	}
	else
	{
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

		if (selected_id == boneID)
			node_flags |= ImGuiTreeNodeFlags_Selected;

		bool open = ImGui::TreeNodeEx(RString::ToNarrow(flv->bones[boneID].name).c_str(), node_flags);

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			selected_id = boneID;

		if (open)
		{
			int siblingIndex = childIndex;

			while (siblingIndex != -1)
			{
				SkeletonInspectorTreeNode(flv, siblingIndex, selected_id);

				siblingIndex = flv->bones[siblingIndex].nextSiblingIndex;
			}

			ImGui::TreePop();
		}
	}
}

void ModelTreeNode(Application* application, FlverModel* model)
{
	if (model)
	{
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

		if (application->m_sceneFlags.m_selectedModel == model)
			node_flags |= ImGuiTreeNodeFlags_Selected;

		bool open = ImGui::TreeNodeEx(model->m_name.c_str(), node_flags);

		if (open)
		{
			if (model->m_flver->header.boneCount > 0)
			{
				if (ImGui::TreeNode("Bones"))
				{
					ImGui::BeginChild("skeleton_inspector", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetWindowSize().y / 3));

					int boneID = 0;

					while (boneID != -1)
					{
						SkeletonInspectorTreeNode(model->m_flver, boneID, model->m_settings.m_selectedBone);

						boneID = model->m_flver->bones[boneID].nextSiblingIndex;
					}

					ImGui::EndChild();

					ImGui::TreePop();
				}
			}

			if (model->m_flver->header.dummyCount > 0)
			{
				if (ImGui::TreeNode("Dummy Polygons"))
				{
					ImGui::BeginChild("dummy_inspector", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetWindowSize().y / 3));

					for (size_t i = 0; i < model->m_flver->header.dummyCount; i++)
					{
						std::string dummy_name = "Dmy_" + std::to_string(model->m_flver->dummies[i].referenceID);

						bool selected = (model->m_settings.m_selectedDummy == i);

						if (ImGui::Selectable(dummy_name.c_str(), selected))
							model->m_settings.m_selectedDummy = i;
					}

					ImGui::EndChild();

					ImGui::TreePop();
				}

			}

			if (ImGui::IsMouseDoubleClicked(0))
			{
				model->m_settings.m_selectedDummy = -1;
				model->m_settings.m_selectedBone = -1;
			}

			ImGui::TreePop();
		}

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			application->m_sceneFlags.m_selectedModel = model;

		if (ImGui::IsMouseDoubleClicked(0))
			application->m_sceneFlags.m_selectedModel = nullptr;
	}

}

void Application::PreviewSceneExplorerWindow()
{
	ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_Appearing);

	ImGui::Begin("Scene Explorer", &this->m_sceneFlags.m_sceneExplorer);

	FlverModel* model = this->m_animPlayer->GetModel();	
	ModelTreeNode(this, model);

	model = this->m_animPlayer->GetModelPart(Parts_Face);
	ModelTreeNode(this, model);

	model = this->m_animPlayer->GetModelPart(Parts_Head);
	ModelTreeNode(this, model);

	model = this->m_animPlayer->GetModelPart(Parts_Body);
	ModelTreeNode(this, model);

	model = this->m_animPlayer->GetModelPart(Parts_Arm);
	ModelTreeNode(this, model);

	model = this->m_animPlayer->GetModelPart(Parts_Leg);
	ModelTreeNode(this, model);

	model = this->m_animPlayer->GetModelPart(Parts_WeaponLeft);
	ModelTreeNode(this, model);

	model = this->m_animPlayer->GetModelPart(Parts_WeaponRight);
	ModelTreeNode(this, model);

	model = this->m_animPlayer->GetModelPartFacegen(FaceGen_Face);
	ModelTreeNode(this, model);

	model = this->m_animPlayer->GetModelPartFacegen(FaceGen_Head);
	ModelTreeNode(this, model);

	model = this->m_animPlayer->GetModelPartFacegen(FaceGen_Eyes);
	ModelTreeNode(this, model);

	model = this->m_animPlayer->GetModelPartFacegen(FaceGen_EyeBrows);
	ModelTreeNode(this, model);

	model = this->m_animPlayer->GetModelPartFacegen(FaceGen_Beard);
	ModelTreeNode(this, model);

	model = this->m_animPlayer->GetModelPartFacegen(FaceGen_Hair);
	ModelTreeNode(this, model);

	ImGui::End();
}

void SaveModelPartsFaceGenPreset(Application* application, FgPartType type, int value)
{
	switch (type)
	{
	case FaceGen_Face:
		application->m_playerModelPreset->SetInt("FaceGen", "face", value);
		break;
	case FaceGen_Head:
		application->m_playerModelPreset->SetInt("FaceGen", "head", value);
		break;
	case FaceGen_Eyes:
		application->m_playerModelPreset->SetInt("FaceGen", "eye", value);
		break;
	case FaceGen_EyeBrows:
		application->m_playerModelPreset->SetInt("FaceGen", "eye_brows", value);
		break;
	case FaceGen_Beard:
		application->m_playerModelPreset->SetInt("FaceGen", "beard", value);
		break;
	case FaceGen_Hair:
		application->m_playerModelPreset->SetInt("FaceGen", "hair", value);
		break;
	default:
		break;
	}
}

void SaveModelPartsArmorPreset(Application* application, PartType type, int value)
{
	switch (type)
	{
	case Parts_Head:
		application->m_playerModelPreset->SetInt("Armor", "head", value);
		break;
	case Parts_Face:
		application->m_playerModelPreset->SetInt("Armor", "face", value);
		break;
	case Parts_Body:
		application->m_playerModelPreset->SetInt("Armor", "body", value);
		break;
	case Parts_Arm:
		application->m_playerModelPreset->SetInt("Armor", "arm", value);
		break;
	case Parts_Leg:
		application->m_playerModelPreset->SetInt("Armor", "leg", value);
		break;
	default:
		break;
	}
}

void SaveModelPartsWeaponPreset(Application* application, PartType type, int value, bool shield)
{
	switch (type)
	{
	case Parts_WeaponLeft:
		application->m_playerModelPreset->SetInt("Left", "id", value);
		application->m_playerModelPreset->SetBool("Left", "is_shield", shield);
		break;
	case Parts_WeaponRight:
		application->m_playerModelPreset->SetInt("Right", "id", value);
		application->m_playerModelPreset->SetBool("Right", "is_shield", shield);
		break;
	}
}

void ModelPartsFgList(Application* application, std::vector<FileNamePathPair> paths, FgPartType type)
{
	if (paths.size() == 0)
		return;

	char childName[255];
	sprintf_s(childName, "fgParts_%ls_%d", paths[0].m_path.c_str(), type);

	FlverModel* currentPart = application->m_animPlayer->GetModelPartFacegen(type);

	ImGui::BeginChild(childName, ImVec2(ImGui::GetContentRegionAvail().x, 300));

	if (ImGui::Selectable("None", currentPart == nullptr))
	{
		application->m_animPlayer->SetModelPartFacegen(type, nullptr);

		SaveModelPartsFaceGenPreset(application, type, -1);
	}

	for (size_t i = 0; i < paths.size(); i++)
	{
		bool selected = false;
		bool is_female = application->m_playerModelPreset->GetBool("Gender", "is_female", false);

		if (type != FaceGen_EyeBrows)
		{
			if (!IsEquipSameGenderAs(paths[i].m_path, is_female))
				continue;
		}

		if (application->m_animPlayer->GetModelPartFacegen(type))
			selected = (application->m_animPlayer->GetModelPartFacegen(type)->m_name.compare(paths[i].m_name) == 0);

		ImGui::Selectable(paths[i].m_name.c_str(), selected);

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
		{
			try
			{
				FlverModel* model = FlverModel::CreateFromBnd(paths[i].m_path);

				if (model)
				{
					application->m_animPlayer->SetModelPartFacegen(type, model);
					application->m_animPlayer->GetModelPartFacegen(type)->CreateFlverToMorphemeBoneMap(application->m_morphemeSystem->GetCharacterDef()->getNetworkDef()->getRig(0));

					SaveModelPartsFaceGenPreset(application, type, GetFgIDByFilename(RString::ToWide(paths[i].m_name), type));
				}
				else
					delete model;
			}
			catch (const std::exception& e)
			{
				g_appLog->PanicMessage(e.what());
			}
		}
	}

	ImGui::EndChild();
}

void ModelPartsList(Application* application, std::vector<FileNamePathPair> paths, PartType type)
{
	if (paths.size() == 0)
		return;

	char childName[255];
	sprintf_s(childName, "parts_%ls_%d", paths[0].m_path.c_str(), type);

	FlverModel* currentPart = application->m_animPlayer->GetModelPart(type);

	ImGui::BeginChild(childName, ImVec2(ImGui::GetContentRegionAvail().x, 300));

	if (ImGui::Selectable("None", currentPart == nullptr))
	{
		application->m_animPlayer->SetModelPart(type, nullptr);

		if (type == Parts_WeaponLeft || type == Parts_WeaponRight)
			SaveModelPartsWeaponPreset(application, type, -1, false);
		else
			SaveModelPartsArmorPreset(application, type, -1);
	}

	for (size_t i = 0; i < paths.size(); i++)
	{
		bool selected = false;
		bool is_female = application->m_playerModelPreset->GetBool("Gender", "is_female", false);

		if ((type != Parts_WeaponLeft) && (type != Parts_WeaponRight))
		{
			if (!IsEquipSameGenderAs(paths[i].m_path, is_female))
				continue;
		}

		if (application->m_animPlayer->GetModelPart(type))
			selected = (application->m_animPlayer->GetModelPart(type)->m_name.compare(paths[i].m_name) == 0);

		ImGui::Selectable(paths[i].m_name.c_str(), selected);

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
		{
			try
			{
				FlverModel* model = FlverModel::CreateFromBnd(paths[i].m_path);

				if (model)
				{
					application->m_animPlayer->SetModelPart(type, model);
					application->m_animPlayer->GetModelPart(type)->CreateFlverToMorphemeBoneMap(application->m_morphemeSystem->GetCharacterDef()->getNetworkDef()->getRig(0));
					
					if (type == Parts_WeaponLeft || type == Parts_WeaponRight)
						SaveModelPartsWeaponPreset(application, type, GetEquipIDByFilename(RString::ToWide(paths[i].m_name)), IsEquipShield(RString::ToWide(paths[i].m_name)));
					else
						SaveModelPartsArmorPreset(application, type, GetEquipIDByFilename(RString::ToWide(paths[i].m_name)));
				}
				else
					delete model;
			}
			catch (const std::exception& e)
			{
				g_appLog->PanicMessage(e.what());
			}
		}
	}

	ImGui::EndChild();
}

void Application::PlayerCharacterPartsManager()
{
	ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_Appearing);

	ImGui::Begin("Player Model", &this->m_windowStates.m_playerPartsManagerWindow);

	static bool female = this->m_playerModelPreset->GetBool("Gender", "is_female", false);
	bool female_bak = female;

	ImGui::Checkbox("Female", &female);

	this->m_playerModelPreset->SetBool("Gender", "is_female", female);

	if (female != female_bak)
	{
		std::wstring parts_path = this->m_gamePath + L"\\model\\parts";

		LoadPlayerModelParts(this, parts_path);
	}

	ImGui::BeginTabBar("parts");
	if (ImGui::BeginTabItem("Equip"))
	{
		if (this->m_fileNameMapPairList->m_weaponModelPaths.size() && this->m_fileNameMapPairList->m_shieldModelPaths.size())
		{
			if (ImGui::TreeNode("Right Hand"))
			{
				ImGui::SeparatorText("Weapon");
				ModelPartsList(this, this->m_fileNameMapPairList->m_weaponModelPaths, Parts_WeaponRight);

				ImGui::SeparatorText("Shield");
				ModelPartsList(this, this->m_fileNameMapPairList->m_shieldModelPaths, Parts_WeaponRight);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Left Hand"))
			{
				ImGui::SeparatorText("Weapon");
				ModelPartsList(this, this->m_fileNameMapPairList->m_weaponModelPaths, Parts_WeaponLeft);

				ImGui::SeparatorText("Shield");
				ModelPartsList(this, this->m_fileNameMapPairList->m_shieldModelPaths, Parts_WeaponLeft);

				ImGui::TreePop();
			}
		}

		if (this->m_fileNameMapPairList->m_headModelPaths.size())
		{
			if (ImGui::TreeNode("Head"))
			{
				ModelPartsList(this, this->m_fileNameMapPairList->m_headModelPaths, Parts_Head);

				ImGui::TreePop();
			}
		}

		if (this->m_fileNameMapPairList->m_bodyModelPaths.size())
		{
			if (ImGui::TreeNode("Body"))
			{
				ModelPartsList(this, this->m_fileNameMapPairList->m_bodyModelPaths, Parts_Body);

				ImGui::TreePop();
			}
		}

		if (this->m_fileNameMapPairList->m_armModelPaths.size())
		{
			if (ImGui::TreeNode("Arm"))
			{
				ModelPartsList(this, this->m_fileNameMapPairList->m_armModelPaths, Parts_Arm);

				ImGui::TreePop();
			}
		}

		if (this->m_fileNameMapPairList->m_legModelPaths.size())
		{
			if (ImGui::TreeNode("Leg"))
			{
				ModelPartsList(this, this->m_fileNameMapPairList->m_legModelPaths, Parts_Leg);

				ImGui::TreePop();
			}
		}

		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("FaceGen"))
	{

		if (this->m_fileNameMapPairList->m_fgFace.size())
		{
			if (ImGui::TreeNode("Face"))
			{
				ModelPartsFgList(this, this->m_fileNameMapPairList->m_fgFace, FaceGen_Face);
				ImGui::TreePop();
			}
		}

		if (this->m_fileNameMapPairList->m_fgHead.size())
		{
			if (ImGui::TreeNode("Head"))
			{
				ModelPartsFgList(this, this->m_fileNameMapPairList->m_fgHead, FaceGen_Head);
				ImGui::TreePop();
			}
		}

		if (this->m_fileNameMapPairList->m_fgEyes.size())
		{
			if (ImGui::TreeNode("Eyes"))
			{
				ModelPartsFgList(this, this->m_fileNameMapPairList->m_fgEyes, FaceGen_Eyes);
				ImGui::TreePop();
			}
		}

		if (this->m_fileNameMapPairList->m_fgEyeBrows.size())
		{
			if (ImGui::TreeNode("EyeBrows"))
			{
				ModelPartsFgList(this, this->m_fileNameMapPairList->m_fgEyeBrows, FaceGen_EyeBrows);
				ImGui::TreePop();
			}
		}

		if (this->m_fileNameMapPairList->m_fgBeard.size())
		{
			if (ImGui::TreeNode("Beard"))
			{
				ModelPartsFgList(this, this->m_fileNameMapPairList->m_fgBeard, FaceGen_Beard);
				ImGui::TreePop();
			}
		}

		if (this->m_fileNameMapPairList->m_fgHair.size())
		{
			if (ImGui::TreeNode("Hair"))
			{
				ModelPartsFgList(this, this->m_fileNameMapPairList->m_fgHair, FaceGen_Hair);
				ImGui::TreePop();
			}
		}

		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();

	ImGui::End();
}

void SetModelFlags(FlverModel* model, DisplayMode mode, bool showDummies)
{
	if (model)
	{
		model->m_settings.m_displayMode = mode;
		model->m_settings.m_drawDummyPolygons = showDummies;
	}
}

EventTrackEditor::EventTrack* getTimeActTrackActionCtrl(EventTrackEditor* trackList)
{
	for (size_t i = 0; i < trackList->m_eventTracks.size(); i++)
	{
		if (trackList->m_eventTracks[i].m_eventId == 1000)
			return &trackList->m_eventTracks[i];
	}

	return nullptr;
}

void LoadMorphemeMarkup(Application* pApplication)
{
	pApplication->m_eventTrackEditor->m_load = false;
	pApplication->m_eventTrackEditor->Clear();

	CharacterDef* characterDef = pApplication->m_morphemeSystem->GetCharacterDef();

	if ((characterDef) && characterDef->isLoaded() && (pApplication->m_eventTrackEditor->m_targetAnimIdx != -1))
	{
		pApplication->m_eventTrackEditor->m_eventTrackActionTimeActValue = -1;
		pApplication->m_eventTrackEditor->m_eventTrackActionTimeActStart = 0.f;
		pApplication->m_eventTrackEditor->m_eventTrackActionTimeActDuration = 0.f;

		AnimSourceInterface* animSource = characterDef->getAnimationById(pApplication->m_eventTrackEditor->m_targetAnimIdx);
		pApplication->m_eventTrackEditor->m_animSource = animSource;

		ME::TakeListXML* takeList = animSource->GetTakeList();
		if (takeList)
		{
			float animLen = animSource->GetTakeList()->getTake(0)->getCachedTakeSecondsDuration();

			pApplication->m_eventTrackEditor->m_frameMin = RMath::TimeToFrame(takeList->getTake(0)->getClipStart() * animLen);
			pApplication->m_eventTrackEditor->m_frameMax = RMath::TimeToFrame(takeList->getTake(0)->getClipEnd() * animLen);

			pApplication->m_eventTrackEditor->m_animIdx = -1;

			for (int i = 0; i < characterDef->getAnimFileLookUp()->getNumAnims(); i++)
			{
				if (characterDef->getAnimation(i)->GetID() == animSource->GetID())
					pApplication->m_eventTrackEditor->m_animIdx = i;
			}

			pApplication->m_eventTrackEditor->m_lenMult = takeList->getTake(0)->getCachedTakeSecondsDuration() / (takeList->getTake(0)->getClipEnd() - takeList->getTake(0)->getClipStart());
			int track_count = takeList->getTake(0)->getNumEventTracks();

			pApplication->m_eventTrackEditor->m_eventTracks.reserve(track_count);

			for (size_t i = 0; i < track_count; i++)
			{
				ME::EventTrackExport* eventTrackXML = takeList->getTake(0)->getEventTrack(i);

				pApplication->m_eventTrackEditor->m_eventTracks.push_back(EventTrackEditor::EventTrack(eventTrackXML, pApplication->m_eventTrackEditor->m_lenMult));
			}

			pApplication->m_eventTrackEditor->SetEditedState(false);

			if (pApplication->m_timeAct)
			{
				EventTrackEditor::EventTrack* timeActTrackAction = getTimeActTrackActionCtrl(pApplication->m_eventTrackEditor);

				if (timeActTrackAction)
				{
					pApplication->m_eventTrackEditor->m_eventTrackActionTimeActValue = timeActTrackAction->m_event[0].m_value;
					pApplication->m_eventTrackEditor->m_eventTrackActionTimeActStart = RMath::FrameToTime(timeActTrackAction->m_event[0].m_frameStart);
					pApplication->m_eventTrackEditor->m_eventTrackActionTimeActDuration = RMath::FrameToTime(timeActTrackAction->m_event[0].m_duration);

					pApplication->m_timeActEditor->m_taeId = pApplication->m_eventTrackEditor->m_eventTrackActionTimeActValue;

					for (size_t j = 0; j < pApplication->m_timeAct->m_tae.size(); j++)
					{
						if (pApplication->m_timeAct->m_tae[j].m_id == pApplication->m_timeActEditor->m_taeId)
							pApplication->m_timeActEditor->m_selectedTimeActIdx = j;
					}

					pApplication->m_timeActEditor->m_load = true;
				}
				else
				{
					g_appLog->DebugMessage(MsgLevel_Warn, "Animation %d has no TimeAct associated to it\n", animSource->GetID());
				}
			}
		}
		else
		{
			g_appLog->DebugMessage(MsgLevel_Debug, "Animation %d has no event tracks associated to it\n", animSource->GetID());
		}

		g_appLog->DebugMessage(MsgLevel_Debug, "\n");
	}

}

void LoadTimeActTracks(Application* pApplication)
{
	pApplication->m_timeActEditor->m_load = false;
	pApplication->m_timeActEditor->Clear();

	pApplication->m_timeActEditor->m_taeIdx = pApplication->m_timeActEditor->m_selectedTimeActIdx;

	if (pApplication->m_timeAct && pApplication->m_timeActEditor->m_taeId > -1)
	{
		if (pApplication->m_timeAct->m_tae.size() > 0)
		{
			TimeAct* timeact = pApplication->m_timeAct->TimeActLookup(pApplication->m_timeActEditor->m_taeId);
			pApplication->m_timeActEditor->m_source = timeact;

			if (timeact)
			{
				float trackLen = (float)timeact->m_taeData->m_animData->m_lenght / (float)timeact->m_taeData->m_animData->m_fps;

				pApplication->m_timeActEditor->m_frameMax = RMath::TimeToFrame(trackLen, 30);
				pApplication->m_timeActEditor->m_frameMin = 0;

				pApplication->m_timeActEditor->SetEditedState(false);

				if (timeact->m_taeData->m_eventGroupCount > 0)
				{
					for (int j = 0; j < timeact->m_taeData->m_eventGroupCount; j++)
						pApplication->m_timeActEditor->m_tracks.push_back(&timeact->m_taeData->m_groups[j]);
				}
			}
			else
				g_appLog->DebugMessage(MsgLevel_Info, "Application.cpp", "TimeAct %d not found\n", pApplication->m_timeActEditor->m_taeId);
		}
		else
			g_appLog->DebugMessage(MsgLevel_Info, "Application.cpp", "No TimeAct is loaded\n");
	}
}

void Application::CheckFlags()
{
	if (this->m_windowStates.m_settingWindow)
	{
		this->SettingsWindow();
	}

	if (this->m_windowStates.m_previewSettings)
	{
		this->PreviewDebugManagerWindow();
	}

	if (this->m_windowStates.m_playerPartsManagerWindow)
	{
		this->PlayerCharacterPartsManager();
	}

	if (this->m_windowStates.m_queryEventTrack)
	{

	}

	if (this->m_windowStates.m_queryTae)
	{

	}

	if (this->m_flags.m_loadFile)
	{
		this->m_flags.m_loadFile = false;

		m_eventTrackEditor->m_load = false;
		m_timeActEditor->m_load = false;

		this->m_eventTrackEditor->Clear();
		this->m_timeActEditor->Clear();

		this->LoadFile();
	}

	if (this->m_flags.m_saveNmb)
	{
		this->m_flags.m_saveNmb = false;
	}

	if (this->m_flags.m_saveTae)
	{
		this->m_flags.m_saveTae = false;

		if (this->m_timeAct->m_init)
			this->SaveTaeFile();
		else
			g_appLog->AlertMessage(MsgLevel_Error, "No TAE file is loaded\n");
	}

	if (this->m_flags.m_saveAll)
	{
		this->m_flags.m_saveAll = false;

		if (this->m_timeAct->m_init)
		{
			bool status = m_timeAct->SaveFile(this->m_timeAct->m_filePath);

			if (status)
				g_appLog->DebugMessage(MsgLevel_Debug, "Save file %ls (taeCount=%d)\n", m_timeAct->m_outFilePath, m_timeAct->m_header.m_taeCount);
			else
				g_appLog->AlertMessage(MsgLevel_Error, "Failed to generate file\n");
		}
	}

	CharacterDef* characterDef = this->m_morphemeSystem->GetCharacterDef();

	if (characterDef)
	{
		if (this->m_flags.m_exportAll)
		{
			this->m_flags.m_exportAll = false;

			this->m_flags.m_exportAnimations = true;
			this->m_flags.m_exportModel = true;
			this->m_flags.m_exportNetwork = true;
		}

		if (this->m_flags.m_exportNetwork)
		{
			this->m_flags.m_exportNetwork = false;

			std::wstring out_path = L"Export";

			wchar_t chr_id_str[50];
			swprintf_s(chr_id_str, L"c%04d", characterDef->getCharacterId());

			out_path += L"\\" + std::wstring(chr_id_str) + L"\\";

			std::filesystem::create_directories(out_path);

			ME::NetworkDefExportXML* netDefExport = MorphemeExport::ExportNetwork(characterDef->getNetworkDef(), characterDef->getCharacterId(), out_path + chr_id_str + L".xml");

			if (!netDefExport->write())
				g_appLog->DebugMessage(MsgLevel_Error, "Failed to export network to XML for %ws\n", chr_id_str);

			for (size_t i = 0; i < characterDef->getNumCharacterControllers(); i++)
			{
				wchar_t filename[260];
				swprintf_s(filename, L"c%04d_%d.mrctrl", characterDef->getCharacterId(), i);

				ME::CharacterControllerExportXML* characterControllerExport = MorphemeExport::ExportCharacterController(characterDef->getCharacterController(i), out_path + filename);
				
				if (!characterControllerExport->write())
					g_appLog->DebugMessage(MsgLevel_Error, "Failed to export CharacterController to XML for %ws (animSet=%d)\n", chr_id_str, i);
			}

			for (size_t i = 0; i < characterDef->getNetworkDef()->getNumAnimSets(); i++)
			{
				wchar_t filename[260];
				swprintf_s(filename, L"c%04d_%d.mrarig", characterDef->getCharacterId(), i);

				ME::RigExportXML* rigExport = MorphemeExport::ExportRig(characterDef->getNetworkDef(), characterDef->getNetworkDef()->getRig(i), out_path + filename);

				if (!rigExport->write())
					g_appLog->DebugMessage(MsgLevel_Error, "Failed to export rig to XML for %ws (animSet=%d)\n", chr_id_str, i);
			}
		}

		if (this->m_flags.m_exportAnimations)
		{
			this->m_flags.m_exportAnimations = false;

			int numAnims = characterDef->getAnimFileLookUp()->getNumAnims();

			if (numAnims == 0)
				g_appLog->AlertMessage(MsgLevel_Warn, "No animations are loaded");

			std::wstring out_path = L"Export";

			wchar_t chr_id_str[50];
			swprintf_s(chr_id_str, L"c%04d", characterDef->getCharacterId());

			out_path += L"\\" + std::wstring(chr_id_str) + L"\\";

			std::filesystem::create_directories(out_path);

			PWSTR export_path = (wchar_t*)out_path.c_str();

			if (this->m_fbxExportFlags.m_exportModelWithAnims)
			{
				if (!this->ExportModelToFbx(export_path))
					g_appLog->AlertMessage(MsgLevel_Error, "Failed to export FBX model (chrId=c%04d)\n", characterDef->getCharacterId());
			}

			for (int i = 0; i < numAnims; i++)
			{
				std::filesystem::path anim_out = std::filesystem::path(RString::ToNarrow(export_path) + "Animations\\" + RString::RemoveExtension(characterDef->getAnimFileLookUp()->getSourceFilename(i)) + ".fbx");
				std::filesystem::create_directories(anim_out.parent_path());

				if (!this->ExportAnimationToFbx(anim_out, i))
					g_appLog->DebugMessage(MsgLevel_Error, "Failed to export animation %d\n", i);

				ME::TakeListXML* takeListXML = characterDef->getAnimation(i)->GetTakeList();

				if (takeListXML)
				{
					std::filesystem::path takeListPath = takeListXML->getDestFilename();
					std::filesystem::create_directories(takeListPath.parent_path());

					if (takeListXML->write())
						g_appLog->DebugMessage(MsgLevel_Error, "Failed to export event track to XML for animation %d\n", i);
				}
			}
		}

		if (this->m_flags.m_exportModel)
		{
			this->m_flags.m_exportModel = false;

			std::wstring out_path = L"Export";

			wchar_t chr_id_str[50];
			swprintf_s(chr_id_str, L"c%04d", characterDef->getCharacterId());

			out_path += L"\\" + std::wstring(chr_id_str) + L"\\";

			std::filesystem::create_directories(out_path);

			PWSTR export_path = (wchar_t*)out_path.c_str();

			this->ExportModelToFbx(export_path);
		}
	}

	if (this->m_morphemeSystem->GetCharacterDef() == nullptr)
	{
		this->m_eventTrackEditor->Clear();

		m_eventTrackEditor->m_load = false;
		m_eventTrackEditor->m_selectedAnimIdx = -1;
	}

	if (this->m_eventTrackEditor->m_reload)
		this->m_eventTrackEditor->ReloadTracks();

	if (this->m_eventTrackEditor->m_load)
		LoadMorphemeMarkup(this);

	if (this->m_timeAct == nullptr)
	{
		this->m_timeActEditor->Clear();

		m_timeActEditor->m_load = false;
		m_timeActEditor->m_selectedTimeActIdx = -1;
	}

	if (this->m_timeActEditor->m_reload)
		this->m_timeActEditor->ReloadTracks();

	if (this->m_timeActEditor->m_load)
		LoadTimeActTracks(this);

	if (this->m_animPlayer)
	{
		FlverModel* model = this->m_animPlayer->GetModel();
		SetModelFlags(model, this->m_sceneFlags.m_displayMode, this->m_sceneFlags.m_drawDummies);

		model = this->m_animPlayer->GetModelPart(Parts_Head);
		SetModelFlags(model, this->m_sceneFlags.m_displayMode, this->m_sceneFlags.m_drawDummies);

		model = this->m_animPlayer->GetModelPart(Parts_Face);
		SetModelFlags(model, this->m_sceneFlags.m_displayMode, this->m_sceneFlags.m_drawDummies);

		model = this->m_animPlayer->GetModelPart(Parts_Body);
		SetModelFlags(model, this->m_sceneFlags.m_displayMode, this->m_sceneFlags.m_drawDummies);

		model = this->m_animPlayer->GetModelPart(Parts_Arm);
		SetModelFlags(model, this->m_sceneFlags.m_displayMode, this->m_sceneFlags.m_drawDummies);

		model = this->m_animPlayer->GetModelPart(Parts_Leg);
		SetModelFlags(model, this->m_sceneFlags.m_displayMode, this->m_sceneFlags.m_drawDummies);

		model = this->m_animPlayer->GetModelPart(Parts_WeaponLeft);
		SetModelFlags(model, this->m_sceneFlags.m_displayMode, this->m_sceneFlags.m_drawDummies);

		model = this->m_animPlayer->GetModelPart(Parts_WeaponRight);
		SetModelFlags(model, this->m_sceneFlags.m_displayMode, this->m_sceneFlags.m_drawDummies);

		model = this->m_animPlayer->GetModelPartFacegen(FaceGen_Face);
		SetModelFlags(model, this->m_sceneFlags.m_displayMode, this->m_sceneFlags.m_drawDummies);

		model = this->m_animPlayer->GetModelPartFacegen(FaceGen_Head);
		SetModelFlags(model, this->m_sceneFlags.m_displayMode, this->m_sceneFlags.m_drawDummies);

		model = this->m_animPlayer->GetModelPartFacegen(FaceGen_Eyes);
		SetModelFlags(model, this->m_sceneFlags.m_displayMode, this->m_sceneFlags.m_drawDummies);

		model = this->m_animPlayer->GetModelPartFacegen(FaceGen_EyeBrows);
		SetModelFlags(model, this->m_sceneFlags.m_displayMode, this->m_sceneFlags.m_drawDummies);

		model = this->m_animPlayer->GetModelPartFacegen(FaceGen_Beard);
		SetModelFlags(model, this->m_sceneFlags.m_displayMode, this->m_sceneFlags.m_drawDummies);

		model = this->m_animPlayer->GetModelPartFacegen(FaceGen_Hair);
		SetModelFlags(model, this->m_sceneFlags.m_displayMode, this->m_sceneFlags.m_drawDummies);
	}
}

std::wstring GetObjIdFromTaeFileName(std::wstring name)
{
	std::wstring obj_id;

	int lastCPos = name.find_last_of(L"\\");

	obj_id = name.substr(lastCPos + 1, 8);

	if (obj_id.substr(0, 1).compare(L"o") != 0)
		return L"";

	g_appLog->DebugMessage(MsgLevel_Debug, "Obj ID: %s\n", obj_id);

	return obj_id;
}

std::wstring FindGamePath(std::wstring current_path)
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

void Application::LoadFile()
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

						if (filepath.extension() == ".nmb")
						{
							g_appLog->DebugMessage(MsgLevel_Info, "Loading NMB file %ws\n", filepath.c_str());

							try
							{
								this->m_fileNameMapPairList->Clear();

								this->m_animPlayer->Clear();

								CharacterDef* characterDef = m_morphemeSystem->createCharacterDef(filepath.string().c_str());

								if (!characterDef)
									return;

								characterDef->loadAnimations();

								Character* character = Character::create(characterDef);

								if (!character)
									return;

								this->m_morphemeSystem->registerCharacter(character);

								if (m_morphemeSystem->GetCharacterDef()->isLoaded())
								{
									int animCount = m_morphemeSystem->GetCharacterDef()->getAnimFileLookUp()->getNumAnims();

									for (int i = 0; i < animCount; i++)
									{
										std::filesystem::path animFolder = pszFilePath;
										std::wstring parent_path = animFolder.parent_path();

										std::wstring anim_name = RString::ToWide(this->m_morphemeSystem->GetCharacterDef()->getAnimFileLookUp()->getFilename(i));

										std::wstring anim_path_str = parent_path + L"\\" + anim_name;

										characterDef->addAnimation(RString::ToNarrow(anim_path_str.c_str()).c_str());
									}

									characterDef->sortAnimations();

									this->m_eventTrackEditor->m_targetAnimIdx = -1;
									this->m_eventTrackEditor->m_selectedAnimIdx = -1;

									this->m_eventTrackEditor->m_edited.clear();
									this->m_eventTrackEditor->m_edited.reserve(m_morphemeSystem->GetCharacterDef()->getAnimFileLookUp()->getNumAnims());

									for (int i = 0; i < m_morphemeSystem->GetCharacterDef()->getAnimFileLookUp()->getNumAnims(); i++)
										this->m_eventTrackEditor->m_edited.push_back(false);

									if (characterDef->getCharacterId() != -1)
									{
										std::filesystem::path gamepath = FindGamePath(pszFilePath);
										this->m_gamePath = gamepath;

										this->m_fileNameMapPairList->Create(this->m_gamePath);

										std::filesystem::path filepath_tae = "";
										std::filesystem::path filepath_dcx = "";
										std::filesystem::path filepath_parts = "";

										if (gamepath.compare("") != 0)
										{
											filepath_tae = gamepath;
											filepath_dcx = gamepath;
											filepath_parts = gamepath;

											filepath_tae += "\\timeact\\chr";
											filepath_dcx += "\\model\\chr";
											filepath_parts += "\\model\\parts";

											wchar_t chr_id_str[50];
											swprintf_s(chr_id_str, L"%04d", characterDef->getCharacterId());

											std::wstring string = std::wstring(chr_id_str);

											this->m_eventTrackEditor->m_taeList = getTaeFileListFromChrId(filepath_tae, chr_id_str);
											this->m_eventTrackEditor->m_loadTae = true;

											std::wstring model_path = getModelNameFromChrId(filepath_dcx, chr_id_str);

											FlverModel* model = FlverModel::CreateFromBnd(model_path);

											if (model)
											{
												this->m_animPlayer->SetModel(model);
												this->m_animPlayer->GetModel()->CreateFlverToMorphemeBoneMap(this->m_morphemeSystem->GetCharacterDef()->getNetworkDef()->getRig(0));
											}

											//Load external parts for the player character
											if (characterDef->getCharacterId() == 1)
											{
												LoadPlayerModelParts(this, filepath_parts);
											}
										}
										else
											g_appLog->DebugMessage(MsgLevel_Debug, "Could not find Game folder\n");
									}
								}
							}
							catch (const std::exception& e)
							{
								g_appLog->PanicMessage(e.what());
							}
						}
						else if (filepath.extension() == ".tae")
						{
							g_appLog->DebugMessage(MsgLevel_Info, "Loading TimeAct file %ws\n", filepath.c_str());

							try
							{
								this->m_fileNameMapPairList->Clear();

								if (this->m_timeAct)
								{
									delete this->m_timeAct;
									this->m_timeAct = nullptr;
								}

								m_timeAct = new TimeActReader(pszFilePath);

								if (m_timeAct->m_init)
								{
									this->m_timeActEditor->m_selectedTimeActIdx = -1;

									this->m_timeActEditor->m_edited.clear();
									this->m_timeActEditor->m_edited.reserve(m_timeAct->m_tae.size());

									for (int i = 0; i < m_timeAct->m_tae.size(); i++)
										this->m_timeActEditor->m_edited.push_back(false);

									g_appLog->DebugMessage(MsgLevel_Debug, "Open file %ls (len=%d)\n", m_timeAct->m_filePath, m_timeAct->m_fileSize);

									std::wstring obj_id = GetObjIdFromTaeFileName(pszFilePath);

									if (obj_id.compare(L"") != 0)
									{
										std::filesystem::path gamepath = FindGamePath(pszFilePath);
										std::filesystem::path filepath_dcx;

										this->m_fileNameMapPairList->Create(this->m_gamePath);

										if (gamepath.compare("") != 0)
										{
											filepath_dcx = gamepath;

											filepath_dcx += "\\model\\obj";

											std::wstring obj_path = getModelNameFromObjId(filepath_dcx, obj_id);

											if (obj_path.compare(L"") != 0)
											{
												PWSTR dcx_path = (wchar_t*)obj_path.c_str();

												BNDReader bnd(dcx_path);

												std::string filename = RString::ToNarrow(obj_id.c_str()) + ".flv";

												bool found_model = false;

												for (size_t i = 0; i < bnd.m_fileCount; i++)
												{
													if (bnd.m_files[i].m_name == filename)
													{
														UMEM* umem = uopenMem(bnd.m_files[i].m_data, bnd.m_files[i].m_uncompressedSize);
														FLVER2 flver_model = FLVER2(umem);

														this->m_animPlayer->SetModel(new FlverModel(umem));

														g_appLog->DebugMessage(MsgLevel_Debug, "Loaded model %s\n", filename.c_str());

														found_model = true;

														break;
													}
												}
											}
										}
										else
											g_appLog->DebugMessage(MsgLevel_Debug, "Could not find Game folder\n");
									}
								}
							}
							catch (const std::exception& e)
							{
								g_appLog->PanicMessage(e.what());
							}
						}
					}
					pItem->Release();
				}
				else
					MessageBoxW(NULL, L"Failed to open file", L"Application.cpp", MB_ICONERROR);
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
}

void Application::SaveFile()
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
							/*
							bool status = m_nmb.SaveToFile(pszOutFilePath);

							if (status)
								g_appLog->DebugMessage(MsgLevel_Debug, "Save file %ls (bundles=%d, len=%d)\n", m_nmb.GetOutFilePath(), m_nmb.GetBundleCount(), m_nmb.GetOutFileSize());
							else
								g_appLog->AlertMessage(MsgLevel_Error, "NMBReader.cpp", "Failed to generate NMB file\n");
							*/
						}
						else if (filepath.extension() == ".tae")
						{
							bool status = m_timeAct->SaveFile(pszOutFilePath);

							if (status)
								g_appLog->DebugMessage(MsgLevel_Debug, "Save file %ls (taeCount=%d)\n", m_timeAct->m_outFilePath, m_timeAct->m_header.m_taeCount);
							else
								g_appLog->AlertMessage(MsgLevel_Error, "Failed to generate TAE file\n");
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

void Application::SaveNmbFile()
{
	COMDLG_FILTERSPEC ComDlgFS[] = { {L"Morpheme Network Binary", L"*.nmb"}, {L"All Files",L"*.*"} };

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
			pFileSave->SetFileTypes(2, ComDlgFS);

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
						/*
						std::filesystem::path filepath = std::wstring(pszOutFilePath);

						bool status = m_nmb.SaveToFile(pszOutFilePath);

						if (status)
							g_appLog->DebugMessage(MsgLevel_Debug, "Save file %ls (bundles=%d, len=%d)\n", m_nmb.GetOutFilePath(), m_nmb.GetBundleCount(), m_nmb.GetOutFileSize());
						else
							g_appLog->AlertMessage(MsgLevel_Error, "NMBReader.cpp", "Failed to generate NMB file\n");
						*/
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

void Application::SaveTaeFile()
{
	COMDLG_FILTERSPEC ComDlgFS[] = { {L"TimeAct", L"*.tae"}, {L"All Files",L"*.*"} };

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
			pFileSave->SetFileTypes(2, ComDlgFS);

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

						bool status = m_timeAct->SaveFile(pszOutFilePath);

						if (status)
							g_appLog->DebugMessage(MsgLevel_Debug, "Save file %ls (taeCount=%d)\n", m_timeAct->m_outFilePath, m_timeAct->m_header.m_taeCount);
						else
							g_appLog->AlertMessage(MsgLevel_Error, "Failed to generate TAE file\n");
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

void Application::SetTimeActCurrentFrameFromEventTrack(int* current_frame_tae, int current_frame)
{
	if (this->m_timeActEditor->m_source == nullptr)
		return;

	if (this->m_eventTrackEditor->GetTrackCount() > 0)
	{
		for (int i = 0; i < this->m_eventTrackEditor->GetTrackCount(); i++)
		{
			if (this->m_eventTrackEditor->m_eventTracks[i].m_eventId == 1000)
			{
				this->m_eventTrackEditor->m_eventTrackActionTimeActStart = RMath::FrameToTime(this->m_eventTrackEditor->m_eventTracks[i].m_event[0].m_frameStart);
				this->m_eventTrackEditor->m_eventTrackActionTimeActDuration = RMath::FrameToTime(this->m_eventTrackEditor->m_eventTracks[i].m_event[0].m_duration);
				this->m_eventTrackEditor->m_eventTrackActionTimeActValue = this->m_eventTrackEditor->m_eventTracks[i].m_event[0].m_value;

				break;
			}
		}

		if (this->m_eventTrackEditor->m_eventTrackActionTimeActValue == this->m_timeActEditor->m_taeId)
			*current_frame_tae = RMath::TimeToFrame(m_timeActEditor->m_source->CalculatePlaybackPosFromMorphemeEventTrack(this->m_eventTrackEditor->m_eventTrackActionTimeActStart, this->m_eventTrackEditor->m_eventTrackActionTimeActDuration, RMath::FrameToTime(current_frame)), 30, false);
	}
}

bool Application::ExportAnimationToFbx(std::filesystem::path export_path, int anim_id)
{
	bool status = true;

	CharacterDef* characterDef = this->m_morphemeSystem->GetCharacterDef();

	g_appLog->DebugMessage(MsgLevel_Debug, "Exporting animation %d (%s)\n", anim_id, export_path.filename().string().c_str());

	FbxExporter* pExporter = FbxExporter::Create(g_pFbxManager, "");
	pExporter->SetFileExportVersion(FBX_2014_00_COMPATIBLE);

	if (!pExporter->Initialize(export_path.string().c_str(), g_pFbxManager->GetIOPluginRegistry()->GetNativeWriterFormat(), g_pFbxManager->GetIOSettings()))
	{
		g_appLog->PanicMessage("Failed to initialise FBX exporter (file=%s)\n", export_path.string().c_str());

		return false;
	}

	FbxScene* pScene = FbxScene::Create(g_pFbxManager, export_path.string().c_str());
	pScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::eMax);
	pScene->GetGlobalSettings().SetSystemUnit(FbxSystemUnit::m);

	FbxPose* pBindPoses = FbxPose::Create(pScene, "BindPoses");
	pBindPoses->SetIsBindPose(true);

	std::vector<FbxNode*> pMorphemeRig = FBXTranslator::CreateFbxMorphemeSkeleton(pScene, characterDef->getNetworkDef()->getRig(0), pBindPoses);

	if (!FBXTranslator::CreateFbxTake(pScene, pMorphemeRig, characterDef->getAnimationById(anim_id), characterDef->getAnimFileLookUp()->getTakeName(anim_id)))
	{
		g_appLog->DebugMessage(MsgLevel_Error, "Failed to create FBX anim take (animId=%d, chrId=c%04d)\n", anim_id, characterDef->getCharacterId());
		status = false;
	}

	pScene->AddPose(pBindPoses);

	pExporter->Export(pScene);
	pScene->Destroy(true);
	pExporter->Destroy(true);

	return status;
}

bool Application::ExportModelToFbx(std::filesystem::path export_path)
{
	bool status = true;

	CharacterDef* characterDef = this->m_morphemeSystem->GetCharacterDef();

	if (characterDef == nullptr)
		return false;

	g_appLog->DebugMessage(MsgLevel_Debug, "Exporting model to FBX for c%04d\n", characterDef->getCharacterId());

	FbxExporter* pExporter = FbxExporter::Create(g_pFbxManager, "");
	pExporter->SetFileExportVersion(FBX_2014_00_COMPATIBLE);

	char chr_id_str[20];
	sprintf_s(chr_id_str, "c%04d", characterDef->getCharacterId());

	std::string model_out = export_path.string() + std::string(chr_id_str) + ".fbx";

	if (!pExporter->Initialize(model_out.c_str(), g_pFbxManager->GetIOPluginRegistry()->GetNativeWriterFormat(), g_pFbxManager->GetIOSettings()))
	{
		g_appLog->PanicMessage("Failed to initialise FBX exporter (file=%s)\n", model_out);

		return false;
	}

	FbxScene* pScene = FbxScene::Create(g_pFbxManager, chr_id_str);
	pScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::eMax);
	pScene->GetGlobalSettings().SetSystemUnit(FbxSystemUnit::m);

	FbxPose* pBindPoses = FbxPose::Create(pScene, "BindPoses");
	pBindPoses->SetIsBindPose(true);

	std::vector<FbxNode*> pMorphemeRig = FBXTranslator::CreateFbxMorphemeSkeleton(pScene, characterDef->getNetworkDef()->getRig(0), pBindPoses);

	if (!FBXTranslator::CreateFbxModel(pScene, this->m_animPlayer->GetModel(), characterDef->getCharacterId(), pBindPoses, pMorphemeRig, model_out, this->m_animPlayer->GetModel()->GetFlverToMorphemeBoneMap()))
	{
		g_appLog->DebugMessage(MsgLevel_Error, "Failed to create FBX model/skeleton (chrId=c%04d)\n", characterDef->getCharacterId());

		status = false;
	}

	pScene->AddPose(pBindPoses);

	pExporter->Export(pScene);
	pScene->Destroy(true);
	pExporter->Destroy(true);

	return status;
}