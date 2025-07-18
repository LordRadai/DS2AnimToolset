#pragma once
#include <vector>
#include <string>

namespace MCN
{
	namespace Node
	{
		class NodePin
		{
		protected:
			bool m_isInput = false;

		public:
			NodePin(bool isInput = false) : m_isInput(isInput) {}
			virtual ~NodePin() {}

			bool isInput() const { return m_isInput; }
			void setInput(bool isInput) { m_isInput = isInput; }
		};

		class FunctionalPin : public NodePin
		{
			std::vector<std::string> m_interfaces;
			bool m_passThrough = true;

		public:
			FunctionalPin() = default;
			FunctionalPin(const std::vector<std::string>& interfaces, bool bPassThrough) : m_interfaces(interfaces), m_passThrough(bPassThrough) {}

			const std::string getInterface(size_t index) const;

			void setPassThrough(bool passThrough) { m_passThrough = passThrough; }
			bool isPassThrough() const { return m_passThrough; }

			void addInterface(const std::string& interface) { m_interfaces.push_back(interface); }
		};

		class DataPin : public NodePin
		{
			std::string m_type;

		public:
			DataPin(const std::string& type) : m_type(type) {}

			const std::string& getType() const { return m_type; }
		};
	}
}