#pragma once
#include "NodeEditor/Entity/Entity.h"

namespace NodeEditor
{
	class Pin;

	class Link : public Entity
	{
		Pin* m_inputPin;
		Pin* m_outputPin;
	public:
		Link(Pin* inputPin, Pin* outputPin);
		virtual ~Link() override {}

		virtual void draw() override;

		Pin* getInputPin() const { return m_inputPin; }
		Pin* getOutputPin() const { return m_outputPin; }
	};
}