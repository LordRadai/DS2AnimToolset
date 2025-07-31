#include "MMTransition.h"

namespace mcc
{
	void MMTransition::fromJson(const nlohmann::json& json)
	{
		m_jsonData = json;

		if (json.contains("interfaces") && json["interfaces"].is_array())
		{
			m_interfaces.clear();
			for (const auto& intr : json["interfaces"])
			{
				if (intr.is_string())
					m_interfaces.push_back(intr.get<std::string>());
			}
		}
	}

	std::string MMTransition::getInterface(uint32_t index) const
	{
		if (index < m_interfaces.size())
			return m_interfaces[index];

		throw std::out_of_range("Index out of range for interfaces.");
	}

	std::string MMTransition::findInterface(const std::string& name) const
	{
		for (const auto& intr : m_interfaces)
		{
			if (intr == name)
				return intr;
		}

		return "";
	}

	mcd::TransitionEdge* MMTransition::createDatabaseTransitionEdge(mcd::StateMachine* parent)
	{
		char transitionName[256];
		snprintf(transitionName, sizeof(transitionName), "%s%d", getName().c_str(), parent->getNumTransitionOfType(getName()) + 1);

		mcd::TransitionEdge* transition = new mcd::TransitionEdge(nullptr, transitionName, nullptr, nullptr, getName(), getVersion());

		parent->addTransitionEdge(transition);

		return transition;
	}
}