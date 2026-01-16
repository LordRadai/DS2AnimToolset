#include "MorphemeDocument.h"
#include "mcd/Pin/PassDownPin.h"

namespace mcc
{
	bool MorphemeDocument::initializeManifest()
	{
		if (!std::filesystem::exists("Data\\manifest\\"))
			throw std::runtime_error("Manifest directory does not exist: Data\\manifest\\");

		if (!std::filesystem::exists("Data\\manifest\\nodes\\"))
			throw std::runtime_error("Manifest node directory does not exist: Data\\manifest\\nodes\\");

		m_manifest->registerStateMachine("Data\\manifest\\nodes\\animation\\StateMachine.json");

		for (const auto& entry : std::filesystem::recursive_directory_iterator("Data\\manifest\\nodes\\"))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".json" && entry.path().filename().replace_extension("") != "StateMachine.json" && entry.path().filename().replace_extension("") != "PhysicsStateMachine.json")
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

	mcd::BlendTree* MorphemeDocument::createNewBlendTree(const std::string& name, mcd::StateMachine* parent, float xPos, float yPos)
	{
		mcd::BlendTree* bt = new mcd::BlendTree(parent, name);

		mcd::StateMachineNode* newNode = new mcd::StateMachineNode(parent, name, xPos, yPos, 100.f, 50.f);
		newNode->setGraphEntry(bt);

		parent->addStateMachineNode(newNode);

		newNode->addPin(new mcd::PassDownPin(newNode, "Result"));

		return bt;
	}

	mcd::BlendTree* MorphemeDocument::createNewBlendTree(const std::string& name, mcd::BlendTree* parent, float xPos, float yPos)
	{
		mcd::BlendTree* bt = new mcd::BlendTree(parent, name);

		mcd::BlendTreeNode* newNode = new mcd::BlendTreeNode(parent, name, xPos, yPos, 100.f, 50.f, "BlendTree", -1);
		newNode->setGraphEntry(bt);

		parent->addBlendTreeNode(newNode);

		newNode->addPin(new mcd::PassDownPin(newNode, "Result"));

		return bt;
	}

	mcd::StateMachine* MorphemeDocument::createNewStateMachine(const std::string& name, mcc::MMStateMachine manifestSM, mcd::Graph* parent, float xPos, float yPos)
	{
		mcd::StateMachine* sm = new mcd::StateMachine(parent, name, manifestSM.getName(), manifestSM.getVersion());

		if (parent->isOfType<mcd::StateMachine>())
		{
			mcd::StateMachine* parentSM = dynamic_cast<mcd::StateMachine*>(parent);

			mcd::StateMachineNode* newNode = new mcd::StateMachineNode(parent, name, xPos, yPos, 100.f, 50.f, manifestSM.getName(), manifestSM.getVersion());
			newNode->setGraphEntry(sm);

			parentSM->addStateMachineNode(newNode);
		}
		else
		{
			mcd::BlendTree* parentBT = dynamic_cast<mcd::BlendTree*>(parent);

			mcd::BlendTreeNode* newNode = new mcd::BlendTreeNode(parent, name, xPos, yPos, 100.f, 50.f, manifestSM.getName(), manifestSM.getVersion());
			newNode->setGraphEntry(sm);

			parentBT->addBlendTreeNode(newNode);
		}

		return sm;
	}

	mcd::StateMachineNode* MorphemeDocument::createNewStateMachineNode(std::string typeName, mcd::StateMachine* parent, const std::string& name, float xPos, float yPos)
	{
		if (typeName == "BlendTree")
		{
			mcd::BlendTree* bt = createNewBlendTree(name, parent, xPos, yPos);

			mcd::StateMachineNode* smNode = new mcd::StateMachineNode(parent, name, xPos, yPos, 100.f, 70.f, "BlendTree");
			smNode->setGraphEntry(bt);
			smNode->addPin(new mcd::PassDownPin(smNode, "Result"));
			parent->addStateMachineNode(smNode);

			return smNode;
		}
		else if (typeName == "StateMachine")
		{
			mcd::StateMachine* sm = createNewStateMachine(name, mcc::MMStateMachine(), parent, xPos, yPos);

			mcd::StateMachineNode* smNode = new mcd::StateMachineNode(parent, name, xPos, yPos, 100.f, 70.f, "StateMachine");
			smNode->setGraphEntry(sm);
			parent->addStateMachineNode(smNode);

			return smNode;
		}

		throw std::runtime_error("Cannot create StateMachineNode. Unknown type name: " + typeName);
	}

	mcd::StateMachineNode* MorphemeDocument::createNewStateMachineNode(std::string typeName, mcd::StateMachine* parent, const std::string& name)
	{
		float xPos = 0.f;
		float yPos = 0.f;
		parent->getFreePosition(xPos, yPos);

		return createNewStateMachineNode(typeName, parent, name, xPos, yPos);
	}

	mcd::Request* MorphemeDocument::createRequest(const std::string& name)
	{
		mcd::Network* network = m_morphemeDB->getNetwork();

		mcd::Request* request = new mcd::Request(network, name, network->getNumRequests());

		return request;
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