#include "MorphemeDocument.h"
#include "mcd/Pin/PassDownPin.h"
#include "mcd/Pin/FunctionalPin.h"

namespace mcc
{
	mcd::ControlParameter* MorphemeDocument::createControlParmeter(const std::string& name, mcd::DataPin::DataType dataType)
	{
		mcd::ControlParametersNode* cpNode = m_morphemeDB->getNetwork()->getControlParametersNode();

		mcd::ControlParameter* cp = new mcd::ControlParameter(cpNode, name, dataType);
		cpNode->addAttribute(cp);

		return cp;
	}

	mcd::ControlParameter* MorphemeDocument::createFloatControlParameter(const std::string& name, float min, float max, float defaultValue)
	{
		mcd::ControlParameter* param = createControlParmeter(name, mcd::DataPin::DataType::kFloat);
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
		mcd::ControlParameter* param = createControlParmeter(name, mcd::DataPin::DataType::kVector3);
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
		mcd::ControlParameter* param = createControlParmeter(name, mcd::DataPin::DataType::kVector4);
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
		mcd::ControlParameter* param = createControlParmeter(name, mcd::DataPin::DataType::kBool);
		param->m_defaultBool->setValue(defaultValue);

		param->addAttribute(param->m_defaultBool.get());

		return param;
	}

	mcd::ControlParameter* MorphemeDocument::createQuaternionControlParameter(const std::string& name, float min, float max, const NMP::Quat& defaultValue)
	{
		mcd::ControlParameter* param = createControlParmeter(name, mcd::DataPin::DataType::kQuaternion);
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
		mcd::ControlParameter* param = createControlParmeter(name, mcd::DataPin::DataType::kInt);
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
		mcd::ControlParameter* param = createControlParmeter(name, mcd::DataPin::DataType::kUInt);
		param->m_minInt->setValue(min);
		param->m_maxInt->setValue(max);
		param->m_defaultInt->setValue(defaultValue);

		param->addAttribute(param->m_minInt.get());
		param->addAttribute(param->m_maxInt.get());
		param->addAttribute(param->m_defaultInt.get());

		return param;
	}

	mcd::BlendTreeNode* MorphemeDocument::createBlendTreeNode(mcc::MMNode* manifestNode, mcd::BlendTree* parent, const std::string& name, float xPos, float yPos)
	{
		mcd::BlendTreeNode* btNode = manifestNode->createDatabaseNode(parent, m_morphemeDB.get());

		if (!name.empty())
			btNode->setName(name);

		btNode->setXPos(xPos);
		btNode->setYPos(yPos);

		return btNode;
	}

	mcd::BlendTreeNode* MorphemeDocument::createBlendTreeNode(mcc::MMNode* manifestNode, mcd::BlendTree* parent, const std::string& name)
	{
		if (manifestNode == nullptr)
			throw std::runtime_error("Cannot create BlendTreeNode. Manifest node is nullptr.");

		float xPos = 0.f;
		float yPos = 0.f;
		parent->getFreePosition(xPos, yPos);

		return createBlendTreeNode(manifestNode, parent, name, xPos, yPos);
	}

	mcd::StateMachineNode* MorphemeDocument::createStateMachineNode(mcc::MMStateMachineNode* manifestNode, mcd::Graph* parent, const std::string& name, float xPos, float yPos)
	{
		if (!parent->isOfType<mcd::StateMachine>())
			return nullptr;

		mcd::StateMachine* smParent = dynamic_cast<mcd::StateMachine*>(parent);
		mcd::StateMachineNode* smNode = manifestNode->createStateMachineNode("StateMachineNode");

		if (smNode != nullptr)
		{
			if (!name.empty())
				smNode->setName(name);

			smNode->setXPos(xPos);
			smNode->setYPos(yPos);
			smParent->addStateMachineNode(smNode);

			return smNode;
		}

		return nullptr;
	}

	mcd::StateMachineNode* MorphemeDocument::createStateMachineNode(mcc::MMStateMachineNode* manifestNode, mcd::Graph* parent, const std::string& name)
	{
		if (manifestNode == nullptr)
			throw std::runtime_error("Cannot create StateMachineNode. Manifest node is nullptr.");

		float xPos = 0.f;
		float yPos = 0.f;

		if (parent->isOfType<mcd::BlendTree>())
			dynamic_cast<mcd::BlendTree*>(parent)->getFreePosition(xPos, yPos);
		else if (parent->isOfType<mcd::StateMachine>())
			dynamic_cast<mcd::StateMachine*>(parent)->getFreePosition(xPos, yPos);

		return createStateMachineNode(manifestNode, parent, name, xPos, yPos);
	}

	mcd::StateMachineNode* MorphemeDocument::createNewBlendTree(const std::string& name, mcd::StateMachine* parent, float xPos, float yPos)
	{
		mcd::BlendTree* bt = new mcd::BlendTree(parent, name);

		mcd::StateMachineNode* newNode = new mcd::StateMachineNode(parent, name, xPos, yPos, 100.f, 50.f);
		newNode->setGraphEntry(bt);
		newNode->addPin(new mcd::PassDownPin(newNode, "Result", false));

		parent->addStateMachineNode(newNode);

		return newNode;
	}

	mcd::BlendTreeNode* MorphemeDocument::createNewBlendTree(const std::string& name, mcd::BlendTree* parent, float xPos, float yPos)
	{
		mcd::BlendTree* bt = new mcd::BlendTree(parent, name);

		mcd::BlendTreeNode* newNode = new mcd::BlendTreeNode(parent, name, xPos, yPos, 100.f, 50.f, "BlendTree", -1);
		newNode->setGraphEntry(bt);
		newNode->addPin(new mcd::PassDownPin(newNode, "Result", false));

		parent->addBlendTreeNode(newNode);

		return newNode;
	}

	mcd::StateMachineNode* MorphemeDocument::createNewBlendTree(const std::string& name, mcd::StateMachine* parent)
	{
		float xPos = 0.f;
		float yPos = 0.f;
		parent->getFreePosition(xPos, yPos);

		return createNewBlendTree(name, parent, xPos, yPos);
	}

	mcd::BlendTreeNode* MorphemeDocument::createNewBlendTree(const std::string& name, mcd::BlendTree* parent)
	{
		float xPos = 0.f;
		float yPos = 0.f;
		parent->getFreePosition(xPos, yPos);

		return createNewBlendTree(name, parent, xPos, yPos);
	}

	mcd::GraphNode* MorphemeDocument::createNewStateMachine(const std::string& name, mcc::MMStateMachine* manifestSM, mcd::Graph* parent, float xPos, float yPos)
	{
		mcd::StateMachine* sm = new mcd::StateMachine(parent, name, manifestSM->getName(), manifestSM->getVersion());

		if (parent->isOfType<mcd::StateMachine>())
		{
			mcd::StateMachine* parentSM = dynamic_cast<mcd::StateMachine*>(parent);

			mcd::StateMachineNode* newNode = new mcd::StateMachineNode(parent, name, xPos, yPos, 100.f, 50.f, manifestSM->getName(), manifestSM->getVersion());
			newNode->setGraphEntry(sm);

			parentSM->addStateMachineNode(newNode);

			return newNode;
		}
		else if (parent->isOfType<mcd::BlendTree>())
		{
			mcd::BlendTree* parentBT = dynamic_cast<mcd::BlendTree*>(parent);

			mcd::BlendTreeNode* newNode = new mcd::BlendTreeNode(parent, name, xPos, yPos, 100.f, 50.f, manifestSM->getName(), manifestSM->getVersion());
			newNode->setGraphEntry(sm);

			parentBT->addBlendTreeNode(newNode);

			return newNode;
		}

		throw std::invalid_argument("Parent graph must be either a StateMachine or a BlendTree.");
	}

	mcd::GraphNode* MorphemeDocument::createNewStateMachine(const std::string& name, mcc::MMStateMachine* manifestSM, mcd::Graph* parent)
	{
		float xPos = 0.f;
		float yPos = 0.f;

		if (parent->isOfType<mcd::BlendTree>())
			dynamic_cast<mcd::BlendTree*>(parent)->getFreePosition(xPos, yPos);
		else if (parent->isOfType<mcd::StateMachine>())
			dynamic_cast<mcd::StateMachine*>(parent)->getFreePosition(xPos, yPos);

		return createNewStateMachine(name, manifestSM, parent, xPos, yPos);
	}

	void MorphemeDocument::setDefaultName(db::Node* node, db::Node* parent)
	{
	}

	mcd::Request* MorphemeDocument::createRequest(const std::string& name)
	{
		mcd::Network* network = m_morphemeDB->getNetwork();

		return new mcd::Request(network, name, network->getNumRequests());
	}

	void MorphemeDocument::save()
	{
		if (m_filepath.empty())
			throw std::runtime_error("Filepath is not set. Please set the filepath before saving.");

		if (!m_morphemeDB)
			throw std::runtime_error("MorphemeDB is not initialized.");

		db::SaverXML saver(fopen(m_filepath.c_str(), "w"), "ConnectNetwork");

		saver.save(m_morphemeDB.get());
	}

	void MorphemeDocument::saveAs(const std::string& filename)
	{
		m_filepath = filename;

		save();
	}
}