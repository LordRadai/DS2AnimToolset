#include "validate.h"

namespace cfr
{
	void assertMsg(int result, std::string message)
	{
		if(result != 0)
		{
			printf("%c",message.c_str());

			throw std::runtime_error("Failed assertMsg()!\n");
		}
	};
};