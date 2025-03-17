#include "FileTranslator.h"

namespace FT
{
	bool FileTranslator::exportModel(Character* character, ExportFormat format)
	{
		FileTranslatorBase* fileTranslator = nullptr;

		switch (format)
		{
		case kFbx:
			fileTranslator = new FbxFileTranslator();
			break;
		case kXmd:
			fileTranslator = new XMDFileTranslator();
			break;
		case kGltf:
			fileTranslator = new GltfFileTranslator();
			break;
		default:
			throw std::runtime_error("Invalid export format");
		}

		bool status = fileTranslator->exportModel(character);

		delete fileTranslator;

		return status;
	}

	bool FileTranslator::exportAnimation(Character* character, std::wstring path, int animSetIdx, int animIdx, int fps, bool includeMeshes, ExportFormat format)
	{
		FileTranslatorBase* fileTranslator = nullptr;

		switch (format)
		{
		case kFbx:
			fileTranslator = new FbxFileTranslator();
			break;
		case kXmd:
			fileTranslator = new XMDFileTranslator();
			break;
		case kGltf:
			fileTranslator = new GltfFileTranslator();
			break;
		default:
			throw std::runtime_error("Invalid export format");
		}

		bool status = fileTranslator->exportAnimation(character, path, animSetIdx, animIdx, fps, includeMeshes);

		delete fileTranslator;

		return status;
	}
}