#pragma once
#include <Windows.h>
#include <string>
#include "../Character/Character.h"

namespace FT
{
	class FileTranslatorBase
	{
	public:
		FileTranslatorBase() {}
		~FileTranslatorBase() {}

		/**
		* @brief Creates a model file.
		* @param character: The input character from which to generate the file.
		* @return Status of the export process.
		*/
		virtual bool exportModel(Character* character) = 0;

		/**
		 * @brief Creates an animation file.
		 * @param character: The input character from which to generate the file.
		 * @param path: The base export folder. THIS IS NOT THE FILE NAME, the actual file name will be generated in the function.
		 * @param animSetIdx: The animation set index from where to pull animation data and filename from.
		 * @param animIdx: The index of the animation within the specified animation set.
		 * @param fps: The framerate at which to sample the animation.
		 * @param includeMeshes: Whether or not to include model meshes in the exported anim file.
		 * @return Status of the export process.
		 */
		virtual bool exportAnimation(Character* character, std::wstring path, int animSetIdx, int animIdx, int fps, bool includeMeshes) = 0;
	};
}