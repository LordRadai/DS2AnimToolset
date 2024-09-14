#include <fstream>
#include <fbxsdk.h>

#include "framework.h"
#include "MorphemeEditor.h"
#include "extern.h"
#include "MorphemeEditorApp/MorphemeEditorApp.h"
#include "GuiManager/GuiManager.h"
#include "RenderManager/RenderManager.h"
#include "WorkerThread/WorkerThread.h"
#include "FromSoftware/TimeAct/TaeTemplate/TaeTemplate.h"

static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;

// Forward declarations of helper functions
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

std::atomic<WorkerThread*> g_workerThread;
MsgLevel g_logLevel = MsgLevel_Debug;
TimeAct::TaeTemplate* g_taeTemplate = nullptr;
fbxsdk::FbxManager* g_pFbxManager = nullptr;
RLog* g_appLog;

// Main code
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    MorphemeEditorApp* morphemeEditorApp = MorphemeEditorApp::getInstance();
    GuiManager* guiManager = GuiManager::getInstance();
    RenderManager* renderManager = RenderManager::getInstance();
    g_workerThread.store(WorkerThread::getInstance());

    DX::StepTimer timer;
    //timer.SetFixedTimeStep(true);
    //timer.SetTargetElapsedSeconds(1.f / 60.f);

    g_appLog = new RLog(MsgLevel_Debug, "morphemeEditor.log", APPNAME_A);

    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, hInstance, LoadIcon(hInstance, MAKEINTRESOURCE(IDC_ICON)), LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, MAKEINTRESOURCEW(IDC_ICON),  LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL)) };
    ::RegisterClassExW(&wc);

    HWND hwnd = ::CreateWindowW(wc.lpszClassName, APPNAME_W, WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

#ifdef _CONSOLE
    AllocConsole();

    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    std::cout.clear();
    std::clog.clear();
    std::cerr.clear();
    std::cin.clear();
#endif

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    g_appLog->debugMessage(MsgLevel_Info, "Application startup\n");

    try
    {
        g_appLog->debugMessage(MsgLevel_Info, "Loading TimeAct template\n");

        g_taeTemplate = TimeAct::TaeTemplate::load(L"Data\\res\\TimeActTemplate.xml");
    }
    catch (const std::exception& exc)
    {
        g_appLog->alertMessage(MsgLevel_Error, exc.what());
    }

    try
    {
        g_appLog->debugMessage(MsgLevel_Info, "Initialising application core module\n");
        morphemeEditorApp->initialise();
    }
    catch (const std::exception& e)
    {
        morphemeEditorApp->shutdown();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

        g_appLog->panicMessage(e.what());

        return 1;
    }

    try
    {
        g_appLog->debugMessage(MsgLevel_Info, "Initialising rendering module\n");

        renderManager->initialise(hwnd);
    }
    catch (const std::exception& e)
    {
        renderManager->shutdown();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

        g_appLog->panicMessage(e.what());

        return 1;
    }

    try
    {
        g_appLog->debugMessage(MsgLevel_Info, "Initialising GUI module\n");
        guiManager->initialise(hwnd, renderManager->getDeviceContext(), renderManager->getDevice());
    }
    catch (const std::exception& e)
    {
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

        g_appLog->panicMessage(e.what());

        return 1;
    }

    g_appLog->debugMessage(MsgLevel_Info, "Creating FBX Manager\n");
    g_pFbxManager = FbxManager::Create();

    // Main loop
    bool done = false;
    while (!done)
    {
        try
        {
            // Poll and handle messages (inputs, window resize, etc.)
            // See the WndProc() function below for our to dispatch events to the Win32 backend.
            MSG msg;
            while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                if (msg.message == WM_QUIT)
                    done = true;
            }
            if (done)
                break;

            // Handle window resize (we don't resize directly in the WM_SIZE handler)
            if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
            {
                renderManager->resize(g_ResizeWidth, g_ResizeHeight);
                g_ResizeWidth = g_ResizeHeight = 0;
            }

            WorkerThread::getInstance()->update();

            timer.Tick([&]()
                {
                    float dt = float(timer.GetElapsedSeconds());

                    renderManager->update(dt);
                    guiManager->update(dt);
                    morphemeEditorApp->update(dt);
                });

            // Rendering
            if (timer.GetFrameCount() > 0)
                renderManager->render();

            renderManager->present();
        }
        catch (const std::exception& e)
        {
            g_appLog->panicMessage(e.what());
        }
    }

    try
    {
        WorkerThread::getInstance()->join();

        // Cleanup
        g_appLog->debugMessage(MsgLevel_Info, "Main app module shutdown\n");
        morphemeEditorApp->shutdown();

        g_appLog->debugMessage(MsgLevel_Info, "Gui module shutdown\n");
        guiManager->shutdown();

        g_appLog->debugMessage(MsgLevel_Info, "Rendering module shutdown\n");
        renderManager->shutdown();

        ::DestroyWindow(hwnd);
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

        g_appLog->debugMessage(MsgLevel_Info, "Exit\n");

        delete g_appLog;
    }
    catch (const std::exception& e)
    {
        g_appLog->panicMessage(e.what());

        return 1;
    }
    
    return 0;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_SETCURSOR:
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}