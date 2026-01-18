#pragma once
#include "Pin.h"

namespace mcd
{
	class Graph;

	class PassDownPin : public Pin
	{
	public:
		PassDownPin(db::Node* parent, const std::string& name);

		mcd::Graph* getChildGraph();
	};
}