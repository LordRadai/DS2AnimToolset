#pragma once
#include "mcd/MorphemeDB/MorphemeDB.h"
#include "mcd/ControlParameter/ControlParameter.h"
#include "mcd/Request/Request.h"
#include "mcc/MorphemeManifest/MorphemeManifest.h"
#include "mcd/BlendTree/BlendTree.h"
#include "mcd/StateMachine/StateMachine.h"
#include "mcc/MorphemeManifest/MMManifestItem/StateMachine/MMStateMachine.h"
#include "mcd/Pin/DataPin.h"

namespace mcc
{
	class MorphemeDocument
	{
		std::unique_ptr<mcd::MorphemeDB> m_morphemeDB;
		std::unique_ptr<mcc::MorphemeManifest> m_manifest;
		std::string m_filepath;
		std::string m_activeAnimSetName;

	public:
		MorphemeDocument() : m_morphemeDB(std::make_unique<mcd::MorphemeDB>()), m_manifest(std::make_unique<mcc::MorphemeManifest>()) {}
		~MorphemeDocument() {}

		/**
		 * \brief Returns the MorphemeDB instance.
		 *
		 * \return A reference to the MorphemeDB instance.
		 */
		mcd::MorphemeDB* getMorphemeDB() { return m_morphemeDB.get(); }

		/**
		 * \brief Returns the MorphemeManifest instance.
		 *
		 * \return A pointer to the MorphemeManifest instance.
		 */
		mcc::MorphemeManifest* getManifest() { return m_manifest.get(); }

		/**
		 * \brief Gets the active animation set name.
		**/
		std::string getActiveAnimationSetName() { return m_activeAnimSetName; }

		/**
		 * \brief Creates a typed control parameter. This is unfinished, the parent is set to nullptr for now.
		 * 
		 * \param name Control parameter name.
		 * \param dataType The type of the control parameter.
		 * \return A pointer to the created control parameter.
		 */
		mcd::ControlParameter* createControlParmeter(const std::string& name, mcd::DataPin::DataType dataType);

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

		/**
		 * \brief Creates a blend tree node at the specified x and y coordinates.
		 * 
		 * \param manifestNode The manifest node to create the blend tree node from.
		 * \param parent The parent blend tree for the new node.
		 * \param name The name of the new blend tree node. If left empty, it will use the default name from the manifest.
		 * \param xPos The x position of the new blend tree node.
		 * \param yPos The y position of the new blend tree node.
		 * \return A pointer to the created blend tree node.
		 */
		mcd::BlendTreeNode* createBlendTreeNode(mcc::MMNode* manifestNode, mcd::BlendTree* parent, const std::string& name, float xPos, float yPos);

		/**
		 * \brief Creates a blend tree node at the first free position under the parent blend tree.
		 * 
		 * \param manifestNode The manifest node to create the blend tree node from.
		 * \param parent The parent blend tree for the new node.
		 * \param name The name of the new blend tree node.
		 * \return A pointer to the created blend tree node.
		 */
		mcd::BlendTreeNode* createBlendTreeNode(mcc::MMNode* manifestNode, mcd::BlendTree* parent, const std::string& name);

		/**
		 * \brief Creates a new blend tree with the specified name and parent state machine.
		 * 
		 * \param name The name of the new blend tree.
		 * \param parent The parent state machine for the new blend tree.
		 * \param xpos The x position of the new blend tree.
		 * \param ypos The y position of the new blend tree.
		 * \return A pointer to the created blend tree.
		 */
		mcd::StateMachineNode* createNewBlendTree(const std::string& name, mcd::StateMachine* parent, float xpos, float ypos);

		/**
		 * \brief Creates a new blend tree with the specified name and parent blend tree.
		 * 
		 * \param name The name of the new blend tree.
		 * \param parent The parent blend tree for the new blend tree.
		 * \param xpos The x position of the new blend tree.
		 * \param ypos The y position of the new blend tree.
		 * \return A pointer to the created blend tree.
		 */
		mcd::BlendTreeNode* createNewBlendTree(const std::string& name, mcd::BlendTree* parent, float xpos, float ypos);

		/**
		 * \brief Creates a new blend tree with the specified name and parent state machine at the first free position.
		 * 
		 * \param name The name of the new blend tree.
		 * \param parent The parent state machine for the new blend tree.
		 * \return A pointer to the created blend tree.
		 */
		mcd::StateMachineNode* createNewBlendTree(const std::string& name, mcd::StateMachine* parent);

		/**
		 * \brief Creates a new blend tree with the specified name and parent blend tree at the first free position.
		 * 
		 * \param name The name of the new blend tree.
		 * \param parent The parent blend tree for the new blend tree.
		 * \return A pointer to the created blend tree.
		 */
		mcd::BlendTreeNode* createNewBlendTree(const std::string& name, mcd::BlendTree* parent);

		/**
		 * \brief Creates a new state machine with the specified name and parent graph.
		 * 
		 * \param name The name of the new state machine.
		 * \param manifestSM The manifest state machine to create the state machine from.
		 * \param parent The parent graph for the new state machine.
		 * \param xpos The x position of the new state machine.
		 * \param ypos The y position of the new state machine.
		 * \return A pointer to the created state machine.
		 */
		mcd::GraphNode* createNewStateMachine(const std::string& name, mcc::MMStateMachine* manifestSM, mcd::Graph* parent, float xpos, float ypos);

		/**
		 * \brief Creates a new state machine with the specified name and parent graph at the first free position.
		 * 
		 * \param name The name of the new state machine.
		 * \param manifestSM The manifest state machine to create the state machine from.
		 * \param parent The parent graph for the new state machine.
		 * \return A pointer to the created state machine.
		 */
		mcd::GraphNode* createNewStateMachine(const std::string& name, mcc::MMStateMachine* manifestSM, mcd::Graph* parent);

		mcd::Request* createRequest(const std::string& name);

		void save();

		void saveAs(const std::string& filename);
	};
}
