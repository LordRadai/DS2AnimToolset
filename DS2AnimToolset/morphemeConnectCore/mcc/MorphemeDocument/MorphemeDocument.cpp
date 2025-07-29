#include "MorphemeDocument.h"

namespace mcc
{
	bool MorphemeDocument::initializeManifest()
	{
		if (!std::filesystem::exists("Data\\manifest\\"))
			throw std::runtime_error("Manifest directory does not exist: Data\\manifest\\");

		if (!std::filesystem::exists("Data\\manifest\\nodes\\"))
			throw std::runtime_error("Manifest node directory does not exist: Data\\manifest\\nodes\\");

		for (const auto& entry : std::filesystem::recursive_directory_iterator("Data\\manifest\\nodes\\"))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".json")
			{
				std::string manifestPath = entry.path().string();
				m_manifest->registerNode(manifestPath);
			}
		}

		if (!std::filesystem::exists("Data\\manifest\\conditions\\"))
			throw std::runtime_error("Manifest condition directory does not exist: Data\\manifest\\conditions\\");

		for (const auto& entry : std::filesystem::recursive_directory_iterator("Data\\manifest\\conditions\\"))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".json")
			{
				std::string manifestPath = entry.path().string();
				m_manifest->registerCondition(manifestPath);
			}
		}

		if (!std::filesystem::exists("Data\\manifest\\transitions"))
			throw std::runtime_error("Manifest transition directory does not exist: Data\\manifest\\transitions\\");

		for (const auto& entry : std::filesystem::recursive_directory_iterator("Data\\manifest\\transitions\\"))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".json")
			{
				std::string manifestPath = entry.path().string();
				m_manifest->registerTransition(manifestPath);
			}
		}

		return true;
	}

	mcd::ControlParameter* MorphemeDocument::createControlParmeter(const std::string& name, DataTypes dataType)
	{
		mcd::ControlParametersNode* cpNode = m_morphemeDB->getNetwork()->getControlParametersNode();

		mcd::ControlParameter* cp = new mcd::ControlParameter(cpNode, name, dataType);
		cpNode->addAttribute(cp);

		return cp;
	}

	mcd::ControlParameter* MorphemeDocument::createFloatControlParameter(const std::string& name, float min, float max, float defaultValue)
	{
		mcd::ControlParameter* param = createControlParmeter(name, DataTypes::kFloat);
		param->m_min->setValue(min);
		param->m_max->setValue(max);
		param->m_defaultFloat->setValue(defaultValue);

		param->addAttribute(param->m_min.get());
		param->addAttribute(param->m_max.get());
		param->addAttribute(param->m_defaultFloat.get());

		return param;
	}

	mcd::ControlParameter* MorphemeDocument::createVector3ControlParameter(const std::string& name, float min, float max, const NMP::Vector3& defaultValue)
	{
		mcd::ControlParameter* param = createControlParmeter(name, DataTypes::kVector3);
		param->m_min->setValue(min);
		param->m_max->setValue(max);
		param->m_defaultVector3->setValue(defaultValue);

		param->addAttribute(param->m_min.get());
		param->addAttribute(param->m_max.get());
		param->addAttribute(param->m_defaultVector3.get());

		return param;
	}

	mcd::ControlParameter* MorphemeDocument::createVector4ControlParameter(const std::string& name, float min, float max, const NMP::Quat& defaultValue)
	{
		mcd::ControlParameter* param = createControlParmeter(name, DataTypes::kVector4);
		param->m_min->setValue(min);
		param->m_max->setValue(max);
		param->m_defaultQuaternion->setValue(defaultValue);

		param->addAttribute(param->m_min.get());
		param->addAttribute(param->m_max.get());
		param->addAttribute(param->m_defaultQuaternion.get());

		return param;
	}

	mcd::ControlParameter* MorphemeDocument::createBoolControlParameter(const std::string& name, bool defaultValue)
	{
		mcd::ControlParameter* param = createControlParmeter(name, DataTypes::kBool);
		param->m_defaultBool->setValue(defaultValue);

		param->addAttribute(param->m_defaultBool.get());

		return param;
	}

	mcd::ControlParameter* MorphemeDocument::createQuaternionControlParameter(const std::string& name, float min, float max, const NMP::Quat& defaultValue)
	{
		mcd::ControlParameter* param = createControlParmeter(name, DataTypes::kQuaternion);
		param->m_defaultQuaternion->setValue(defaultValue);
		param->m_min->setValue(min);
		param->m_max->setValue(max);

		param->addAttribute(param->m_min.get());
		param->addAttribute(param->m_max.get());
		param->addAttribute(param->m_defaultQuaternion.get());

		return param;
	}

	mcd::ControlParameter* MorphemeDocument::createIntControlParameter(const std::string& name, int min, int max, int defaultValue)
	{
		mcd::ControlParameter* param = createControlParmeter(name, DataTypes::kInt);
		param->m_minInt->setValue(min);
		param->m_maxInt->setValue(max);
		param->m_defaultInt->setValue(defaultValue);

		param->addAttribute(param->m_minInt.get());
		param->addAttribute(param->m_maxInt.get());
		param->addAttribute(param->m_defaultInt.get());

		return param;
	}

	mcd::ControlParameter* MorphemeDocument::createUIntControlParameter(const std::string& name, uint32_t min, uint32_t max, uint32_t defaultValue)
	{
		mcd::ControlParameter* param = createControlParmeter(name, DataTypes::kUInt);
		param->m_minInt->setValue(min);
		param->m_maxInt->setValue(max);
		param->m_defaultInt->setValue(defaultValue);

		param->addAttribute(param->m_minInt.get());
		param->addAttribute(param->m_maxInt.get());
		param->addAttribute(param->m_defaultInt.get());

		return param;
	}

	void MorphemeDocument::save()
	{
		if (m_filepath.empty())
			throw std::runtime_error("Filepath is not set. Please set the filepath before saving.");
		if (!m_morphemeDB)
			throw std::runtime_error("MorphemeDB is not initialized.");

		m_morphemeDB->exportXML(m_filepath.c_str());
	}

	void MorphemeDocument::saveAs(const std::string& filename)
	{
		m_filepath = filename;

		save();
	}
}