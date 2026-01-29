#pragma once
#include <string>
#include "NodeEditor/Entity/Entity.h"

namespace NodeEditor
{
	class Node;

	class Pin : public Entity
	{
	public:
		enum PinType
		{
			kPinTypeInput,
			kPinTypeOutput
		};

	private:
		Node* m_parentNode;
		PinType m_type;

	public:
		Pin(Node* parent, const std::string& name, bool isInput);
		virtual ~Pin() override {}

		virtual void draw() override;

		bool connectTo(Pin* other);
	private:
		void drawInputPin();
		void drawOutputPin();
	};
}
