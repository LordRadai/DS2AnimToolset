#pragma once
#include "extern.h"
#include <string>

class ProgressIndicator
{
public:
	ProgressIndicator() {}

	ProgressIndicator(std::string name, int numElements)
	{
		this->m_processName = name;
		this->m_step = 0;
		this->m_numSteps = numElements;
		this->m_busy = true;
	}

	~ProgressIndicator() {}

	int getStep() const { return this->m_step; }
	int getNumSteps() const { return this->m_numSteps; }
	float getProgress() const { return float(this->m_step) / float(this->m_numSteps); }
	std::string getProcessName() const { return this->m_processName; }
	std::string getStepName() const { return this->m_stepName; }
	bool isBusy() const { return this->m_busy; }

	void setStep(int step);
	void setStepName(std::string name);

private:
	void reset();

	int m_step = 0;
	int m_numSteps = 0;
	std::string m_processName = "";
	std::string m_stepName = "";
	bool m_busy = false;
};