#pragma once
#include <stdint.h>

namespace db
{
	class Registry
	{
		static Registry* g_Registry;

		bool m_enableRuntimeIDGeneration;
		uint32_t m_nextRuntimeID;
		uint32_t m_nextNodeID;
		uint32_t m_nextTypeID;

		Registry();
		~Registry();
	public:
		static Registry* getInstance();
		void shutdown();

		int getNewRuntimeID();
		int getNewNodeID();
		int getNewTypeID();

		void enableRuntimeIDGeneration(bool enable) { m_enableRuntimeIDGeneration = enable; }
	};
}
