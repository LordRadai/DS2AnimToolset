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
		Link(Pin* inputPin, Pin* outputPin) : Entity(), m_inputPin(inputPin), m_outputPin(outputPin) {}
		virtual ~Link() override {}

		Pin* getInputPin() const { return m_inputPin; }
		Pin* getOutputPin() const { return m_outputPin; }

		void draw();
	};
}