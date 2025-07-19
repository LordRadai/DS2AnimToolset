#pragma once
#include "RCore.h"
#include "../Request/Request.h"

namespace db
{
	class RequestPreset
	{
		Request* m_request;

	public:
		RequestPreset(Request* request)
			: m_request(request) {
		};
		~RequestPreset() {};

		Request* getRequest() const { return m_request; };
	};
}