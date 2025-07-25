#pragma once
#include "mcd/MorphemeDB/MorphemeDB.h"
#include "mcd/ControlParameter/ControlParameter.h"

namespace mcc
{
	class MorphemeDocument
	{
		mcd::MorphemeDB m_morphemeDB;
	public:
		MorphemeDocument() {}
		~MorphemeDocument() {}
		
		/**
		 * \brief Creates a typed control parameter. This is unfinished, the parent is set to nullptr for now.
		 * 
		 * \param name Control parameter name.
		 * \param dataType The type of the control parameter.
		 * \return A pointer to the created control parameter.
		 */
		mcd::ControlParameter* createControlParmeter(const std::string& name, DataTypes dataType);

		/**
		 * \brief Creates a float control parameter with specified min, max, and default value.
		 * 
		 * \param name Control parameter name.
		 * \param min Minimum value for the control parameter.
		 * \param max Maximum value for the control parameter.
		 * \param defaultValue Default value for the control parameter.
		 * \return A pointer to the created float control parameter.
		 */
		mcd::ControlParameter* createFloatControlParameter(const std::string& name, float min, float max, float defaultValue);

		/**
		 * \brief Creates a vector3 control parameter with specified min, max, and default value.
		 * 
		 * \param name Control parameter name.
		 * \param min Minimum value for the control parameter.
		 * \param max Maximum value for the control parameter.
		 * \param defaultValue Default value for the control parameter.
		 * \return A pointer to the created vector3 control parameter.
		 */
		mcd::ControlParameter* createVector3ControlParameter(const std::string& name, float min, float max, const NMP::Vector3& defaultValue);

		/**
		 * \brief Creates a vector4 control parameter with specified min, max, and default value.
		 * 
		 * \param name Control parameter name.
		 * \param min Minimum value for the control parameter.
		 * \param max Maximum value for the control parameter.
		 * \param defaultValue Default value for the control parameter.
		 * \return A pointer to the created vector4 control parameter.
		 */
		mcd::ControlParameter* createVector4ControlParameter(const std::string& name, float min, float max, const NMP::Quat& defaultValue);

		/**
		 * \brief Creates a boolean control parameter with a default value.
		 * 
		 * \param name Control parameter name.
		 * \param defaultValue Default value for the control parameter.
		 * \return A pointer to the created boolean control parameter.
		 */
		mcd::ControlParameter* createBoolControlParameter(const std::string& name, bool defaultValue);

		/**
		 * \brief Creates a quaternion control parameter with a default value.
		 * 
		 * \param name Control parameter name.
		 * \param defaultValue Default value for the control parameter.
		 * \param min Minimum value for the control parameter.
		 * \param max Maximum value for the control parameter.
		 * \return A pointer to the created quaternion control parameter.
		 */
		mcd::ControlParameter* createQuaternionControlParameter(const std::string& name, float min, float max, const NMP::Quat& defaultValue);

		/**
		 * \brief Creates an integer control parameter with specified min, max, and default value.
		 * 
		 * \param name Control parameter name.
		 * \param min Minimum value for the control parameter.
		 * \param max Maximum value for the control parameter.
		 * \param defaultValue Default value for the control parameter.
		 * \return A pointer to the created integer control parameter.
		 */
		mcd::ControlParameter* createIntControlParameter(const std::string& name, int min, int max, int defaultValue);

		/**
		 * \brief Creates an unsigned integer control parameter with specified min, max, and default value.
		 * 
		 * \param name Control parameter name.
		 * \param min Minimum value for the control parameter.
		 * \param max Maximum value for the control parameter.
		 * \param defaultValue Default value for the control parameter.
		 * \return A pointer to the created unsigned integer control parameter.
		 */
		mcd::ControlParameter* createUIntControlParameter(const std::string& name, uint32_t min, uint32_t max, uint32_t defaultValue);

		void saveAs(const std::string& filename);
	};
}
