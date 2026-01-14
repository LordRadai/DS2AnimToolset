#include "BlendTree.h"

namespace mcd
{
	BlendTree::BlendTree(db::Attribute* parent, std::string name) : Graph(parent, "BlendTree", name),
		m_blendTreeNodes(std::make_unique<db::TypedNodeContainer<mcd::BlendTreeNode>>(this, "BlendTreeNodes")),
		m_controlParamXPos(std::make_unique<db::FloatAttribute>(this, "ControlParamXPosition", -100.0f)),
		m_controlParamYPos(std::make_unique<db::FloatAttribute>(this, "ControlParamYPosition", -100.0f)),
		m_controlParamWidth(std::make_unique<db::FloatAttribute>(this, "ControlParamWidth", 100.0f)),
		m_controlParamHeight(std::make_unique<db::FloatAttribute>(this, "ControlParamHeight", 200.0f)),
		m_emittedControlParamXPos(std::make_unique<db::FloatAttribute>(this, "EmittedControlParamXPosition", 100.0f)),
		m_emittedControlParamYPos(std::make_unique<db::FloatAttribute>(this, "EmittedControlParamYPosition", 100.0f)),
		m_emittedControlParamWidth(std::make_unique<db::FloatAttribute>(this, "EmittedControlParamWidth", 100.0f)),
		m_emittedControlParamHeight(std::make_unique<db::FloatAttribute>(this, "EmittedControlParamHeight", 200.0f)),
		m_outputPinXPos(std::make_unique<db::FloatAttribute>(this, "OutputPinXPos", 100.0f)),
		m_outputPinYPos(std::make_unique<db::FloatAttribute>(this, "OutputPinYPos", 500.0f)),
		m_outputPinWidth(std::make_unique<db::FloatAttribute>(this, "OutputPinWidth", 100.0f)),
		m_outputPinHeight(std::make_unique<db::FloatAttribute>(this, "OutputPinHeight", 200.0f))
	{
		addAttribute(m_blendTreeNodes.get());
		addAttribute(m_controlParamXPos.get());
		addAttribute(m_controlParamYPos.get());
		addAttribute(m_controlParamWidth.get());
		addAttribute(m_controlParamHeight.get());
		addAttribute(m_emittedControlParamXPos.get());
		addAttribute(m_emittedControlParamYPos.get());
		addAttribute(m_emittedControlParamWidth.get());
		addAttribute(m_emittedControlParamHeight.get());
		addAttribute(m_outputPinXPos.get());
		addAttribute(m_outputPinYPos.get());
		addAttribute(m_outputPinWidth.get());
		addAttribute(m_outputPinHeight.get());
	}

	void BlendTree::getFreePosition(float& x, float& y)
	{
		x = 10.f;
		y = 0.f;
		
		float maxX = 0.f;
		float maxY = 0.f;

		if (m_blendTreeNodes->empty())
			return;

		for (size_t i = 0; i < m_blendTreeNodes->size(); i++)
		{
			mcd::BlendTreeNode* node = m_blendTreeNodes->getNode(i);

			if (node == nullptr) continue;

			float nodexPos = node->getXPos();
			float nodeyPos = node->getYPos();

			float x = nodexPos ? nodexPos : 0.0f;
			float y = nodeyPos ? nodeyPos : 0.0f;

			if (!std::isnan(x) && x > maxX)
				maxX = x;

			if (!std::isnan(y) && y > maxY)
				maxY = y;
		}

		x = 10.0f;
		y = maxY + 100.0f;
	}

	uint32_t BlendTree::getNumNodesOfType(const std::string& type) const
	{
		uint32_t count = 0;

		for (size_t i = 0; i < m_blendTreeNodes->size(); i++)
		{
			mcd::BlendTreeNode* node = m_blendTreeNodes->getNode(i);

			if (node->getNodeType() == type)
				count++;
		}

		return count;
	}
}
