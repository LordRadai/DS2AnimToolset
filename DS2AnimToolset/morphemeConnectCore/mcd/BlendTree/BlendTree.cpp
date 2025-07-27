#include "BlendTree.h"

namespace mcd
{
	BlendTree::BlendTree(db::Attribute* parent, std::string name) : Graph(parent, "BlendTree", name),
		m_blendTreeNodes(this, "BlendTreeNodes"),
		m_controlParamXPos(this, "ControlParamXPos", 0.0f),
		m_controlParamYPos(this, "ControlParamYPos", 0.0f),
		m_controlParamWidth(this, "ControlParamWidth", 100.0f),
		m_controlParamHeight(this, "ControlParamHeight", 100.0f),
		m_emittedControlParamXPos(this, "EmittedControlParamXPos", 0.0f),
		m_emittedControlParamYPos(this, "EmittedControlParamYPos", 0.0f),
		m_emittedControlParamWidth(this, "EmittedControlParamWidth", 100.0f),
		m_emittedControlParamHeight(this, "EmittedControlParamHeight", 100.0f),
		m_outputPinXPos(this, "OutputPinXPos", 0.0f),
		m_outputPinYPos(this, "OutputPinYPos", 0.0f),
		m_outputPinWidth(this, "OutputPinWidth", 100.0f),
		m_outputPinHeight(this, "OutputPinHeight", 100.0f)
	{
		addAttribute(&m_blendTreeNodes);
		addAttribute(&m_controlParamXPos);
		addAttribute(&m_controlParamYPos);
		addAttribute(&m_controlParamWidth);
		addAttribute(&m_controlParamHeight);
		addAttribute(&m_emittedControlParamXPos);
		addAttribute(&m_emittedControlParamYPos);
		addAttribute(&m_emittedControlParamWidth);
		addAttribute(&m_emittedControlParamHeight);
		addAttribute(&m_outputPinXPos);
		addAttribute(&m_outputPinYPos);
		addAttribute(&m_outputPinWidth);
		addAttribute(&m_outputPinHeight);
	}
}
