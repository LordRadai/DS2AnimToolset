#include "MorphemeDocument.h"

namespace mcc
{
	mcd::ControlParameter* MorphemeDocument::createControlParmeter(const std::string& name, DataTypes dataType)
	{
		mcd::ControlParameter* cp = new mcd::ControlParameter(&m_morphemeDB, name);
		m_morphemeDB.addAttribute(cp);

		return cp;
	}

	mcd::ControlParameter* MorphemeDocument::createFloatControlParameter(const std::string& name, float min, float max, float defaultValue)
	{
		mcd::ControlParameter* param = createControlParmeter(name, DataTypes::kFloat);
		param->m_min.setValue(min);
		param->m_max.setValue(max);
		param->m_defaultFloat.setValue(defaultValue);

		param->addAttribute(&param->m_min);
		param->addAttribute(&param->m_max);
		param->addAttribute(&param->m_defaultFloat);

		return param;
	}

	mcd::ControlParameter* MorphemeDocument::createVector3ControlParameter(const std::string& name, float min, float max, const NMP::Vector3& defaultValue)
	{
		mcd::ControlParameter* param = createControlParmeter(name, DataTypes::kVector3);
		param->m_min.setValue(min);
		param->m_max.setValue(max);
		param->m_defaultVector3.setValue(defaultValue);

		param->addAttribute(&param->m_min);
		param->addAttribute(&param->m_max);
		param->addAttribute(&param->m_defaultVector3);

		return param;
	}

	mcd::ControlParameter* MorphemeDocument::createVector4ControlParameter(const std::string& name, float min, float max, const NMP::Quat& defaultValue)
	{
		mcd::ControlParameter* param = createControlParmeter(name, DataTypes::kVector4);
		param->m_min.setValue(min);
		param->m_max.setValue(max);
		param->m_defaultQuaternion.setValue(defaultValue);

		param->addAttribute(&param->m_min);
		param->addAttribute(&param->m_max);
		param->addAttribute(&param->m_defaultQuaternion);

		return param;
	}

	mcd::ControlParameter* MorphemeDocument::createBoolControlParameter(const std::string& name, bool defaultValue)
	{
		mcd::ControlParameter* param = createControlParmeter(name, DataTypes::kBool);
		param->m_defaultBool.setValue(defaultValue);

		param->addAttribute(&param->m_defaultBool);

		return param;
	}

	mcd::ControlParameter* MorphemeDocument::createQuaternionControlParameter(const std::string& name, float min, float max, const NMP::Quat& defaultValue)
	{
		mcd::ControlParameter* param = createControlParmeter(name, DataTypes::kQuaternion);
		param->m_defaultQuaternion.setValue(defaultValue);
		param->m_min.setValue(min);
		param->m_max.setValue(max);

		param->addAttribute(&param->m_min);
		param->addAttribute(&param->m_max);
		param->addAttribute(&param->m_defaultQuaternion);

		return param;
	}

	mcd::ControlParameter* MorphemeDocument::createIntControlParameter(const std::string& name, int min, int max, int defaultValue)
	{
		mcd::ControlParameter* param = createControlParmeter(name, DataTypes::kInt);
		param->m_minInt.setValue(min);
		param->m_maxInt.setValue(max);
		param->m_defaultInt.setValue(defaultValue);

		param->addAttribute(&param->m_minInt);
		param->addAttribute(&param->m_maxInt);
		param->addAttribute(&param->m_defaultInt);

		return param;
	}

	mcd::ControlParameter* MorphemeDocument::createUIntControlParameter(const std::string& name, uint32_t min, uint32_t max, uint32_t defaultValue)
	{
		mcd::ControlParameter* param = createControlParmeter(name, DataTypes::kUInt);
		param->m_minInt.setValue(min);
		param->m_maxInt.setValue(max);
		param->m_defaultInt.setValue(defaultValue);

		param->addAttribute(&param->m_minInt);
		param->addAttribute(&param->m_maxInt);
		param->addAttribute(&param->m_defaultInt);

		return param;
	}

	void MorphemeDocument::saveAs(const std::string& filename)
	{
		m_morphemeDB.exportXML(filename.c_str());
	}
}