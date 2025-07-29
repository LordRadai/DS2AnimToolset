#pragma once
#include "MMManifestItem/MMManifestItemBase.h"

namespace mcc
{
	class MorphemeManifest
	{
		static MorphemeManifest* s_instance;

		MorphemeManifest() = default;
		~MorphemeManifest() = default;
		MorphemeManifest(const MorphemeManifest&) = delete;
		MorphemeManifest& operator=(const MorphemeManifest&) = delete;
		MorphemeManifest(MorphemeManifest&&) = delete;
		MorphemeManifest& operator=(MorphemeManifest&&) = delete;

	public:
		static MorphemeManifest* getInstance()
		{
			if (!s_instance)
				s_instance = new MorphemeManifest();

			return s_instance;
		}

		static void destroyInstance()
		{
			if (s_instance)
				delete s_instance;

			s_instance = nullptr;
		}
	};
}
