#include "ProgressIndicator.h"
#include "RCore.h"

void ProgressIndicator::setStep(int step)
{
	if (!this->m_busy)
		g_appLog->panicMessage("Forgot to call startProcessing before setting current step\n");

	this->m_step = step;

	if (this->m_step >= this->m_numSteps)
		this->m_busy = false;
}

void ProgressIndicator::setStepName(std::string name)
{
	this->m_stepName = name;
}

void ProgressIndicator::reset()
{
	this->m_processName = "";
	this->m_step = 0;
	this->m_numSteps = 0;
	this->m_stepName = "";
	this->m_busy = false;
}