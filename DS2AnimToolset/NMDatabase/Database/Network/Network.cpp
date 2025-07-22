#include "Network.h"
#include "mcnSerializer/mcnSerializer.h"

namespace db
{
	Network::Network(Node* parent) : Node(parent, "Network", "Network"),
		m_nodes(this, "GraphEntry"),
		m_passDownPin(this, "Result", true),
		m_animationLocations(this, "AnimationLocations"),
		m_previewScripts(this, "PreviewScripts"),
		m_controlParameters(this),
		m_emittedControlParameters(this),
		m_layers(this),
		m_sceneGraphRoot(this),
		m_animationSets(this),
		m_bodyGroups(this),
		m_requests(this),
		m_requestPresets(this),
		m_messagePresetsGroup(this),
		m_characterStartPoints(this)
	{
		CoCreateGuid(&m_GUID);
	}

	bool Network::isValid() const
	{
		return Node::isValid();
	}

	tinyxml2::XMLElement* Network::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);
		m_nodes.serialize(element);
		m_animationLocations.serialize(element);
		m_previewScripts.serialize(element);
		mcnSerializer::createStringElement(element, "GUID", RString::guidToString(m_GUID));
		mcnSerializer::createStringElement(element, "AnimLibraryGUID", m_animLibraryGUID);
		mcnSerializer::createStringElement(element, "AssetManagerSelectedSet", m_assetManagerSelectedSet);
		m_passDownPin.serialize(element);
		m_controlParameters.serialize(element);
		m_emittedControlParameters.serialize(element);
		m_layers.serialize(element);
		m_sceneGraphRoot.serialize(element);
		m_animationSets.serialize(element);
		m_bodyGroups.serialize(element);
		m_requests.serialize(element);
		m_requestPresets.serialize(element);
		m_messagePresetsGroup.serialize(element);
		m_characterStartPoints.serialize(element);

		return element;
	}

	AnimationLocation* Network::getAnimationLocation(int index) const
	{
		if (index < 0 || index >= m_animationLocations.getNumNodes())
			throw std::out_of_range("Index out of range");

		return dynamic_cast<AnimationLocation*>(m_animationLocations.getNode(index));
	}

	AnimationLocation* Network::addAnimationLocation(const std::string& sourceDir, const std::string& markupDir, bool bIncludeSubDirs)
	{
		AnimationLocation* newLocation = new AnimationLocation(this, sourceDir, markupDir, bIncludeSubDirs);

		m_animationLocations.addNode(newLocation);
		return newLocation;
	}

	void Network::removeAnimationLocation(int index)
	{
		if (index < 0 || index >= m_animationLocations.getNumNodes())
			throw std::out_of_range("Index out of range");

		m_animationLocations.removeNode(index);
	}

	PreviewScript* Network::getPreviewScript(int index) const
	{
		if (index < 0 || index >= m_previewScripts.getNumNodes())
			throw std::out_of_range("Index out of range");

		return dynamic_cast<PreviewScript*>(m_previewScripts.getNode(index));
	}

	PreviewScript* Network::addPreviewScript(const std::string& filepath)
	{
		PreviewScript* newScript = new PreviewScript(this, "Preview script", filepath);

		m_previewScripts.addNode(newScript);
		return newScript;
	}

	void Network::removePreviewScript(int index)
	{
		if (index < 0 || index >= m_previewScripts.getNumNodes())
			throw std::out_of_range("Index out of range");

		m_previewScripts.removeNode(index);
	}

	ControlParameter* Network::getControlParameter(int index) const
	{
		return m_controlParameters.getControlParameter(index);
	}

	ControlParameter* Network::getControlParameterByName(const std::string& name) const
	{
		for (size_t i = 0; i < m_controlParameters.getNumControlParameters(); i++)
		{
			ControlParameter* cp = m_controlParameters.getControlParameter(i);

			if (cp->getName() == name)
				return cp;
		}

		return nullptr;
	}

	ControlParameter* Network::addControlParameter(ControlParameter* controlParameter)
	{
		m_controlParameters.addControlParameter(controlParameter);
		return controlParameter;
	}

	void Network::removeControlParameter(int index)
	{
		m_controlParameters.removeControlParameter(index);
	}

	EmittedControlParameter* Network::getEmittedControlParameter(int index) const
	{
		return m_emittedControlParameters.getEmittedControlParameter(index);
	}

	EmittedControlParameter* Network::addEmittedControlParameter(EmittedControlParameter* emittedControlParameter) 
	{ 
		m_emittedControlParameters.addEmittedControlParameter(emittedControlParameter); 
		return emittedControlParameter;
	}

	void Network::removeEmittedControlParameter(int index)
	{
		m_emittedControlParameters.removeEmittedControlParameter(index);
	}

	AnimationSet* Network::getAnimationSet(int index) const
	{
		return m_animationSets.getAnimationSet(index);
	}

	AnimationSet* Network::addAnimationSet(std::string name)
	{
		AnimationSet* newSet = new AnimationSet(this, name);
		m_animationSets.addAnimationSet(newSet);

		return newSet;
	}

	void  Network::removeAnimationSet(int index)
	{
		m_animationSets.removeAnimationSet(index);
	}

	BodyGroup* Network::getBodyGroup(int index) const
	{
		return m_bodyGroups.getBodyGroup(index);
	}

	BodyGroup* Network::addBodyGroup(const std::string& name)
	{
		BodyGroup* newGroup = new BodyGroup(this, name);
		m_bodyGroups.addBodyGroup(newGroup);

		return newGroup;
	}

	void  Network::removeBodyGroup(int index)
	{
		m_bodyGroups.removeBodyGroup(index);
	}

	Request* Network::getRequest(int index) const
	{
		return m_requests.getRequest(index);
	}

	Request* Network::getRequestByName(const std::string& name) const
	{
		for (size_t i = 0; i < m_requests.getNumRequests(); i++)
		{
			Request* request = m_requests.getRequest(i);

			if (request->getName() == name)
				return request;
		}

		return nullptr;
	}

	Request* Network::addRequest(const std::string& name)
	{
		Request* newRequest = new Request(this, name);

		m_requests.addRequest(newRequest);
		return newRequest;
	}

	void Network::removeRequest(int index)
	{
		m_requests.removeRequest(index);
	}

	RequestPreset* Network::getRequestPreset(int index) const
	{
		return m_requestPresets.getRequestPreset(index);
	}

	RequestPreset* Network::addRequestPreset(Request request)
	{
		RequestPreset* newPreset = new RequestPreset(this, request);
		m_requestPresets.addRequestPreset(newPreset);

		return newPreset;
	}

	void  Network::removeRequestPreset(int index)
	{
		m_requestPresets.removeRequestPreset(index);
	}

	CharacterStartPoint* Network::getCharacterStartPoint(int index) const
	{
		return m_characterStartPoints.getCharacterStartPoint(index);
	}

	CharacterStartPoint* Network::addCharacterStartPoint(AnimationSet* animationSet)
	{
		CharacterStartPoint* newStartPoint = new CharacterStartPoint(this, animationSet);
		m_characterStartPoints.addCharacterStartPoint(newStartPoint);

		return newStartPoint;
	}

	void  Network::removeCharacterStartPoint(int index)
	{
		m_characterStartPoints.removeCharacterStartPoint(index);
	}
}