#pragma once
#include "CompositeAttribute.h"

namespace db
{
	class CompoundAttribute : public CompositeAttribute
	{
	public:
		CompoundAttribute(Attribute* parent, std::string name, std::string label, std::string type) : 
			CompositeAttribute(parent, name, label, type) {};
		
		virtual ~CompoundAttribute() override {};
		virtual bool writeValueXML(int format) override;
	};
}
