#pragma once

namespace fbxsdk
{
	class FbxManager;
}

class Application;
class RLog;
class Scene;
class TaeTemplate;
enum MsgLevel;

extern MsgLevel g_logLevel;
extern Application* g_appRootWindow;
extern RLog* g_appLog;
extern Scene* g_scene;
extern TaeTemplate* g_taeTemplate;
extern fbxsdk::FbxManager* g_pFbxManager;