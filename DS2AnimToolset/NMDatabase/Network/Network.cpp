#include "Network.h"

namespace db
{
	Network::Network()
	{
		CoCreateGuid(&m_GUID);
		m_passDownPin = new PassDownPin("Result", true);
	}

	AnimationLocation* Network::getAnimationLocation(int index) const
	{
		if (index < 0 || index >= m_animationLocations.size())
			throw std::out_of_range("Index out of range");
		return m_animationLocations[index];
	}

	AnimationLocation* Network::addAnimationLocation(const std::string& sourceDir, const std::string& markupDir, bool bIncludeSubDirs = true)
	{
		m_animationLocations.push_back(new AnimationLocation(sourceDir, markupDir, bIncludeSubDirs));
		return m_animationLocations.back();
	}

	void Network::removeAnimationLocation(int index)
	{
		if (index < 0 || index >= m_animationLocations.size())
			throw std::out_of_range("Index out of range");
		delete m_animationLocations[index];
		m_animationLocations.erase(m_animationLocations.begin() + index);
	}

	PreviewScript* Network::getPreviewScript(int index) const
	{
		if (index < 0 || index >= m_previewScripts.size())
			throw std::out_of_range("Index out of range");
		return m_previewScripts[index];
	}

	PreviewScript* Network::addPreviewScript(const std::string& filepath)
	{
		m_previewScripts.push_back(new PreviewScript(filepath));
		return m_previewScripts.back();
	}

	void Network::removePreviewScript(int index)
	{
		if (index < 0 || index >= m_previewScripts.size())
			throw std::out_of_range("Index out of range");
		delete m_previewScripts[index];
		m_previewScripts.erase(m_previewScripts.begin() + index);
	}

	ControlParameter* Network::getControlParameter(int index) const
	{
		if (index < 0 || index >= m_controlParameters.size())
			throw std::out_of_range("Index out of range");
		return m_controlParameters[index];
	}

	ControlParameter* Network::getControlParameterByName(const std::string& name) const
	{
		for (const auto& cp : m_controlParameters)
		{
			if (cp->getName() == name)
				return cp;
		}

		return nullptr; // Not found
	}

	ControlParameter* Network::addControlParameter(ControlParameter* controlParameter)
	{
		m_controlParameters.push_back(controlParameter);
		return m_controlParameters.back();
	}

	void Network::removeControlParameter(int index)
	{
		if (index < 0 || index >= m_controlParameters.size())
			throw std::out_of_range("Index out of range");
		delete m_controlParameters[index];
		m_controlParameters.erase(m_controlParameters.begin() + index);
	}

	EmittedControlParameter* Network::getEmittedControlParameter(int index) const
	{
		if (index < 0 || index >= m_emittedControlParameters.size())
			throw std::out_of_range("Index out of range");
		return m_emittedControlParameters[index];
	}

	EmittedControlParameter* Network::addEmittedControlParameter(EmittedControlParameter* emittedControlParameter) 
	{ 
		m_emittedControlParameters.push_back(emittedControlParameter); 
		return m_emittedControlParameters.back();
	}

	void Network::removeEmittedControlParameter(int index)
	{
		if (index < 0 || index >= m_emittedControlParameters.size())
			throw std::out_of_range("Index out of range");
		delete m_emittedControlParameters[index];
		m_emittedControlParameters.erase(m_emittedControlParameters.begin() + index);
	}

	AnimationSet* Network::getAnimationSet(int index) const
	{
		if (index < 0 || index >= m_animationSets.size())
			throw std::out_of_range("Index out of range");
		return m_animationSets[index];
	}

	AnimationSet* Network::addAnimationSet()
	{
		m_animationSets.push_back(new AnimationSet());
		return m_animationSets.back();
	}

	void  Network::removeAnimationSet(int index)
	{
		if (index < 0 || index >= m_animationSets.size())
			throw std::out_of_range("Index out of range");
		delete m_animationSets[index];
		m_animationSets.erase(m_animationSets.begin() + index);
	}

	BodyGroup* Network::getBodyGroup(int index) const
	{
		if (index < 0 || index >= m_bodyGroups.size())
			throw std::out_of_range("Index out of range");
		return m_bodyGroups[index];
	}

	BodyGroup* Network::addBodyGroup(const std::string& name)
	{
		m_bodyGroups.push_back(new BodyGroup(name));
		return m_bodyGroups.back();
	}

	void  Network::removeBodyGroup(int index)
	{
		if (index < 0 || index >= m_bodyGroups.size())
			throw std::out_of_range("Index out of range");
		delete m_bodyGroups[index];
		m_bodyGroups.erase(m_bodyGroups.begin() + index);
	}

	Request* Network::getRequest(int index) const
	{
		if (index < 0 || index >= m_requests.size())
			throw std::out_of_range("Index out of range");
		return m_requests[index];
	}

	Request* Network::getRequestByName(const std::string& name) const
	{
		for (const auto& request : m_requests)
		{
			if (request->getName() == name)
				return request;
		}

		return nullptr; // Not found
	}

	Request* Network::addRequest(const std::string& name)
	{
		m_requests.push_back(new Request(name));
		return m_requests.back();
	}

	void  Network::removeRequest(int index)
	{
		if (index < 0 || index >= m_requests.size())
			throw std::out_of_range("Index out of range");
		delete m_requests[index];
		m_requests.erase(m_requests.begin() + index);
	}

	RequestPreset* Network::getRequestPreset(int index) const
	{
		if (index < 0 || index >= m_requestPresets.size())
			throw std::out_of_range("Index out of range");
		return m_requestPresets[index];
	}

	RequestPreset* Network::addRequestPreset(Request* request)
	{
		m_requestPresets.push_back(new RequestPreset(request));
		return m_requestPresets.back();
	}

	void  Network::removeRequestPreset(int index)
	{
		if (index < 0 || index >= m_requestPresets.size())
			throw std::out_of_range("Index out of range");
		delete m_requestPresets[index];
		m_requestPresets.erase(m_requestPresets.begin() + index);
	}

	CharacterStartPoint* Network::getCharacterStartPoint(int index) const
	{
		if (index < 0 || index >= m_characterStartPoints.size())
			throw std::out_of_range("Index out of range");
		return m_characterStartPoints[index];
	}

	CharacterStartPoint* Network::addCharacterStartPoint(AnimationSet* animationSet)
	{
		m_characterStartPoints.push_back(new CharacterStartPoint(animationSet));
		return m_characterStartPoints.back();
	}

	void  Network::removeCharacterStartPoint(int index)
	{
		if (index < 0 || index >= m_characterStartPoints.size())
			throw std::out_of_range("Index out of range");
		delete m_characterStartPoints[index];
		m_characterStartPoints.erase(m_characterStartPoints.begin() + index);
	}
}