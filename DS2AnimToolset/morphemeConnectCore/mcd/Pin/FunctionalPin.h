#pragma once
#include "Pin.h"

namespace mcd
{
	class FunctionalPin : public Pin
	{
		std::unique_ptr<db::StringArrayAttribute> m_interfaces;
		std::unique_ptr<db::BoolAttribute> m_passThroughEnabled;

	public:
		FunctionalPin(db::Node* parent, const std::string& name);

		virtual ~FunctionalPin() override {};
		virtual bool isCompatibleConnectionTarget(Pin* to) override;

		void addInterface(const std::string& interfaceName);
		void removeInterface(const uint32_t index);
		std::string getInterface(const uint32_t index) const { return m_interfaces->getElement(index); }
		std::string findInterface(const std::string& name) const;
		uint32_t getInterfaceCount() const { return m_interfaces->size(); }

		void setPassThroughEnabled(bool enabled);
		bool isPassThroughEnabled() const { return m_passThroughEnabled->getValue(); }

		bool dfsHasUpstreamKnownInterfaces();
	};
}
