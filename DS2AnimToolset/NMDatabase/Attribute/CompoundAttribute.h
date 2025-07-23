#pragma once
#include "CompositeAttribute.h"

namespace db
{
	class CompoundAttribute : public CompositeAttribute
	{
	public:
		CompoundAttribute(Attribute* parent, std::string name, std::string type) : 
			CompositeAttribute(parent, name, type) {};
		
		virtual ~CompoundAttribute() override {};
		virtual bool writeValueXML(int format) override;
	};
}
