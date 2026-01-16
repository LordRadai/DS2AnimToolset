#pragma once
#include "NMDatabase/NMDatabase.h"
#include "mcd/Graph/Graph.h"
#include "NMDBExtensions/TypedNodeContainer.inl"
#include "BlendTreeNode.h"
#include "mcd/Attribute/SingleValue/FloatAttribute.h"

namespace mcd
{
	class BlendTree : public Graph
	{
		std::unique_ptr<db::TypedNodeContainer<mcd::BlendTreeNode>> m_blendTreeNodes;
		std::unique_ptr<db::FloatAttribute> m_controlParamXPos;
		std::unique_ptr<db::FloatAttribute> m_controlParamYPos;
		std::unique_ptr<db::FloatAttribute> m_controlParamWidth;
		std::unique_ptr<db::FloatAttribute> m_controlParamHeight;
		std::unique_ptr<db::FloatAttribute> m_emittedControlParamXPos;
		std::unique_ptr<db::FloatAttribute> m_emittedControlParamYPos;
		std::unique_ptr<db::FloatAttribute> m_emittedControlParamWidth;
		std::unique_ptr<db::FloatAttribute> m_emittedControlParamHeight;
		std::unique_ptr<db::FloatAttribute> m_outputPinXPos;
		std::unique_ptr<db::FloatAttribute> m_outputPinYPos;
		std::unique_ptr<db::FloatAttribute> m_outputPinWidth;
		std::unique_ptr<db::FloatAttribute> m_outputPinHeight;

	public:
		BlendTree(db::CompositeAttribute* parent, std::string name);

		virtual ~BlendTree() override {};

		void addBlendTreeNode(mcd::BlendTreeNode* node) { m_blendTreeNodes->add(node); }
		mcd::BlendTreeNode* getBlendTreeNode(const uint32_t index) const { return m_blendTreeNodes->getNode(index); }
		mcd::BlendTreeNode* findBlendTreeNode(const std::string& name) const { return m_blendTreeNodes->find(name); }
		uint32_t getNumBlendTreeNodes() const { return m_blendTreeNodes->size(); }

		float getControlParamXPos() const { return m_controlParamXPos->getValue(); }
		float getControlParamYPos() const { return m_controlParamYPos->getValue(); }
		float getControlParamWidth() const { return m_controlParamWidth->getValue(); }
		float getControlParamHeight() const { return m_controlParamHeight->getValue(); }
		float getEmittedControlParamXPos() const { return m_emittedControlParamXPos->getValue(); }
		float getEmittedControlParamYPos() const { return m_emittedControlParamYPos->getValue(); }
		float getEmittedControlParamWidth() const { return m_emittedControlParamWidth->getValue(); }
		float getEmittedControlParamHeight() const { return m_emittedControlParamHeight->getValue(); }
		float getOutputPinXPos() const { return m_outputPinXPos->getValue(); }
		float getOutputPinYPos() const { return m_outputPinYPos->getValue(); }
		float getOutputPinWidth() const { return m_outputPinWidth->getValue(); }
		float getOutputPinHeight() const { return m_outputPinHeight->getValue(); }

		void setControlParamXPos(float x) { m_controlParamXPos->setValue(x); }
		void setControlParamYPos(float y) { m_controlParamYPos->setValue(y); }
		void setControlParamWidth(float width) { m_controlParamWidth->setValue(width); }
		void setControlParamHeight(float height) { m_controlParamHeight->setValue(height); }
		void setEmittedControlParamXPos(float x) { m_emittedControlParamXPos->setValue(x); }
		void setEmittedControlParamYPos(float y) { m_emittedControlParamYPos->setValue(y); }
		void setEmittedControlParamWidth(float width) { m_emittedControlParamWidth->setValue(width); }
		void setEmittedControlParamHeight(float height) { m_emittedControlParamHeight->setValue(height); }
		void setOutputPinXPos(float x) { m_outputPinXPos->setValue(x); }
		void setOutputPinYPos(float y) { m_outputPinYPos->setValue(y); }
		void setOutputPinWidth(float width) { m_outputPinWidth->setValue(width); }
		void setOutputPinHeight(float height) { m_outputPinHeight->setValue(height); }

		/**
		 * \brief Returns the first free position in the graph.
		 * 
		 * \param x Output buffer for the x position.
		 * \param y Output buffer for the y position.
		 */
		void getFreePosition(float& x, float& y);

		/**
		 * \brief Returns the number of nodes with the specified type.
		 * 
		 * \param type The node type name.
		 * \return The number of nodes of the specified type.
		 */
		uint32_t getNumNodesOfType(const std::string& type) const;
	};
}
