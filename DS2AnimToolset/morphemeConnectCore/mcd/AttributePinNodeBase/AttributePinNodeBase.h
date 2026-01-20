#pragma once
#include "NMDatabase/NMDatabase.h"

namespace mcd
{
	class MorphemeDB;
	class Network;

	class Pin;
	class FunctionalPin;
	class Attribute;

	class AttributePinNodeBase : public db::Node
	{
	public:
		AttributePinNodeBase(db::CompositeAttribute* parent, const std::string& name, const std::string nodeName)
			: db::Node(parent, name, nodeName) {};
		
		virtual ~AttributePinNodeBase() override {};
		virtual int getMorphemeAttributeCount() const { return 0; }
		virtual mcd::Attribute* getMorphemeAttribute(int idx) { return nullptr; }
		virtual mcd::Attribute* getMorphemeAttribute(const std::string& name) { return nullptr; }
		virtual int getPinCount() const { return 0; }
		virtual mcd::Pin* getPin(int idx) { return nullptr; }
		virtual mcd::Pin* getPin(const std::string& name) { return nullptr; }
		virtual void getAllPins(std::vector<mcd::Pin*>& outPins) {};
		virtual std::vector<db::Node> getConnectedObjects(bool param_1, bool param_2);
		virtual std::vector<db::Node> getDirectlyConnectedObjects(bool param_1, bool param_2);
		virtual bool getOutputFunctionalPin(FunctionalPin** out);

		mcd::MorphemeDB* getMorphemeDatabase();
		mcd::Network* getMorphemeNetwork();
	};
}
