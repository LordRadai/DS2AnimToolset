#pragma once

class Application
{
public:
	~Application();

	bool isInitialised() const { return this->m_initialised; }

	virtual void initialise() { this->m_initialised = true; }
	virtual void update(float dt) { return; };
	virtual void shutdown() { return; }

protected:
	Application();

	bool m_initialised = false;
};