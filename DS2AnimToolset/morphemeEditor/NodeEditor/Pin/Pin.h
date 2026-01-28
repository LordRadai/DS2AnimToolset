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
		std::string m_name;
		PinType m_type;

	public:
		Pin(Node* parent, const std::string& name, bool isInput);
		virtual ~Pin() override {}

		const std::string& getName() const { return m_name; }

		void draw();

		bool connectTo(Pin* other);
	private:
		void drawInputPin();
		void drawOutputPin();
	};
}
