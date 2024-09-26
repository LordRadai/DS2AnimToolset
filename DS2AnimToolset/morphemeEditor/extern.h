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

extern std::atomic<WorkerThread*> g_workerThread;
extern MsgLevel g_logLevel;
extern RLog* g_appLog;
extern TimeAct::TaeTemplate* g_taeTemplate;
extern fbxsdk::FbxManager* g_pFbxManager;