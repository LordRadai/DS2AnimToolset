#include "GuiManager.h"

#include "framework.h"
#include "extern.h"
#include <IconsFontAwesome6.h>
#include "RenderManager/RenderManager.h"
#include "MorphemeEditorApp/MorphemeEditorApp.h"
#include "WorkerThread/WorkerThread.h"
#include "Camera/Camera.h"

#define MSAA_SETTING_COUNT 4

namespace
{
	void trackEditorColorSelector(TrackEditor::TrackEditorBase* trackEditor)
	{
		ImGui::ColorEdit4("Track", (float*)&trackEditor->getColors()->m_trackColor);
		ImGui::ColorEdit4("Track Inactive", (float*)&trackEditor->getColors()->m_trackColorInactive);
		ImGui::ColorEdit4("Track Active", (float*)&trackEditor->getColors()->m_trackColorActive);
		ImGui::ColorEdit4("Track Bounding Box", (float*)&trackEditor->getColors()->m_trackBoundingBox);
		ImGui::ColorEdit4("Track Bounding Box Active", (float*)&trackEditor->getColors()->m_trackBoundingBoxActive);
		ImGui::ColorEdit4("Highlight", (float*)&trackEditor->getColors()->m_highlight);
		ImGui::ColorEdit4("Track Text Color", (float*)&trackEditor->getColors()->m_trackTextColor);
		ImGui::ColorEdit4("Cursor Color", (float*)&trackEditor->getColors()->m_cursorColor);
	}

	TimeAct::TaeExport::TimeActTrackExportXML* getTimeActTrackForAnimation(AnimObject* animObj)
	{
		MorphemeEditorApp* editorApp = MorphemeEditorApp::getInstance();
		Character* character = editorApp->getCharacter();

		if (character == nullptr || animObj->getTakeList() == nullptr)
			return nullptr;

		int timeActTrackId = -1;
		ME::TakeExport* take = animObj->getTakeList()->getTake(0);

		for (size_t i = 0; i < take->getNumEventTracks(); i++)
		{
			ME::EventTrackExport* eventTrack = take->getEventTrack(i);
			int trackId = eventTrack->getUserData();

			if ((trackId == 1000) && (eventTrack->getEventTrackType() == ME::EventTrackExport::EVENT_TRACK_TYPE_DURATION))
			{
				int timeActId = static_cast<ME::DurationEventTrackExport*>(eventTrack)->getEvent(0)->getUserData();

				TimeAct::TaeExport::TimeActExportXML* timeAct = character->getTimeAct();

				if (timeAct != nullptr)
					return timeAct->getTrackList()->timeActTrackLookup(timeActId);
			}
		}

		return nullptr;
	}

	void skeletonInspectorTreeNode(FlverModel* model, int boneID, int& selected_id)
	{
		if (boneID == -1)
			return;

		cfr::FLVER2::Bone bone = model->getFlverBone(boneID);

		int childIndex = bone.childIndex;

		if (childIndex == -1)
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

			if (selected_id == boneID)
				node_flags |= ImGuiTreeNodeFlags_Selected;

			ImGui::TreeNodeEx(RString::toNarrow(bone.name).c_str(), node_flags);

			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				selected_id = boneID;
		}
		else
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

			if (selected_id == boneID)
				node_flags |= ImGuiTreeNodeFlags_Selected;

			bool open = ImGui::TreeNodeEx(RString::toNarrow(bone.name).c_str(), node_flags);

			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				selected_id = boneID;

			if (open)
			{
				int siblingIndex = childIndex;

				while (siblingIndex != -1)
				{
					skeletonInspectorTreeNode(model, siblingIndex, selected_id);

					siblingIndex = model->getFlverBone(siblingIndex).nextSiblingIndex;
				}

				ImGui::TreePop();
			}
		}
	}

	void modelTreeNode(MorphemeEditorApp* editorApp, FlverModel* model)
	{
		if (model == nullptr)
			return;

		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

		if (editorApp->getPreviewFlags()->selectedModel == model)
			node_flags |= ImGuiTreeNodeFlags_Selected;

		bool open = ImGui::TreeNodeEx(model->getModelName().c_str(), node_flags);

		if (open)
		{
			if (model->getNumFlverBones() > 0)
			{
				if (ImGui::TreeNode("Bones"))
				{
					ImGui::BeginChild("skeleton_inspector", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetWindowSize().y / 3));

					int boneID = 0;

					while (boneID != -1)
					{
						skeletonInspectorTreeNode(model, boneID, model->getSettings()->selectedBone);

						boneID = model->getFlverBone(boneID).nextSiblingIndex;
					}

					ImGui::EndChild();

					ImGui::TreePop();
				}
			}

			if (model->getNumFlverDummies() > 0)
			{
				if (ImGui::TreeNode("Dummy Polygons"))
				{
					ImGui::BeginChild("dummy_inspector", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetWindowSize().y / 3));

					for (size_t i = 0; i < model->getNumFlverDummies(); i++)
					{
						std::string dummy_name = "Dmy_" + std::to_string(model->getFlverDummy(i).referenceID);

						bool selected = (model->getSettings()->selectedDummy == i);

						if (ImGui::Selectable(dummy_name.c_str(), selected))
							model->getSettings()->selectedDummy = i;
					}

					ImGui::EndChild();

					ImGui::TreePop();
				}

			}

			if (ImGui::IsMouseDoubleClicked(0))
			{
				model->getSettings()->selectedDummy = -1;
				model->getSettings()->selectedBone = -1;
			}

			ImGui::TreePop();
		}

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		{
			if (editorApp->getPreviewFlags()->selectedModel != nullptr)
				editorApp->getPreviewFlags()->selectedModel->getSettings()->highlight = false;

			editorApp->getPreviewFlags()->selectedModel = model;
			editorApp->getPreviewFlags()->selectedModel->getSettings()->highlight = true;
		}

		if (ImGui::IsMouseDoubleClicked(0))
		{
			if (editorApp->getPreviewFlags()->selectedModel != nullptr)
				editorApp->getPreviewFlags()->selectedModel->getSettings()->highlight = false;

			editorApp->getPreviewFlags()->selectedModel = nullptr;
		}
	}

	std::string getTimeActGroupTooltip(int groupId)
	{
		RINI* ini = RINI::open("Data\\res\\tooltip\\timeact\\group.ini");

		char defaultInfo[255];
		sprintf_s(defaultInfo, "No specific information is known for this event group\n");

		if (ini == nullptr)
		{
			g_appLog->debugMessage(MsgLevel_Debug, "Failed to load group.ini\n");
			return std::string(defaultInfo);
		}

		std::string categoryStr = std::to_string(groupId);

		std::string tooltip = ini->getString(categoryStr, "tooltip", defaultInfo);

		if (tooltip.compare("") == 0)
			return defaultInfo;

		return tooltip;
	}

	std::string getTimeActEventTooltip(int eventId)
	{
		RINI* ini = RINI::open("Data\\res\\tooltip\\timeact\\event.ini");

		char defaultInfo[255];
		sprintf_s(defaultInfo, "No specific information is known for this event\n");

		if (ini == nullptr)
		{
			g_appLog->debugMessage(MsgLevel_Debug, "Failed to load event.ini\n");
			return std::string(defaultInfo);
		}

		std::string categoryStr = std::to_string(eventId);

		std::string tooltip = ini->getString(categoryStr, "tooltip", defaultInfo);

		if (tooltip.compare("") == 0)
			return defaultInfo;

		return tooltip;
	}

	std::string getEventTrackGroupTooltip(int groupId)
	{
		RINI* ini = RINI::open("Data\\res\\tooltip\\eventrack\\group.ini");

		char defaultInfo[255];
		sprintf_s(defaultInfo, "No specific information is known for this track type\n");

		if (ini == nullptr)
		{
			g_appLog->debugMessage(MsgLevel_Debug, "Failed to load group.ini\n");
			return std::string(defaultInfo);
		}

		std::string categoryStr = std::to_string(groupId);

		std::string tooltip = ini->getString(categoryStr, "tooltip", defaultInfo);

		if (tooltip.compare("") == 0)
			return defaultInfo;

		return tooltip;
	}

	std::string getEventTrackEventTooltip(int eventId)
	{
		RINI* ini = RINI::open("Data\\res\\tooltip\\eventrack\\event.ini");

		char defaultInfo[255];
		sprintf_s(defaultInfo, "No specific information is known for this event\n");

		if (ini == nullptr)
		{
			g_appLog->debugMessage(MsgLevel_Debug, "Failed to load event.ini\n");
			return std::string(defaultInfo);
		}

		std::string categoryStr = std::to_string(eventId);

		std::string tooltip = ini->getString(categoryStr, "tooltip", defaultInfo);

		if (tooltip.compare("") == 0)
			return defaultInfo;

		return tooltip;
	}

	void modelPartsSelectorGUI(const FileIDNamesTable list, bool female, Character* character, PartType type)
	{
		ImGuiTextFilter filter;

		if (female)
			strcpy(filter.InputBuf, "_f, None");
		else
			strcpy(filter.InputBuf, "_m, None");

		filter.Build();

		std::string childName = "resource_list##" + std::to_string(type);
		if (ImGui::BeginChild(childName.c_str()))
		{
			for (size_t i = 0; i < list.getNumEntries(); i++)
			{
				std::wstring path = list.getPath(i);
				std::string name = std::filesystem::path(path).filename().string();

				if (filter.PassFilter(name.c_str()))
				{
					bool selected = false;

					if (character->getCharacterModelCtrl()->getModelPart(type) != nullptr)
						selected = (character->getCharacterModelCtrl()->getModelPart(type)->getModelName().compare(RString::removeExtension(name)) == 0);
					else if (i == 0)
						selected = true;

					ImGui::Selectable(name.c_str(), selected);

					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
					{
						if (path.compare(L"") == 0)
							character->getCharacterModelCtrl()->setModelPart(type, nullptr);
						else
						{
							FlverModel* model = FlverModel::createFromBnd(list.getPath(i).c_str(), character->getRig(0));

							character->getCharacterModelCtrl()->setModelPart(type, model);
						}
					}
				}
			}

			ImGui::EndChild();
		}
	}

	void modelPartsSelectorGUI(const FileIDNamesTable list, Character* character, PartType type)
	{
		std::string childName = "resource_list##" + std::to_string(type);
		if (ImGui::BeginChild(childName.c_str())) 
		{
			for (size_t i = 0; i < list.getNumEntries(); i++)
			{
				std::wstring path = list.getPath(i);
				std::string name = std::filesystem::path(path).filename().string();
				bool selected = false;

				if (character->getCharacterModelCtrl()->getModelPart(type) != nullptr)
					selected = (character->getCharacterModelCtrl()->getModelPart(type)->getModelName().compare(RString::removeExtension(name)) == 0);
				else if (i == 0)
					selected = true;

				ImGui::Selectable(name.c_str(), selected);

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
				{
					if (path.compare(L"") == 0)
						character->getCharacterModelCtrl()->setModelPart(type, nullptr);
					else
					{
						FlverModel* model = FlverModel::createFromBnd(list.getPath(i).c_str(), character->getRig(0));

						character->getCharacterModelCtrl()->setModelPart(type, model);
					}
				}
			}

			ImGui::EndChild();
		}
	}

	void modelFgSelectorGUI(const FileIDNamesTable list, bool female, Character* character, FgPartType type)
	{
		std::string childName = "resource_list_fg##" + std::to_string(type);
		if (ImGui::BeginChild(childName.c_str()))
		{
			for (size_t i = 0; i < list.getNumEntries(); i++)
			{
				std::wstring path = list.getPath(i);
				std::string name = std::filesystem::path(path).filename().string();
				bool selected = false;

				if (character->getCharacterModelCtrl()->getModelFg(type) != nullptr)
					selected = (character->getCharacterModelCtrl()->getModelFg(type)->getModelName().compare(RString::removeExtension(name)) == 0);
				else if (i == 0)
					selected = true;

				ImGui::Selectable(name.c_str(), selected);

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
				{
					if (path.compare(L"") == 0)
						character->getCharacterModelCtrl()->setModelFg(type, nullptr);
					else
					{
						FlverModel* model = FlverModel::createFromBnd(list.getPath(i).c_str(), character->getRig(0));

						character->getCharacterModelCtrl()->setModelFg(type, model);
					}
				}
			}

			ImGui::EndChild();
		}
	}
}

GuiManager::GuiManager()
{
}

GuiManager::~GuiManager()
{
}

GuiManager* GuiManager::getInstance()
{
	if (_instance == nullptr)
		_instance = new GuiManager();

	return _instance;
}

void GuiManager::initialise(HWND hwnd, ID3D11DeviceContext* pContext, ID3D11Device* pDevice)
{
	this->m_window = hwnd;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Scene backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(pDevice, pContext);

	g_appLog->debugMessage(MsgLevel_Info, "Add ImGui fonts\n");
	io.Fonts->AddFontDefault();

	float baseFontSize = 13.0f; // 13.0f is the size of the default font. Change to the font size you use.
	float iconFontSize = baseFontSize * 2.0f / 3.0f; // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly

	// merge in icons from Font Awesome
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMinAdvanceX = iconFontSize;
	io.Fonts->AddFontFromFileTTF("Data//font//" FONT_ICON_FILE_NAME_FAS, iconFontSize, &icons_config, icons_ranges);
	// use FONT_ICON_FILE_NAME_FAR if you want regular instead of solid

	this->initGuiStyle();

	this->m_initialised = true;
}

void GuiManager::initGuiStyle()
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
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
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
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_TabSelected] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_TabDimmed] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
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
	colors[ImGuiCol_TextLink] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void GuiManager::update(float dt)
{
	if (!this->m_initialised)
		throw("Called update() without calling initialise() first\n");

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	this->rootWindow();
	this->assetsWindow();
	this->modelViewerWindow();
	this->eventTrackEditorWindow();
	this->timeActEditorWindow();
	this->eventTrackInfoWindow();
	this->timeActInfoWindow();

	MorphemeEditorApp* editorApp = MorphemeEditorApp::getInstance();
	MorphemeEditorApp::WindowFlags* windowFlags = editorApp->getWindowFlags();

	if (GetForegroundWindow() == this->m_window)
	{
		if (editorApp->getCharacter() != nullptr && editorApp->getCharacter()->getTimeAct() != nullptr)
		{
			if (RInput::isKeyComboPressed(VK_CONTROL, 0x46))	//Ctrl+F
				windowFlags->searchQuery = true;
		}
	}

	if (windowFlags->imGuiDemo)
		ImGui::ShowDemoWindow(&windowFlags->imGuiDemo);

	if (windowFlags->sceneExplorer)
		this->sceneExplorerWindow();

	if (windowFlags->playerPartsManager)
		this->partsManagerWindow();

	if (windowFlags->styleEditor)
		this->colorSettingsWindow();

	if (windowFlags->graphicsSettings)
		this->graphicsSettingsWindow();

	if (windowFlags->searchQuery)
		this->searchQueryWindow();
}

void GuiManager::render(ID3D11DeviceContext* pContext, ID3D11RenderTargetView* pRenderTargetView)
{
	pContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);
	pContext->ClearRenderTargetView(pRenderTargetView, DirectX::Colors::CornflowerBlue);

	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void GuiManager::shutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void GuiManager::rootWindow()
{
	MorphemeEditorApp* editorApp = MorphemeEditorApp::getInstance();

	ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y));
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::Begin("AppRootWindow", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImGui::PopStyleVar(1);

	ImGuiID dockSpace = ImGui::GetID("MainWindowDockspace");
	ImGui::DockSpace(dockSpace, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	ImGui::BeginMenuBar();

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Open...")) { editorApp->getTaskFlags()->loadFile = true; }
		//if (ImGui::MenuItem("Save...")) { editorApp->getTaskFlags()->saveFile = true; }

		ImGui::Separator();
		
		if (ImGui::BeginMenu("Export"))
		{
			ImGui::BeginDisabled(editorApp->getCharacter() == nullptr);

			if (ImGui::MenuItem("Export All")) { editorApp->getTaskFlags()->exportAll = true; }
			if (ImGui::MenuItem("Export Model")) { editorApp->getTaskFlags()->exportModel = true; }
			if (ImGui::MenuItem("Export Animations")) { editorApp->getTaskFlags()->exportAnimations = true; }
			if (ImGui::MenuItem("Export Network")) { editorApp->getTaskFlags()->exportNetwork = true; }
			if (ImGui::MenuItem("Export TimeAct")) { editorApp->getTaskFlags()->exportTae = true; }

			ImGui::EndDisabled();

			ImGui::Separator();

			int currentItem = editorApp->getTaskFlags()->exportFormat;

			const char* items[] = { "FBX", "XMD" };

			if (ImGui::BeginCombo("Format", items[editorApp->getTaskFlags()->exportFormat]))
			{
				for (size_t i = 0; i < MorphemeEditorApp::kNumExportFormats; i++)
				{
					const bool selected = (editorApp->getTaskFlags()->exportFormat == i);

					if (ImGui::Selectable(items[i], selected))
						editorApp->getTaskFlags()->exportFormat = (MorphemeEditorApp::ExportFormat)i;

					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Edit"))
	{
		ImGui::BeginDisabled(editorApp->getCharacter() == nullptr || editorApp->getCharacter()->getTimeAct() == nullptr);
		if (ImGui::MenuItem("Find Value...", "CTRL+F")) { editorApp->getWindowFlags()->searchQuery = true; }

		ImGui::EndDisabled();

		ImGui::Separator();

		if (ImGui::MenuItem("Colors")) { editorApp->getWindowFlags()->styleEditor = true; }
		
		if (ImGui::BeginMenu("Graphics"))
		{
			RenderManager* renderMan = RenderManager::getInstance();

			const char* msaaQualitySettings[5] = { "Off", "2x", "4x", "8x", "16x" };
			int selectedSetting = 0;

			int msaaCountSetting = renderMan->getSettings()->msaaCount;

			switch (msaaCountSetting)
			{
			case 1:
				selectedSetting = 0;
				break;
			case 2:
				selectedSetting = 1;
				break;
			case 4:
				selectedSetting = 2;
				break;
			case 8:
				selectedSetting = 3;
				break;
			case 16:
				selectedSetting = 4;
				break;
			default:
				g_appLog->panicMessage("Unsupported MSAA count %d (maximum handled is 16, recommended maximum is 8)\n", msaaCountSetting);
				break;
			}

			if (ImGui::BeginCombo("MSAA", msaaQualitySettings[selectedSetting]))
			{
				for (size_t i = 0; i < MSAA_SETTING_COUNT; i++)
				{
					ImGui::Selectable(msaaQualitySettings[i]);

					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
					{
						const int msaaCount = 1 << i;

						renderMan->getSettings()->msaaCount = msaaCount;
					}
				}

				ImGui::EndCombo();
			}

			ImGui::EndMenu();
		}
		
		ImGui::Separator();

		if (ImGui::BeginMenu("Log Level"))
		{
			if (ImGui::MenuItem("Info", nullptr, g_logLevel == MsgLevel_Info)) { g_logLevel = MsgLevel_Info; }
			if (ImGui::MenuItem("Warn", nullptr, g_logLevel == MsgLevel_Warn)) { g_logLevel = MsgLevel_Warn; }
			if (ImGui::MenuItem("Error", nullptr, g_logLevel == MsgLevel_Error)) { g_logLevel = MsgLevel_Error; }
			if (ImGui::MenuItem("Debug", nullptr, g_logLevel == MsgLevel_Debug)) { g_logLevel = MsgLevel_Debug; }

			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}

#ifdef _DEBUG
	if (ImGui::BeginMenu("Debug"))
	{
		if (ImGui::MenuItem("ImGui Demo", nullptr, editorApp->getWindowFlags()->imGuiDemo)) { editorApp->getWindowFlags()->imGuiDemo = !editorApp->getWindowFlags()->imGuiDemo; }
		if (ImGui::MenuItem("Create Tae Template XML")) { editorApp->getTaskFlags()->exportTaeTemplateXml = true; }

		ImGui::EndMenu();
	}
#endif

	ImGui::EndMenuBar();

	this->progressIndicatorPopup();

	ImGui::End();
}

void GuiManager::modelViewerWindow()
{
	MorphemeEditorApp* editorApp = MorphemeEditorApp::getInstance();

	Camera* camera = editorApp->getCamera();

	ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_Appearing);

	ImGui::Begin("Model Viewer", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar);

	MorphemeEditorApp::WindowFlags* windowStates = editorApp->getWindowFlags();
	MorphemeEditorApp::PreviewFlags* previewFlags = editorApp->getPreviewFlags();
	MorphemeEditorApp::TaskFlags* taskFlags = editorApp->getTaskFlags();

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Reset Camera")) { taskFlags->resetCamera = true; }

			ImGui::SeparatorText("Display Mode");

			if (ImGui::MenuItem("Normal", nullptr, previewFlags->displayMode == kDispNormal)) { previewFlags->displayMode = kDispNormal; }
			if (ImGui::MenuItem("X-Ray", nullptr, previewFlags->displayMode == kDispXRay)) { previewFlags->displayMode = kDispXRay; }
			if (ImGui::MenuItem("Wireframe", nullptr, previewFlags->displayMode == kDispWireframe)) { previewFlags->displayMode = kDispWireframe; }

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Model"))
		{
			if (ImGui::MenuItem("Draw Bones", nullptr, previewFlags->drawBones)) { previewFlags->drawBones = !previewFlags->drawBones; }
			if (ImGui::MenuItem("Draw Dummies", nullptr, previewFlags->drawDummies)) { previewFlags->drawDummies = !previewFlags->drawDummies; }

#ifdef _DEBUG
			if (ImGui::MenuItem("Draw Bounding Boxes", nullptr, previewFlags->drawBoundingBoxes)) { previewFlags->drawBoundingBoxes = !previewFlags->drawBoundingBoxes; }
#endif

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			bool disabled_explorer = true;

			Character* character = editorApp->getCharacter();

			if (character)
				disabled_explorer = false;

			ImGui::BeginDisabled(disabled_explorer);

			if (ImGui::MenuItem("Scene Explorer", nullptr, windowStates->sceneExplorer)) { windowStates->sceneExplorer = !windowStates->sceneExplorer; }

			ImGui::EndDisabled();

			bool disabled_parts = true;

			if (character && character->getCharacterId() == 1)
				disabled_parts = false;

			ImGui::BeginDisabled(disabled_parts);

			if (ImGui::MenuItem("Player Parts", nullptr, windowStates->playerPartsManager)) { windowStates->playerPartsManager = !windowStates->playerPartsManager; }

			ImGui::EndDisabled();

			ImGui::EndMenu();
		}

		ImGui::Separator();

		AnimPlayer* animPlayer = editorApp->getAnimPlayer();

		TrackEditor::EventTrackEditor* eventTrackEditor = editorApp->getEventTrackEditor();
		TrackEditor::TimeActEditor* timeActEditor = editorApp->getTimeActEditor();

		if (ImGui::Button(ICON_FA_BACKWARD_FAST))
		{
			animPlayer->setTime(RMath::frameToTime(eventTrackEditor->getClipStart()));
			eventTrackEditor->setCurrentTime(animPlayer->getTime());
			timeActEditor->setCurrentTime(animPlayer->getTime());
		}

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_BACKWARD_STEP))
		{
			animPlayer->stepPlay(-1.f / 30.f);
			eventTrackEditor->setCurrentTime(animPlayer->getTime());
			timeActEditor->setCurrentTime(animPlayer->getTime());
		}

		ImGui::SameLine();

		if (!animPlayer->isPaused())
		{
			if (ImGui::Button(ICON_FA_PAUSE))
				animPlayer->setPause(true);
		}
		else
		{
			if (ImGui::Button(ICON_FA_PLAY))
				animPlayer->setPause(false);
		}

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_FORWARD_STEP))
		{
			animPlayer->stepPlay(1.f / 30.f);
			eventTrackEditor->setCurrentTime(animPlayer->getTime());
			timeActEditor->setCurrentTime(animPlayer->getTime());
		}

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_FORWARD_FAST))
		{
			animPlayer->setTime(RMath::timeToFrame(eventTrackEditor->getClipEnd()));
			eventTrackEditor->setCurrentTime(animPlayer->getTime());
			timeActEditor->setCurrentTime(animPlayer->getTime());
		}

		ImGui::Separator();

		ImGui::EndMenuBar();
	}

	ImVec2 pos = ImGui::GetWindowPos();

	int	width = ImGui::GetWindowSize().x;
	int	height = ImGui::GetWindowSize().y;

	ImGui::InvisibleButton("viewport_preview", ImVec2(width, height));

	if (ImGui::IsItemFocused() && ImGui::IsItemHovered())
	{
		camera->setInputEnabled(true);

		if (ImGui::IsMouseDown(0))
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

		if (ImGui::IsMouseDown(1))
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
	}

	camera->setResolution(width, height);
	RenderManager::getInstance()->setResolution(width, height);

	ImGui::GetWindowDrawList()->AddImage(RenderManager::getInstance()->getShaderResourceViewport(), pos, ImVec2(pos.x + width, pos.y + height));

	ImGui::End();
}

void GuiManager::assetsWindow()
{
	MorphemeEditorApp* editorApp = MorphemeEditorApp::getInstance();
	Character* character = editorApp->getCharacter();

	if (character)
	{
		ImGui::SetNextWindowPos(ImGui::GetWindowPos() + ImVec2(2, 2), ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Appearing);

		if (ImGui::BeginPopupModal("Select TimeAct File"))
		{
			static std::wstring selectedFile;

			if (ImGui::Button("Load"))
			{
				character->loadTimeAct(RString::toNarrow(selectedFile).c_str());

				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel") || RInput::isKeyStateChanged(VK_ESCAPE))
				ImGui::CloseCurrentPopup();

			ImGui::BeginChild("file_list");

			std::vector<std::wstring> fileList = editorApp->getTimeActFileList();

			for (size_t i = 0; i < fileList.size(); i++)
			{
				ImGui::PushID(i);

				ImGui::Selectable(RString::toNarrow(std::filesystem::path(fileList[i]).filename()).c_str(), selectedFile == fileList[i]);

				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
					selectedFile = fileList[i];

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
				{
					selectedFile = fileList[i];
					
					character->loadTimeAct(RString::toNarrow(selectedFile).c_str());

					ImGui::CloseCurrentPopup();
				}

				ImGui::PopID();
			}

			ImGui::EndChild();

			ImGui::EndPopup();
		}
	}

	ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_Appearing);

	ImGui::Begin("Assets");
	ImGui::BeginTabBar("assets");
	
	if (character)
	{
		if (ImGui::BeginTabItem("Animations"))
		{
			AnimPlayer* animPlayer = MorphemeEditorApp::getInstance()->getAnimPlayer();

			if (character)
			{
				MorphemeCharacter* morphemeCharacter = character->getMorphemeCharacter();

				if (morphemeCharacter)
				{
					MorphemeCharacterDef* characterDef = morphemeCharacter->getCharacterDef();

					if (characterDef->isLoaded())
						ImGui::Text(characterDef->getFilename());

					static ImGuiTextFilter filter;
					ImGui::Text("Filter:");
					filter.Draw("##asset searchbar", 340.f);

					if (characterDef->isLoaded())
					{
						TrackEditor::EventTrackEditor* eventTrackEditor = editorApp->getEventTrackEditor();
						TrackEditor::TimeActEditor* timeActEditor = editorApp->getTimeActEditor();

						ImGui::BeginChild("anim_list");
						{
							int numAnims = characterDef->getAnimFileLookUp()->getNumAnims();

							for (int i = 0; i < numAnims; i++)
							{
								std::string anim_name = "";

								//if (eventTrackEditor->isEdited())
									//anim_name += "*";

								AnimObject* currentAnim = characterDef->getAnimation(i);

								anim_name += RString::removeExtension(characterDef->getAnimFileLookUp()->getSourceFilename(currentAnim->getAnimID()));

								bool selected = false;

								if (animPlayer->getAnim())
									selected = (animPlayer->getAnim()->getAnimID() == currentAnim->getAnimID());

								bool loaded = currentAnim->isLoaded();

								if (!loaded)
									ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));

								if (filter.PassFilter(anim_name.c_str()))
								{
									ImGui::PushID(i);
									ImGui::Selectable(anim_name.c_str(), &selected);

									if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
									{
										timeActEditor->reset();

										animPlayer->setAnim(currentAnim);
										eventTrackEditor->setEventTrack(currentAnim);
										timeActEditor->setTimeAct(getTimeActTrackForAnimation(currentAnim));
									}

									ImGui::PopID();
								}

								if (!loaded)
									ImGui::PopStyleColor();
							}
						}

						ImGui::EndChild();
					}
				}
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("TimeAct"))
		{
			static int selectedTrackIdx = -1;

			TimeAct::TaeExport::TimeActExportXML* tae = character->getTimeAct();

			TrackEditor::TimeActEditor* timeActEditor = MorphemeEditorApp::getInstance()->getTimeActEditor();

			if (tae == nullptr)
				return;

			ImGui::Text(tae->getName().c_str());

			if (ImGui::Button("Add TimeAct"))
				ImGui::OpenPopup("Add TimeAct");

			if ((selectedTrackIdx != -1) && RInput::isKeyStateChanged(VK_DELETE))
				ImGui::OpenPopup("Delete TimeAct");

			if (ImGui::BeginPopup("Add TimeAct"))
			{
				static float lenght = 0.f;
				static int id = 0;

				ImGui::InputInt("ID", &id);
				ImGui::InputFloat("Lenght", &lenght, 1.f / 30.f);

				if (ImGui::Button("Add"))
				{
					tae->getTrackList()->addTrack(id, RMath::timeToFrame(lenght, 30), 30);
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				if (ImGui::Button("Cancel"))
					ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}

			if (ImGui::BeginPopup("Delete TimeAct"))
			{
				std::string label = "TrackID: " + std::to_string(tae->getTrackList()->getTrack(selectedTrackIdx)->getTrackId());
				ImGui::Text(label.c_str());

				if (ImGui::Button("Delete"))
				{
					tae->getTrackList()->deleteTrack(selectedTrackIdx);
					ImGui::CloseCurrentPopup();

					timeActEditor->reset();
					selectedTrackIdx = -1;
				}

				ImGui::SameLine();

				if (ImGui::Button("Cancel"))
					ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}

			static ImGuiTextFilter filter;
			ImGui::Text("Filter:");
			filter.Draw("##asset searchbar", 340.f);

			ImGui::BeginChild("tae_list");

			int taeCount = tae->getTrackList()->getNumTracks();

			for (size_t i = 0; i < taeCount; i++)
			{
				TimeAct::TaeExport::TimeActTrackExportXML* track = tae->getTrackList()->getTrack(i);

				int id = track->getTrackId();

				std::string label = std::to_string(id);

				bool selected = (track == timeActEditor->getSource());
				bool edited = false;

				if (filter.PassFilter(label.c_str()))
				{
					ImGui::PushID(i);

					ImGui::Selectable(label.c_str(), selected);

					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
					{
						selectedTrackIdx = i;
						timeActEditor->setTimeAct(track);
					}

					ImGui::PopID();
				}
			}

			ImGui::EndChild();

			ImGui::EndTabItem();
		}
	}

	ImGui::EndTabBar();
	ImGui::End();
}

void GuiManager::eventTrackEditorWindow()
{
	MorphemeEditorApp* editorApp = MorphemeEditorApp::getInstance();

	ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_Appearing);

	ImGui::Begin("EventTrack");
	TrackEditor::EventTrackEditor* eventTrackEditor = editorApp->getEventTrackEditor();

	if (eventTrackEditor->getSource() != nullptr)
		ImGui::Text(RString::removeExtension(editorApp->getCharacter()->getMorphemeCharacterDef()->getAnimFileLookUp()->getSourceFilename(eventTrackEditor->getSource()->getAnimID())).c_str());

	if (ImGui::Button("Load"))
		eventTrackEditor->setReload(true);

	ImGui::SameLine();

	if (ImGui::Button("Save"))
		eventTrackEditor->saveTracks();

	eventTrackEditor->editorGUI();

	ImGui::End();
}

void GuiManager::timeActEditorWindow()
{
	MorphemeEditorApp* editorApp = MorphemeEditorApp::getInstance();

	ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_Appearing);

	ImGui::Begin("TimeAct");
	TrackEditor::TimeActEditor* timeActEditor = editorApp->getTimeActEditor();

	if (timeActEditor->getSource() != nullptr)
		ImGui::Text(std::to_string(timeActEditor->getSource()->getTrackId()).c_str());

	if (ImGui::Button("Load"))
		timeActEditor->setReload(true);

	ImGui::SameLine();

	if (ImGui::Button("Save"))
		timeActEditor->saveTracks();

	timeActEditor->editorGUI();

	ImGui::End();
}

void GuiManager::eventTrackInfoWindow()
{
	MorphemeEditorApp* editorApp = MorphemeEditorApp::getInstance();

	ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_Appearing);

	ImGui::Begin("EventTrack Data");
	TrackEditor::EventTrackEditor* eventTrackEditor = editorApp->getEventTrackEditor();

	TrackEditor::Track* selectedTrack = eventTrackEditor->getSelectedTrack();

	if (selectedTrack != nullptr)
	{
		ImGui::InputText("Track Name", selectedTrack->name, 256);
		ImGui::InputInt("User Data##track", &selectedTrack->userData);

		if (ImGui::IsItemHovered())
		{
			ImGui::PushTextWrapPos(ImGui::GetWindowContentWidth());
			ImGui::Text(getEventTrackGroupTooltip(selectedTrack->userData).c_str());
			ImGui::PopTextWrapPos();
		}

		ImGui::SeparatorText("Event Data");

		TrackEditor::Event* selectedEvent = eventTrackEditor->getSelectedEvent();

		if (selectedEvent != nullptr)
		{
			float fps = eventTrackEditor->getFps();
			float step = 1.f / fps;

			float startTime = RMath::frameToTime(selectedEvent->frameStart, fps);
			float endTime = RMath::frameToTime(selectedEvent->frameEnd, fps);

			ImGui::InputFloat("Start Time", &startTime, step);

			if (!selectedTrack->discrete)
				ImGui::InputFloat("End Time", &endTime, step);

			ImGui::InputInt("User Data", &selectedEvent->userData);

			if (ImGui::IsItemHovered())
			{
				ImGui::PushTextWrapPos(ImGui::GetWindowContentWidth());
				ImGui::Text(getEventTrackEventTooltip(selectedEvent->userData).c_str());
				ImGui::PopTextWrapPos();
			}

			selectedEvent->frameStart = RMath::timeToFrame(startTime, fps);
			selectedEvent->frameEnd = RMath::timeToFrame(endTime, fps);
		}
	}

	ImGui::End();
}

void GuiManager::timeActInfoWindow()
{
	MorphemeEditorApp* editorApp = MorphemeEditorApp::getInstance();

	ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_Appearing);

	ImGui::Begin("TimeAct Data");
	TrackEditor::TimeActEditor* timeActEditor = editorApp->getTimeActEditor();

	TrackEditor::Track* selectedTrack = timeActEditor->getSelectedTrack();
	TrackEditor::Event* selectedEvent = timeActEditor->getSelectedEvent();

	if (selectedTrack != nullptr)
	{
		std::string label = selectedTrack->name;

		if (selectedEvent)
			label = g_taeTemplate->getEventName(selectedTrack->userData, selectedEvent->userData) + selectedEvent->getArgumentsString(selectedTrack->userData, g_taeTemplate);

		ImGui::Text(label.c_str());
		ImGui::InputInt("Group ID", &selectedTrack->userData, 0, 0, ImGuiInputTextFlags_ReadOnly);

		if (ImGui::IsItemHovered())
		{
			ImGui::PushTextWrapPos(ImGui::GetWindowContentWidth());
			ImGui::Text(getTimeActGroupTooltip(selectedTrack->userData).c_str());
			ImGui::PopTextWrapPos();
		}

		ImGui::SeparatorText("Event Data");

		if (selectedEvent != nullptr)
		{
			float fps = timeActEditor->getFps();
			float step = 1.f / fps;

			float startTime = RMath::frameToTime(selectedEvent->frameStart, fps);
			float endTime = RMath::frameToTime(selectedEvent->frameEnd, fps);

			ImGui::InputFloat("Start Time", &startTime, step);
			ImGui::InputFloat("End Time", &endTime, step);

			ImGui::InputInt("Event ID", &selectedEvent->userData, 0, 0, ImGuiInputTextFlags_ReadOnly);

			if (ImGui::IsItemHovered())
			{
				ImGui::PushTextWrapPos(ImGui::GetWindowContentWidth());
				ImGui::Text(getTimeActEventTooltip(selectedEvent->userData).c_str());
				ImGui::PopTextWrapPos();
			}

			selectedEvent->frameStart = RMath::timeToFrame(startTime, fps);
			selectedEvent->frameEnd = RMath::timeToFrame(endTime, fps);

			if (selectedEvent->arguments.size() > 0)
			{
				ImGui::SeparatorText("Arguments");

				selectedEvent->argsEditorGUI(selectedTrack->userData, g_taeTemplate);
			}
		}
	}

	ImGui::End();
}

void GuiManager::graphicsSettingsWindow()
{
	MorphemeEditorApp* editorApp = MorphemeEditorApp::getInstance();
	RenderManager* renderMan = RenderManager::getInstance();

	ImGui::SetNextWindowSize(ImVec2(200, 300), ImGuiCond_Appearing);
	ImGui::Begin("Graphics", &editorApp->getWindowFlags()->graphicsSettings);

	const char* msaaQualitySettings[6] = {"Off", "2x", "4x", "8x", "16x"};
	int selectedSetting = 0;

	switch (renderMan->getSettings()->msaaCount)
	{
	case 1:
		selectedSetting = 0;
		break;
	case 2:
		selectedSetting = 1;
		break;
	case 4:
		selectedSetting = 2;
		break;
	case 8:
		selectedSetting = 3;
		break;
	case 16:
		selectedSetting = 4;
		break;
	}

	if (ImGui::BeginCombo("MSAA", msaaQualitySettings[selectedSetting]))
	{
		for (size_t i = 0; i < 5; i++)
		{
			ImGui::Selectable(msaaQualitySettings[i]);

			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
			{
				const int msaaCount = 1 << i;

				renderMan->getSettings()->msaaCount = msaaCount;
			}
		}

		ImGui::EndCombo();
	}

	ImGui::End();
}

void GuiManager::colorSettingsWindow()
{
	MorphemeEditorApp* editorApp = MorphemeEditorApp::getInstance();

	ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_Appearing);
	ImGui::Begin("Color Settings", &editorApp->getWindowFlags()->styleEditor);

	ImGui::BeginTabBar("color_categories");

#ifdef _DEBUG
	if (ImGui::BeginTabItem("ImGui"))
	{
		ImGui::ShowStyleEditor();

		ImGui::EndTabItem();
	}
#endif

	if (ImGui::BeginTabItem("EventTrack Editor"))
	{
		trackEditorColorSelector(editorApp->getEventTrackEditor());

		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("TimeAct Editor"))
	{
		trackEditorColorSelector(editorApp->getTimeActEditor());

		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();

	ImGui::End();
}

void GuiManager::sceneExplorerWindow()
{
	MorphemeEditorApp* editorApp = MorphemeEditorApp::getInstance();

	ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_Appearing);

	ImGui::Begin("Scene Explorer", &editorApp->getWindowFlags()->sceneExplorer);

	FlverModel* model = editorApp->getCharacter()->getCharacterModelCtrl()->getModel();
	modelTreeNode(editorApp, model);

	model = editorApp->getCharacter()->getCharacterModelCtrl()->getModelPart(kPartsFace);
	modelTreeNode(editorApp, model);

	model = editorApp->getCharacter()->getCharacterModelCtrl()->getModelPart(kPartsHead);
	modelTreeNode(editorApp, model);

	model = editorApp->getCharacter()->getCharacterModelCtrl()->getModelPart(kPartsBody);
	modelTreeNode(editorApp, model);

	model = editorApp->getCharacter()->getCharacterModelCtrl()->getModelPart(kPartsArm);
	modelTreeNode(editorApp, model);

	model = editorApp->getCharacter()->getCharacterModelCtrl()->getModelPart(kPartsLeg);
	modelTreeNode(editorApp, model);

	model = editorApp->getCharacter()->getCharacterModelCtrl()->getModelPart(kPartsWeaponLeft);
	modelTreeNode(editorApp, model);

	model = editorApp->getCharacter()->getCharacterModelCtrl()->getModelPart(kPartsWeaponRight);
	modelTreeNode(editorApp, model);

	model = editorApp->getCharacter()->getCharacterModelCtrl()->getModelFg(kFgFace);
	modelTreeNode(editorApp, model);

	model = editorApp->getCharacter()->getCharacterModelCtrl()->getModelFg(kFgHead);
	modelTreeNode(editorApp, model);

	model = editorApp->getCharacter()->getCharacterModelCtrl()->getModelFg(kFgEyeBrows);
	modelTreeNode(editorApp, model);

	model = editorApp->getCharacter()->getCharacterModelCtrl()->getModelFg(kFgEyeBrows);
	modelTreeNode(editorApp, model);

	model = editorApp->getCharacter()->getCharacterModelCtrl()->getModelFg(kFgBeard);
	modelTreeNode(editorApp, model);

	model = editorApp->getCharacter()->getCharacterModelCtrl()->getModelFg(kFgHair);
	modelTreeNode(editorApp, model);

	ImGui::End();
}

void GuiManager::partsManagerWindow()
{
	MorphemeEditorApp* editorApp = MorphemeEditorApp::getInstance();
	FlverResources* resources = editorApp->getFlverResources();
	Character* character = editorApp->getCharacter();

	ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_Appearing);

	ImGui::Begin("Parts Manager", &editorApp->getWindowFlags()->playerPartsManager);

	ImGui::BeginTabBar("parts_selector");

	static bool female = editorApp->getPlayerModelPreset()->getBool("Gender", "is_female", false);

	if (ImGui::BeginTabItem("Armour"))
	{
		ImGui::Checkbox("Female", &female);

		ImGui::Separator();

		if (ImGui::TreeNode("Head"))
		{
			modelPartsSelectorGUI(resources->headModelResources, female, character, kPartsHead);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Face"))
		{
			modelPartsSelectorGUI(resources->faceModelResources, female, character, kPartsFace);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Body"))
		{
			modelPartsSelectorGUI(resources->bodyModelResources, female, character, kPartsBody);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Arm"))
		{
			modelPartsSelectorGUI(resources->armModelResources, female, character, kPartsArm);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Leg"))
		{
			modelPartsSelectorGUI(resources->legModelResources, female, character, kPartsLeg);

			ImGui::TreePop();
		}

		ImGui::EndTabItem();
	}
	
	if (ImGui::BeginTabItem("Right Hand"))
	{
		static bool rh_shield = editorApp->getPlayerModelPreset()->getBool("Right", "is_shield", false);
		ImGui::Checkbox("Shield", &rh_shield);

		ImGui::Separator();

		if (rh_shield)
			modelPartsSelectorGUI(resources->shieldModelResources, character, kPartsWeaponRight);
		else
			modelPartsSelectorGUI(resources->weaponModelResources, character, kPartsWeaponRight);

		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Left Hand"))
	{
		static bool lh_shield = editorApp->getPlayerModelPreset()->getBool("Left", "is_shield", false);
		ImGui::Checkbox("Shield", &lh_shield);

		ImGui::Separator();

		if (lh_shield)
			modelPartsSelectorGUI(resources->shieldModelResources, character, kPartsWeaponLeft);
		else
			modelPartsSelectorGUI(resources->weaponModelResources, character, kPartsWeaponLeft);

		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("FaceGen"))
	{
		ImGui::Checkbox("Female", &female);

		ImGui::Separator();

		if (ImGui::TreeNode("Hair"))
		{
			modelFgSelectorGUI(resources->fgHairModelResources, female, character, kFgHair);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Head"))
		{
			modelFgSelectorGUI(resources->fgHeadModelResources, female, character, kFgHead);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Face"))
		{
			modelFgSelectorGUI(resources->fgFaceModelResources, female, character, kFgFace);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Eyes"))
		{
			modelFgSelectorGUI(resources->fgEyesModelResources, female, character, kFgEyes);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("EyeBrows"))
		{
			modelFgSelectorGUI(resources->fgEyeBrowsModelResources, female, character, kFgEyeBrows);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Beard"))
		{
			modelFgSelectorGUI(resources->fgBeardModelResources, female, character, kFgBeard);

			ImGui::TreePop();
		}

		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();

	ImGui::End();
}

void GuiManager::searchQueryWindow()
{
	MorphemeEditorApp* editorApp = MorphemeEditorApp::getInstance();
	TimeAct::TaeExport::TimeActExportXML* timeAct = editorApp->getCharacter()->getTimeAct();

	ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_Appearing);

	ImGui::Begin("Find", &editorApp->getWindowFlags()->searchQuery);

	static int selectedRow = -1;
	static int targetValue = 0;
	static std::vector<TimeAct::TaeExport::TimeActEventExportXML*> queryResult;

	if (ImGui::IsWindowAppearing())
	{
		selectedRow = -1;
		targetValue = 0;
		queryResult.clear();
	}

	constexpr int rowCount = 50;
	constexpr int columnCount = 3;

	ImGui::InputInt("Target Value", &targetValue, 0, 0);

	if (ImGui::Button("Search"))
	{
		timeAct->findEventsWithId(queryResult, targetValue);
		selectedRow = -1;

		if (queryResult.size() == 0)
			g_appLog->alertMessage(MsgLevel_Info, "Could not find any event with the specified ID\n");
	}

	static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody;

	if (ImGui::BeginTable("result_list", columnCount, flags))
	{
		ImGui::TableSetupColumn("TimeAct", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Track", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Event", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableHeadersRow();

		if (queryResult.size())
		{
			for (size_t row = 0; row < queryResult.size(); row++)
			{
				ImGui::TableNextRow(ImGuiTableRowFlags_None, 20.f);

				TimeAct::TaeExport::TimeActEventExportXML* result = queryResult[row];

				int groupId = result->getOwner()->getGroupId();
				int eventId = result->getEventId();

				std::string col0 = std::to_string(result->getOwner()->getOwner()->getTrackId());
				std::string col1 = g_taeTemplate->getGroupName(groupId);
				std::string col2 = g_taeTemplate->getEventName(groupId, eventId) + result->getArgumentsString();

				for (size_t column = 0; column < columnCount; column++)
				{
					ImGui::TableSetColumnIndex(column);
					switch (column)
					{
					case 0:
						ImGui::Selectable(col0.c_str(), selectedRow == row, ImGuiSelectableFlags_SpanAllColumns);

						if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
						{
							selectedRow = row;
							editorApp->getTimeActEditor()->setTimeAct(result->getOwner()->getOwner());
						}
						break;
					case 1:
						ImGui::Selectable(col1.c_str(), selectedRow == row, ImGuiSelectableFlags_SpanAllColumns);

						if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
						{
							selectedRow = row;
							editorApp->getTimeActEditor()->setTimeAct(result->getOwner()->getOwner());
						}
						break;
					case 2:
						ImGui::Selectable(col2.c_str(), selectedRow == row, ImGuiSelectableFlags_SpanAllColumns);

						if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
						{
							selectedRow = row;
							editorApp->getTimeActEditor()->setTimeAct(result->getOwner()->getOwner());
						}
						break;
					}
				}
			}
		}
		else
		{
			for (size_t row = 0; row < rowCount; row++)
			{
				ImGui::TableNextRow(ImGuiTableRowFlags_None, 20.f);

				TimeAct::TaeExport::TimeActEventExportXML* result = queryResult[row];

				for (size_t column = 0; column < columnCount; column++)
				{
					ImGui::TableSetColumnIndex(column);
				}
			}
		}

		ImGui::EndTable();
	}

	ImGui::End();
}

void GuiManager::progressIndicatorPopup()
{
	if (!g_workerThread.load()->isDone())
	{
		ImGui::OpenPopup(g_workerThread.load()->getThreadName().c_str());

		ImGui::SetNextWindowPos(ImGui::GetWindowPos() + (ImGui::GetWindowSize() / 2 - ImVec2(150, 50)), ImGuiCond_Always);

		if (ImGui::BeginPopupModal(g_workerThread.load()->getThreadName().c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
		{
			for (size_t i = 0; i < g_workerThread.load()->getNumProcesses(); i++)
			{
				if (g_workerThread.load()->isProcessBusy(i))
					ImGui::CompositeProgressBar(g_workerThread.load()->getProcessName(i).c_str(), g_workerThread.load()->getProcessStep(i), g_workerThread.load()->getProcessNumSteps(i), g_workerThread.load()->getProcessStepName(i).c_str());
			}

			if (g_workerThread.load()->isDone())
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}
	}
}