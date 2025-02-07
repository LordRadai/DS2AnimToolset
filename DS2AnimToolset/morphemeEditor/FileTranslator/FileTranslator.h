#pragma once
#include "FBXTranslator/FBXTranslator.h"
#include "XMDTranslator/XMDTranslator.h"
#include "GLTFTranslator/GLTFTranslator.h"

namespace FT
{
	enum ExportFormat : char
	{
		kFbx,
		kXmd,
		kGltf,
		kNumExportFormats
	};

	class FileTranslator
	{
	public:
		FileTranslator() {}
		~FileTranslator() {}

		bool exportModel(Character* character, ExportFormat fileFormat);
		bool exportAnimation(Character* character, std::wstring path, int animSetIdx, int animIdx, int fps, bool includeMeshes, ExportFormat fileFormat);
	};
}