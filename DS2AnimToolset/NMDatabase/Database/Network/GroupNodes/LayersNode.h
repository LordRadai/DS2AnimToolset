#pragma once
#include "../Layer/Layer.h"
#include "../NodeContainer/NodeContainer.h"

namespace db
{
	class LayersNode : public Node
	{
		NodeContainer m_layers;

	public:
		LayersNode(Node* parent) : Node(parent, "LayersNode", "Layers"), m_layers(this, "LayerArray") {};
		virtual ~LayersNode() {};
		virtual bool isValid() const { return Node::isValid() && m_layers.isValid(); }
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		void addLayer(Layer* layer) { m_layers.addNode(layer); }
		void removeLayer(int index) { m_layers.removeNode(index); }
		Layer* getLayer(int index) const { return static_cast<Layer*>(m_layers.getNode(index)); }
		size_t getNumLayers() const { return m_layers.getNumNodes(); }
	};
}
