#pragma once
#include "../ManifestItemBase.h"
#include "NodeEditor/Editor/Transition/Transition.h"

namespace NodeEditor
{
	namespace Manifest
	{
		class MMTransition : public ManifestItemBase
		{
			std::vector<std::string> m_interfaces;
		public:
			MMTransition() : ManifestItemBase() {}
			MMTransition(const nlohmann::json& json) : ManifestItemBase() { fromJson(json); }

			virtual ~MMTransition() {}
			virtual void fromJson(const nlohmann::json& json) override;

			int getAnimID() const { return m_jsonData["animId"]; }
			int getEuphoriaID() const { return m_jsonData["euphoriaId"]; }
			int getPhysicsID() const { return m_jsonData["physicsId"]; }
			bool supportTransitionToSelf() const { return m_jsonData["supportsTransitToSelf"]; }

			void addInterface(const std::string& interfaceName) { m_interfaces.push_back(interfaceName); }
			std::string getInterface(uint32_t index) const;
			std::string findInterface(const std::string& name) const;
			uint32_t getNumInterfaces() const { return static_cast<uint32_t>(m_interfaces.size()); }

			Transition* makeTransition(NodeEditor* editor, int id, Node* sourceNode, Node* destinationNode);
		};
	}
}