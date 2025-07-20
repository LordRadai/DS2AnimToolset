#pragma once
#include "RCore.h"
#include "../Request/Request.h"
#include "../Node/Node.h"
#include "../NodeContainer/NodeContainer.h"

namespace db
{
	typedef std::string AddedManifestPreset;

	class RequestPreset : public Node
	{
		Request m_request;
		std::vector<AddedManifestPreset> m_addedManifestPresets;

	public:
		RequestPreset(Node* parent, Request request) : Node(parent, "RequestPresets"), m_request(request) {};
		~RequestPreset() {};

		Request getRequest() const { return m_request; };
		std::string getManfiestPreset(int index);
	};
}