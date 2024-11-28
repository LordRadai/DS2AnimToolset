#pragma once
#include <Windows.h>
#include <d3d11.h>

class GuiManager
{
public:
	~GuiManager();

	GuiManager(const GuiManager&) = delete;
	void operator=(const GuiManager&) = delete;

	static GuiManager* getInstance();

	void initialise(HWND hwnd, ID3D11DeviceContext* pContext, ID3D11Device* pDevice);
	void shutdown();

	void update(float dt);

	void render(ID3D11DeviceContext* pContext, ID3D11RenderTargetView* pRenderTargetView);

	bool isApplicationFocused();
private:
	HWND m_window = nullptr;
	bool m_initialised = false;

	GuiManager();

	void initGuiStyle();

	inline static GuiManager* _instance = nullptr;

	void handleUserInputs();

	void rootWindow();
	void modelViewerWindow();
	void assetsWindow();
	void eventTrackEditorWindow();
	void timeActEditorWindow();
	void eventTrackInfoWindow();
	void timeActInfoWindow();

	void graphicsSettingsWindow();
	void colorSettingsWindow();
	void sceneExplorerWindow();
	void partsManagerWindow();
	void searchQueryWindow();

	void selectTimeActPopup();
	void importFilePopup();
	void progressIndicatorPopup();
};