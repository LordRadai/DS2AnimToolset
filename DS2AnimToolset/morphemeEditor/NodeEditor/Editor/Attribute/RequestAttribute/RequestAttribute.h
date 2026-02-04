#pragma once
#include "../Attribute.h"
#include "NodeEditor/Editor/Request/Request.h"

namespace NodeEditor
{
	class RequestAttribute : public Attribute
	{
		Message* m_request;
	public:
		RequestAttribute(Entity* owner, const std::string& name) : Attribute(owner, name, "request"), m_request(nullptr) {}

		virtual ~RequestAttribute() override {}
		virtual bool editorGUI() override;
		virtual void setValue(const std::vector<std::any>& values) override;

		Message* getRequest() const { return m_request; }
	};
}
