#include "WorkerThread.h"

WorkerThread* WorkerThread::getInstance()
{
	if (_instance == nullptr)
		_instance = new WorkerThread;

	return _instance;
}

void WorkerThread::update()
{
	for (size_t i = 0; i < this->m_processes.size(); i++)
	{
		if (!this->m_processes[i].isBusy())
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);
			this->m_processes.erase(this->m_processes.begin() + i);
		}
	}

	if (this->m_done)
	{
		this->m_processes.clear();
		this->join();
	}
}

void WorkerThread::join()
{
	std::lock_guard<std::mutex> lock(this->m_mutex);

	if (this->m_thread.joinable())
		this->m_thread.join();
}

void WorkerThread::addProcess(std::string name, int numSteps)
{
	if (!this->m_done)
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);
		this->m_processes.push_back(ProgressIndicator(name, numSteps));
	}
}