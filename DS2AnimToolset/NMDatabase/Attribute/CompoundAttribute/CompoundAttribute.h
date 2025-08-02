#pragma once
#include "../CompositeAttribute/CompositeAttribute.h"

namespace db
{
	class CompoundAttribute : public CompositeAttribute
	{
	public:
		CompoundAttribute(Attribute* parent, std::string name, std::string type) : 
			CompositeAttribute(parent, name, type) {};
		
		virtual ~CompoundAttribute() override {};
		virtual CompoundAttribute* asCompound() const override { return const_cast<CompoundAttribute*>(this); }
		virtual bool isCompound() const override { return true; }
		virtual bool writeValueXML(int format, SaverXML* saver) override;
	};
}
