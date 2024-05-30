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

using namespace cfr;

class Application
{
public:
	struct WindowStates
	{
		bool m_settingWindow = false;
		bool m_previewSettings = false;
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

	struct TimeActFlags
	{
		bool m_addTimeAct = false;
		bool m_deleteTimeAct = false;
		int m_addTimeActId = 0;
		float m_addTimeActLenght = 1.f;
		int m_deleteTimeActId = -1;
	} m_timeActFlags;

	struct EventTrackEditorFlags
	{
		bool m_load = false;
		bool m_save = false;
		int m_selectedAnimIdx = -1;
		std::vector<BYTE> m_edited;

		int m_targetAnimIdx = -1;
		float m_lenMult = 1.f;
		bool m_loadTae = false;
		std::vector<std::wstring> m_taeList;

		int m_selectedTrack = -1;
		int m_selectedEvent = -1;

		float m_eventTrackActionTimeActStart = 0.f;
		float m_eventTrackActionTimeActDuration = 0.f;
		int m_eventTrackActionTimeActValue = -1;
	} m_eventTrackEditorFlags;

	struct TimeActEditorFlags
	{
		bool m_load = false;
		bool m_save = false;
		int m_selectedTimeActIdx = -1;
		std::vector<BYTE> m_edited;
		int m_taeId = -1;

		int m_selectedTrack = -1;
		int m_selectedEvent = -1;
	} m_timeActEditorFlags;

	struct FbxExportFlags
	{
		bool m_exportModelWithAnims = true;
		bool m_exportMorphemeRigWithModel = true;
	} m_fbxExportFlags;
	
	MorphemeSystem m_morphemeSystem;
	TimeActReader m_tae;
	AnimPlayer m_animPlayer;
	BNDReader m_bnd;
	int m_chrId = -1;

	EventTrackEditor m_eventTrackEditor;
	TimeActEditor m_timeActEditor;

	Application();
	~Application();

	void GUIStyle();
	void Initialise();
	void Update();
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

	void CheckFlags();

	void LoadFile();
	void SaveFile();
	void SaveNmbFile();
	void SaveTaeFile();

	bool ExportModelToFbx(std::filesystem::path export_path);
	bool ExportAnimationToFbx(std::filesystem::path export_path, int anim_id);
	bool ExportToXMD(PWSTR export_path);
	bool ExportToFbxTest(PWSTR export_path);

	void SetTimeActCurrentFrameFromEventTrack(int* current_frame_tae, int current_frame);
	int GetChrIdFromNmbFileName(std::wstring name);
	std::wstring GetObjIdFromTaeFileName(std::wstring name);

	void ResetEventTrackEditor();
	void ResetTimeActEditor();
};