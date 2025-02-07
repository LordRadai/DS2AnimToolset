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

		/**
		* @brief Creates a model file.
		* @param character: The input character from which to generate the file.
		* @param format: The export format.
		* @return Status of the export process.
		*/
		bool exportModel(Character* character, ExportFormat format);

		/**
		* @brief Creates an animation file.
		* @param character: The input character from which to generate the file.
		* @param path: The base export folder. THIS IS NOT THE FILE NAME, the actual file name will be generated in the function.
		* @param animSetIdx: The animation set index from where to pull animation data and filename from.
		* @param animIdx: The index of the animation within the specified animation set.
		* @param fps: The framerate at which to sample the animation.
		* @param includeMeshes: Whether or not to include model meshes in the exported anim file.
		* @param format: The export format.
		* @return Status of the export process.
		*/
		bool exportAnimation(Character* character, std::wstring path, int animSetIdx, int animIdx, int fps, bool includeMeshes, ExportFormat fileFormat);
	};
}