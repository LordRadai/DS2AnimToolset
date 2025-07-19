#include "Network.h"

namespace db
{
	Network::Network() : Node("Network")
	{
		CoCreateGuid(&m_GUID);
		m_passDownPin = PassDownPin("Result", true);
		m_sceneGraphRoot = SceneGraphRoot();
	}

	AnimationLocation* Network::getAnimationLocation(int index) const
	{
		if (index < 0 || index >= m_animationLocations.getNumNodes())
			throw std::out_of_range("Index out of range");

		return static_cast<AnimationLocation*>(m_animationLocations.getNode(index));
	}

	AnimationLocation* Network::addAnimationLocation(const std::string& sourceDir, const std::string& markupDir, bool bIncludeSubDirs = true)
	{
		AnimationLocation* newLocation = new AnimationLocation(sourceDir, markupDir, bIncludeSubDirs);

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

		return static_cast<PreviewScript*>(m_previewScripts.getNode(index));
	}

	PreviewScript* Network::addPreviewScript(const std::string& filepath)
	{
		PreviewScript* newScript = new PreviewScript(filepath);

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
		if (index < 0 || index >= m_controlParameters.getNumNodes())
			throw std::out_of_range("Index out of range");

		return static_cast<ControlParameter*>(m_controlParameters.getNode(index));
	}

	ControlParameter* Network::getControlParameterByName(const std::string& name) const
	{
		for (size_t i = 0; i < m_controlParameters.getNumNodes(); i++)
		{
			ControlParameter* cp = static_cast<ControlParameter*>(m_controlParameters.getNode(i));

			if (cp->getName() == name)
				return cp;
		}

		return nullptr;
	}

	ControlParameter* Network::addControlParameter(ControlParameter* controlParameter)
	{
		m_controlParameters.addNode(controlParameter);
		return controlParameter;
	}

	void Network::removeControlParameter(int index)
	{
		if (index < 0 || index >= m_controlParameters.getNumNodes())
			throw std::out_of_range("Index out of range");

		m_controlParameters.removeNode(index);
	}

	EmittedControlParameter* Network::getEmittedControlParameter(int index) const
	{
		if (index < 0 || index >= m_emittedControlParameters.getNumNodes())
			throw std::out_of_range("Index out of range");

		return static_cast<EmittedControlParameter*>(m_emittedControlParameters.getNode(index));
	}

	EmittedControlParameter* Network::addEmittedControlParameter(EmittedControlParameter* emittedControlParameter) 
	{ 
		m_emittedControlParameters.addNode(emittedControlParameter); 
		return emittedControlParameter;
	}

	void Network::removeEmittedControlParameter(int index)
	{
		if (index < 0 || index >= m_emittedControlParameters.getNumNodes())
			throw std::out_of_range("Index out of range");

		m_emittedControlParameters.removeNode(index);
	}

	AnimationSet* Network::getAnimationSet(int index) const
	{
		if (index < 0 || index >= m_animationSets.getNumNodes())
			throw std::out_of_range("Index out of range");

		return static_cast<AnimationSet*>(m_animationSets.getNode(index));
	}

	AnimationSet* Network::addAnimationSet()
	{
		AnimationSet* newSet = new AnimationSet();
		m_animationSets.addNode(newSet);

		return newSet;
	}

	void  Network::removeAnimationSet(int index)
	{
		if (index < 0 || index >= m_animationSets.getNumNodes())
			throw std::out_of_range("Index out of range");

		m_animationSets.removeNode(index);
	}

	BodyGroup* Network::getBodyGroup(int index) const
	{
		if (index < 0 || index >= m_bodyGroups.getNumNodes())
			throw std::out_of_range("Index out of range");

		return static_cast<BodyGroup*>(m_bodyGroups.getNode(index));
	}

	BodyGroup* Network::addBodyGroup(const std::string& name)
	{
		BodyGroup* newGroup = new BodyGroup(name);
		m_bodyGroups.addNode(newGroup);

		return newGroup;
	}

	void  Network::removeBodyGroup(int index)
	{
		if (index < 0 || index >= m_bodyGroups.getNumNodes())
			throw std::out_of_range("Index out of range");

		m_bodyGroups.removeNode(index);
	}

	Request* Network::getRequest(int index) const
	{
		if (index < 0 || index >= m_requests.getNumNodes())
			throw std::out_of_range("Index out of range");

		return static_cast<Request*>(m_requests.getNode(index));
	}

	Request* Network::getRequestByName(const std::string& name) const
	{
		for (size_t i = 0; i < m_requests.getNumNodes(); i++)
		{
			Request* request = static_cast<Request*>(m_requests.getNode(i));

			if (request->getName() == name)
				return request;
		}

		return nullptr;
	}

	Request* Network::addRequest(const std::string& name)
	{
		Request* newRequest = new Request(name);

		m_requests.addNode(newRequest);
		return newRequest;
	}

	void  Network::removeRequest(int index)
	{
		if (index < 0 || index >= m_requests.getNumNodes())
			throw std::out_of_range("Index out of range");

		m_requests.removeNode(index);
	}

	RequestPreset* Network::getRequestPreset(int index) const
	{
		if (index < 0 || index >= m_requestPresets.getNumNodes())
			throw std::out_of_range("Index out of range");

		return static_cast<RequestPreset*>(m_requestPresets.getNode(index));
	}

	RequestPreset* Network::addRequestPreset(Request request)
	{
		RequestPreset* newPreset = new RequestPreset(request);
		m_requestPresets.addNode(newPreset);

		return newPreset;
	}

	void  Network::removeRequestPreset(int index)
	{
		if (index < 0 || index >= m_requestPresets.getNumNodes())
			throw std::out_of_range("Index out of range");

		m_requestPresets.removeNode(index);
	}

	CharacterStartPoint* Network::getCharacterStartPoint(int index) const
	{
		if (index < 0 || index >= m_characterStartPoints.getNumNodes())
			throw std::out_of_range("Index out of range");

		return static_cast<CharacterStartPoint*>(m_characterStartPoints.getNode(index));
	}

	CharacterStartPoint* Network::addCharacterStartPoint(AnimationSet* animationSet)
	{
		CharacterStartPoint* newStartPoint = new CharacterStartPoint(animationSet);
		m_characterStartPoints.addNode(newStartPoint);

		return newStartPoint;
	}

	void  Network::removeCharacterStartPoint(int index)
	{
		if (index < 0 || index >= m_characterStartPoints.getNumNodes())
			throw std::out_of_range("Index out of range");

		m_characterStartPoints.removeNode(index);
	}
}