#pragma once
#include <string>
#include "NodeEditor/Entity/Entity.h"
#include "NodeEditor/imnodes/imnodes.h"

namespace NodeEditor
{
	class Node;

	class Pin : public Entity
	{
	protected:
		Node* m_parentNode;
		bool m_isInput;

	public:
		Pin(NodeEditor* editor, Node* parent, const std::string& name, bool isInput);
		virtual ~Pin() override {}

		virtual void draw() override;

		Node* getParentNode() const { return m_parentNode; }
		bool connectTo(Pin* other);
	protected:
		void drawInternal(ImNodesPinShape_ shape, ImColor color);
	};
}
