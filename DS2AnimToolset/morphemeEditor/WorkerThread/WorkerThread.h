#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <string>
#include <functional>
#include "ProgressIndicator/ProgressIndicator.h"

// Runs long tasks (exports, compiles) synchronously on the calling thread, while a separate
// UI thread owns a small native progress window and keeps it repainted and responsive.
// The task reports progress through addProcess/setProcessStepName/increaseProgressStep.
class WorkerThread
{
public:
	static WorkerThread* getInstance();

	~WorkerThread() {}

	WorkerThread(const WorkerThread&) = delete;
	void operator=(const WorkerThread&) = delete;

	bool isDone() const
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);
		return this->m_done;
	}

	// Invokes func(args...) on the calling thread and shows the progress window until it returns
	template <class _Fn, class... _Args>
	void runTask(std::string name, _Fn&& func, _Args&&... args)
	{
		// Nested task: it reports into the progress window that is already up
		if (!this->isDone())
		{
			std::invoke(std::forward<_Fn>(func), std::forward<_Args>(args)...);
			return;
		}

		this->beginTask(name);

		try
		{
			std::invoke(std::forward<_Fn>(func), std::forward<_Args>(args)...);
		}
		catch (...)
		{
			this->endTask();
			throw;
		}

		this->endTask();
	}

	// The editor's main window (HWND), which the progress window is kept inside of
	void setMainWindow(void* hwnd);

	void addProcess(std::string name, int numSteps);
	void increaseProgressStep();
	void setProcessStepName(std::string name);

	std::string getThreadName() const
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);
		return this->m_threadName;
	}

	int getNumProcesses() const
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);
		return int(this->m_processes.size());
	}

	// Copy of the active processes, safe to read from the progress UI thread
	std::vector<ProgressIndicator> getProcessesSnapshot() const
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);
		return this->m_processes;
	}

private:
	WorkerThread() {}

	void beginTask(std::string name);
	void endTask();

	void progressWindowThread(void* parentWnd);

	std::string m_threadName = "";
	std::thread m_uiThread;
	void* m_mainWnd = nullptr;
	bool m_done = true;
	std::vector<ProgressIndicator> m_processes;
	mutable std::mutex m_mutex;

	inline static WorkerThread* _instance;
};
