#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include "ProgressIndicator/ProgressIndicator.h"

class WorkerThread
{
public:
	static WorkerThread* getInstance();

	~WorkerThread() {}

	WorkerThread(const WorkerThread&) = delete;
	void operator=(const WorkerThread&) = delete;

	void update();
	void join();

	bool isDone() const 
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);
		return this->m_done;
	}

	template <class _Fn, class... _Args, std::enable_if_t<!std::is_same_v<std::_Remove_cvref_t<_Fn>, std::thread>, int> = 0>
	void startThread(std::string name, _Fn&& func, _Args&&... args)
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		this->m_done = false;
		this->m_threadName = name;

		this->m_thread = std::thread([this, func = std::forward<_Fn>(func)](auto&&... args_inner) {
			std::invoke(func, std::forward<decltype(args_inner)>(args_inner)...);

			std::lock_guard<std::mutex> lock(this->m_mutex);
			this->m_done = true;
			}, std::forward<_Args>(args)...);
	}

	void addProcess(std::string name, int numSteps);

	std::string getThreadName() const 
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);
		return this->m_threadName; 
	}

	int getNumProcesses() const 
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);
		return this->m_processes.size(); 
	}

	int getProcessStep(int idx) const
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);
		return this->m_processes[idx].getStep();
	}

	int getProcessNumSteps(int idx) const
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);
		return this->m_processes[idx].getNumSteps();
	}

	float getProcessProgress(int idx) const
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);
		return this->m_processes[idx].getProgress();
	}

	std::string getProcessName(int idx) const 
	{ 
		std::lock_guard<std::mutex> lock(this->m_mutex);
		return this->m_processes[idx].getProcessName();
	}

	std::string getProcessStepName(int idx) const 
	{ 
		std::lock_guard<std::mutex> lock(this->m_mutex);
		return this->m_processes[idx].getStepName();
	}

	bool isProcessBusy(int idx) const 
	{ 
		std::lock_guard<std::mutex> lock(this->m_mutex);
		return this->m_processes[idx].isBusy();
	}

	void increaseProgressStep()
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);
		
		int step = this->m_processes.back().getStep();
		this->m_processes.back().setStep(++step);

		for (size_t i = 0; i < this->m_processes.size(); i++)
		{
			if (!this->m_processes[i].isBusy())
				this->m_processes.erase(this->m_processes.begin() + i);
		}
	}

	void setProcessStepName(std::string name)
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);
		this->m_processes.back().setStepName(name);
	}

private:
	WorkerThread() {}

	std::string m_threadName = "";
	std::thread m_thread;
	bool m_done = true;
	std::vector<ProgressIndicator> m_processes;
	mutable std::mutex m_mutex;

	inline static WorkerThread* _instance;
};