#include "Application/Application.h"

#include "extern.h"
#include "framework.h"

#include "fromloader/fromloader.h"
#include "FlverModel/FlverModel.h"

#include "MorphemeSystem/MorphemeSystem.h"
#include "Character/Character.h"
#include "AnimPlayer/AnimPlayer.h"
#include "Camera/Camera.h"
#include "TrackEditor/TimeActEditor.h"
#include "TrackEditor/EventTrackEditor.h"
#include "FileIDNamesTable/FileIDNamesTable.h"
#include "PlayerModelPreset/PlayerModelPreset.h"

struct FlverResources
{
	FileIDNamesTable weaponModelResources;
	FileIDNamesTable shieldModelResources;

	FileIDNamesTable faceModelResources;
	FileIDNamesTable headModelResources;
	FileIDNamesTable bodyModelResources;
	FileIDNamesTable armModelResources;
	FileIDNamesTable legModelResources;

	FileIDNamesTable fgFaceModelResources;
	FileIDNamesTable fgHeadModelResources;
	FileIDNamesTable fgEyesModelResources;
	FileIDNamesTable fgEyeBrowsModelResources;
	FileIDNamesTable fgBeardModelResources;
	FileIDNamesTable fgHairModelResources;
};

class MorphemeEditorApp : public Application
{
public:
	enum ExportFormat
	{
		kFbx,
		kXmd,
		kNumExportFormats
	};

	struct TaskFlags
	{
		bool resetCamera = false;

		bool loadFile = false;
		bool saveFile = false;

		bool exportTaeTemplateXml = false;
		bool exportTae = false;
		bool exportModel = false;
		bool exportAnimations = false;
		bool exportNetwork = false;
		bool exportAll = false;
		bool exportAndProcess = false;
		ExportFormat exportFormat = kFbx;

		bool compileNetwork = false;
		bool compileTaes = false;
	};

	struct PreviewFlags
	{
		bool drawBones = true;
		bool drawDummies = false;
		bool drawBoundingBoxes = false;
		DisplayMode displayMode = kDispNormal;

		FlverModel* selectedModel = nullptr;
	};

	struct WindowFlags
	{
		bool searchQuery = false;
		bool graphicsSettings = false;
		bool styleEditor = false;
		bool imGuiDemo = false;
		bool sceneExplorer = false;
		bool playerPartsManager = false;
	};

	~MorphemeEditorApp();

	MorphemeEditorApp(const MorphemeEditorApp&) = delete;
	void operator=(const MorphemeEditorApp&) = delete;

	void initialise();
	void update(float dt);
	void shutdown();

	Character* getCharacter() const { return this->m_character; }
	AnimPlayer* getAnimPlayer() const { return this->m_animPlayer; }
	Camera* getCamera() const { return this->m_camera; }
	TrackEditor::TimeActEditor* getTimeActEditor() const { return this->m_timeActEditor; }
	TrackEditor::EventTrackEditor* getEventTrackEditor() const { return this->m_eventTrackEditor; }
	FlverResources* getFlverResources() const { return this->m_flverResources; }
	PlayerModelPreset* getPlayerModelPreset() const { return this->m_playerModelPreset; }

	WindowFlags* getWindowFlags() { return &this->m_windowFlags; }
	TaskFlags* getTaskFlags() { return &this->m_taskFlags; }
	PreviewFlags* getPreviewFlags() { return &this->m_previewFlags; }
	std::vector<std::wstring> getTimeActFileList() const { return this->m_timeActFileList; }
	std::wstring getGamePath() const { return this->m_gamePath; }

	static MorphemeEditorApp* getInstance();

private:
	MorphemeEditorApp();

	void loadSettings();
	void saveSettings();

	void loadPlayerModelPreset();
	void savePlayerModelPreset();

	void loadFile();
	void saveFile();

	bool exportAll(std::wstring path);
	bool exportAndProcess(std::wstring path);
	bool exportAndCompileTae(std::wstring path);

	bool exportTimeAct(std::wstring path);
	bool exportNetwork(std::wstring path);
	void exportAnimationsAndMarkups(std::wstring path);
	bool exportModel(std::wstring path);
	bool exportAnimations(std::wstring path);
	bool exportAnimMarkups(std::wstring path);

	bool compileMorphemeAssets(std::wstring path);
	bool compileTimeActFiles(std::wstring path);

	bool exportAnimation(std::wstring path, int animSetIdx, int animId);
	bool exportAnimMarkup(std::wstring path, int animSetIdx, int animId);

	void exportTaeTemplateXML();

	WindowFlags m_windowFlags;
	TaskFlags m_taskFlags;
	PreviewFlags m_previewFlags;

	FlverResources* m_flverResources = nullptr;
	PlayerModelPreset* m_playerModelPreset = nullptr;
	Camera* m_camera = nullptr;
	AnimPlayer* m_animPlayer = nullptr;
	Character* m_character = nullptr;

	TrackEditor::TimeActEditor* m_timeActEditor = nullptr;
	TrackEditor::EventTrackEditor* m_eventTrackEditor = nullptr;

	std::vector<std::wstring> m_timeActFileList;

	std::wstring m_gamePath = L"";

	inline static MorphemeEditorApp* _instance = nullptr;
};