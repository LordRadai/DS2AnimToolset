#pragma once
#include "RCore.h"

namespace db
{
	class Pin
	{
		std::string m_name;

	public:
		Pin(std::string name)
			: m_name(name) {
		};
		~Pin() {};

		std::string getName() const { return m_name; };
	};

	class PassDownPin : public Pin
	{
		bool m_bReference;

	public:
		PassDownPin(std::string name, bool bReference = true)
			: Pin(name), m_bReference(bReference) {
		};
		~PassDownPin() {};

		bool isReference() const { return m_bReference; };
		void setReference(bool bReference) { m_bReference = bReference; };
	};

	class FunctionalPin : public Pin
	{
		std::vector<std::string> m_interfaces;

	public:
		FunctionalPin(std::string name)
			: Pin(name){
		};

		~FunctionalPin() {};

		std::string getInterface(int index) const;
		void addInterface(const std::string& interf);
		void removeInterface(int index);

		size_t getNumInterfaces() const { return m_interfaces.size(); }
	};
}