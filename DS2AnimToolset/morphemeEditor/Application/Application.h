#pragma once
#include <windows.h>
#include <thread>
#include <io.h>
#include <stdarg.h>
#include <strsafe.h>
#include <d3d11.h>
#include <dxgi.h>
#include <XInput.h>
#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <fbxsdk.h>

#include "PrimitiveBatch.h"
#include "VertexTypes.h"
#include "BufferHelpers.h"
#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "DirectXHelpers.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PostProcess.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "WICTextureLoader.h"

#include "extern.h"
#include "framework.h"
#include "imsequencer/ImSequencer.h"
#include "FromsoftFormat/TimeActReader/TimeActReader.h"
#include "FromsoftFormat/BNDReader/BNDReader.h"
#include "FromsoftFormat/FlverReader/FlverReader.h"
#include "fromloader/fromloader.h"
#include "FlverModel/FlverModel.h"

#include "MorphemeSystem/MorphemeSystem.h"
#include "AnimPlayer/AnimPlayer.h"
#include "INI/INI.h"

using namespace cfr;

struct FileNamePathPair
{
	std::wstring m_path;
	std::string m_name;
	int m_id;

	FileNamePathPair() {}
	FileNamePathPair(std::wstring path);
};

struct FileNameMapPairList
{
	std::vector<FileNamePathPair> m_weaponModelPaths;
	std::vector<FileNamePathPair> m_shieldModelPaths;
	std::vector<FileNamePathPair> m_headModelPaths;
	std::vector<FileNamePathPair> m_bodyModelPaths;
	std::vector<FileNamePathPair> m_armModelPaths;
	std::vector<FileNamePathPair> m_legModelPaths;

	void Clear();
	void Create(std::wstring gamePath);
};

class Application
{
public:
	struct WindowStates
	{
		bool m_settingWindow = false;
		bool m_previewSettings = false;
		bool m_entityManager = false;
		bool m_queryTae = false;
		bool m_queryEventTrack = false;
	} m_windowStates;

	struct Flags
	{
		bool m_loadFile = false;
		bool m_saveNmb = false;
		bool m_saveTae = false;
		bool m_saveAll = false;
		bool m_exportAll = false;
		bool m_exportModel = false;
	} m_flags;

	struct ModelLoadFlags
	{
		bool m_loadModel = false;
		std::wstring m_modelPath = L"";
		PartType m_partType = Parts_Invalid;
	} m_modelLoadFlags;

	struct TimeActFlags
	{
		bool m_addTimeAct = false;
		bool m_deleteTimeAct = false;
		int m_addTimeActId = 0;
		float m_addTimeActLenght = 1.f;
		int m_deleteTimeActId = -1;
	} m_timeActFlags;

	struct FbxExportFlags
	{
		bool m_exportModelWithAnims = true;
		bool m_exportMorphemeRigWithModel = true;
	} m_fbxExportFlags;

	struct SceneFlags
	{
		DisplayMode m_displayMode = Mode_Normal;
		bool m_sceneExplorer = false;
		bool m_drawDummies = false;
		FlverModel* m_selectedModel = nullptr;
	} m_sceneFlags;
	
	std::wstring m_gamePath;

	int m_chrId = -1;

	MorphemeSystem* m_morphemeSystem;
	AnimPlayer* m_animPlayer;

	EventTrackEditor* m_eventTrackEditor;
	TimeActEditor* m_timeActEditor;

	TimeActReader* m_timeAct;

	INI* m_playerModelPreset;
	FileNameMapPairList* m_fileNameMapPairList;

	Application();
	~Application();

	void GUIStyle();
	void Initialise();
	void Shutdown();

	void Update(float delta_time);
	void RenderGUI(const char* title);
	void ModelPreviewWindow();
	void AssetsWindow();
	void EventTrackWindow(int* current_frame, int* first_frame, float* zoom_level, bool* is_expanded);
	void EventTrackInfoWindow();
	void TimeActWindow(int* current_frame, int* first_frame, float* zoom_level, bool* is_expanded);
	void TimeActInfoWindow();
	void SettingsWindow();
	void PreviewDebugManagerWindow();
	void PreviewSceneExplorerWindow();
	void EntityManagerWindow();

	void CheckFlags();

	void LoadFile();
	void SaveFile();
	void SaveNmbFile();
	void SaveTaeFile();

	bool ExportModelToFbx(std::filesystem::path export_path);
	bool ExportAnimationToFbx(std::filesystem::path export_path, int anim_id);

	void SetTimeActCurrentFrameFromEventTrack(int* current_frame_tae, int current_frame);
};