#pragma once

class Application
{
public:
	~Application();

	bool isInitialised() const { return this->m_initialised; }

	virtual void initialise() = 0;
	virtual void update(float dt) = 0;
	virtual void shutdown() = 0;

protected:
	Application();

	bool m_initialised = false;
};