#pragma once
#include "Pin.inl"

namespace mcd
{
	class FunctionalPin : public Pin
	{
		std::unique_ptr<db::StringArrayAttribute> m_interfaces;
		std::unique_ptr<db::BoolAttribute> m_passThroughEnabled;

	public:
		FunctionalPin(db::Node* parent, const std::string& name);

		virtual ~FunctionalPin() override {};

		void addInterface(const std::string& interfaceName) { m_interfaces->add(interfaceName); }
		void removeInterface(const uint32_t index) { m_interfaces->removeAt(index); }
		std::string getInterface(const uint32_t index) const { return m_interfaces->getElement(index); }
		std::string findInterface(const std::string& name) const;
		uint32_t getInterfaceCount() const { return m_interfaces->size(); }

		void setPassThroughEnabled(bool enabled);
		bool isPassThroughEnabled() const { return m_passThroughEnabled->getValue(); }
	};
}
