#pragma once
#include "NodeEditor/Editor/Entity/Entity.h"

namespace NodeEditor
{
	class Pin;

	class Link : public Entity
	{
		Pin* m_inputPin;
		Pin* m_outputPin;
	public:
		Link(Editor* editor, Pin* inputPin, Pin* outputPin);
		virtual ~Link() override {}

		virtual void draw() override;
		virtual const std::string getFullName() const override { return ""; }

		Pin* getInputPin() const { return m_inputPin; }
		Pin* getOutputPin() const { return m_outputPin; }
	};
}