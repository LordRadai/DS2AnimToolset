#pragma once
#include <atomic>

namespace fbxsdk
{
	class FbxManager;
}


namespace TimeAct
{
	class TaeTemplate;
}

class WorkerThread;
class RLog;
enum MsgLevel;

class MorphemeEditorApp;
class GuiManager;
class RenderManager;

extern std::atomic<WorkerThread*> g_workerThread;
extern MsgLevel g_logLevel;
extern RLog* g_appLog;
extern TimeAct::TaeTemplate* g_taeTemplate;
extern fbxsdk::FbxManager* g_pFbxManager;

extern MorphemeEditorApp* g_morphemeEditorApp;
extern GuiManager* g_guiManager;
extern RenderManager* g_renderManager;
