#pragma once
#include "../CharacterStartPoint/CharacterStartPoint.h"
#include "../NodeContainer/NodeContainer.h"

namespace db
{
	class CharacterStartPointsNode : public Node
	{
		NodeContainer m_characterStartPoints;

	public:
		CharacterStartPointsNode(Node* parent) : Node(parent, "CharacterStartPointsNode", "CharacterStartPoints"), m_characterStartPoints(this, "CharacterStartPointsArray") {};
		virtual ~CharacterStartPointsNode() {};
		virtual bool isValid() const { return Node::isValid() && m_characterStartPoints.isValid(); };
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		void addCharacterStartPoint(CharacterStartPoint* startPoint) { m_characterStartPoints.addNode(startPoint); }
		void removeCharacterStartPoint(int index) { m_characterStartPoints.removeNode(index); }
		CharacterStartPoint* getCharacterStartPoint(int index) const { return static_cast<CharacterStartPoint*>(m_characterStartPoints.getNode(index)); }
		size_t getNumCharacterStartPoints() const { return m_characterStartPoints.getNumNodes(); }
	};
}