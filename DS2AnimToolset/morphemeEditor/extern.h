#pragma once

namespace fbxsdk
{
	class FbxManager;
}

class Application;
class Scene;
class TaeTemplate;
class ProcessReader;
enum MsgLevel;

extern MsgLevel g_logLevel;
extern Application g_morphemeConnect;
extern Scene g_preview;
extern TaeTemplate g_taeTemplate;
extern ProcessReader g_frpg2;
extern fbxsdk::FbxManager* g_pFbxManager;