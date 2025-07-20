#pragma once
#include "RCore.h"
#include "../AnimationLocation/AnimationLocation.h"
#include "../PreviewScript/PreviewScript.h"
#include "../Request/Request.h"
#include "../RequestPreset/RequestPreset.h"
#include "../BodyGroup/BodyGroup.h"
#include "../AnimationSet/AnimationSet.h"
#include "../ControlParameter/ControlParameter.h"
#include "../EmittedControlParameter/EmittedControlParameter.h"
#include "../CharacterStartPoint/CharacterStartPoint.h"
#include "../Pin/Pin.h"
#include "../Layer/Layer.h"
#include "../SceneGraphRoot/SceneGraphRoot.h"
#include "../Node/Node.h"
#include "../NodeDef/NodeDef.h"
#include "AnimationSetsNode.h"
#include "BodyGroupsNode.h"
#include "CharacterStartPointsNode.h"
#include "ControlParametersNode.h"
#include "EmittedControlParametersNode.h"
#include "LayersNode.h"
#include "RequestPresetsNode.h"
#include "RequestsNode.h"

namespace db
{
	class Network : public Node
	{
		std::vector<NodeDef> m_nodes;
		NodeContainer m_animationLocations;
		NodeContainer m_previewScripts;
		GUID m_GUID;
		std::string m_animLibraryGUID;
		std::string m_assetManagerSelectedSet;
		PassDownPin m_passDownPin;
		ControlParametersNode m_controlParameters;
		EmittedControlParametersNode m_emittedControlParameters;
		LayersNode m_layers;
		SceneGraphRoot m_sceneGraphRoot;
		AnimationSetsNode m_animationSets;
		BodyGroupsNode m_bodyGroups;
		RequestsNode m_requests;
		RequestPresetsNode m_requestPresets;
		CharacterStartPointsNode m_characterStartPoints;

	public:
		Network(Node* parent);

		virtual ~Network() {};
		virtual bool isValid() const;
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		GUID getGUID() const { return m_GUID; }
		void setGUID(const GUID& guid) { m_GUID = guid; }

		std::string getAnimLibraryGUID() const { return m_animLibraryGUID; }
		void setAnimLibraryGUID(const std::string& guid) { m_animLibraryGUID = guid; }

		std::string getAssetManagerSelectedSet() const { return m_assetManagerSelectedSet; }
		void setAssetManagerSelectedSet(const std::string& set) { m_assetManagerSelectedSet = set; }

		AnimationLocation* getAnimationLocation(int index) const;
		AnimationLocation* addAnimationLocation(const std::string& sourceDir, const std::string& markupDir, bool bIncludeSubDirs = true);
		void removeAnimationLocation(int index);
		size_t getNumAnimationLocations() const { return m_animationLocations.getNumNodes(); }

		PreviewScript* getPreviewScript(int index) const;
		PreviewScript* addPreviewScript(const std::string& filepath);
		void removePreviewScript(int index);

		ControlParameter* getControlParameter(int index) const;
		ControlParameter* getControlParameterByName(const std::string& name) const;
		ControlParameter* addControlParameter(ControlParameter* controlParameter);
		void removeControlParameter(int index);
		size_t getNumControlParameters() const { return m_controlParameters.getNumControlParameters(); }

		EmittedControlParameter* getEmittedControlParameter(int index) const;
		EmittedControlParameter* addEmittedControlParameter(EmittedControlParameter* emittedControlParameter);
		void removeEmittedControlParameter(int index);
		size_t getNumEmittedControlParameters() const { return m_emittedControlParameters.getNumEmittedControlParameters(); }

		AnimationSet* getAnimationSet(int index) const;
		AnimationSet* addAnimationSet(std::string name);
		void removeAnimationSet(int index);
		size_t getNumAnimationSets() const { return m_animationSets.getNumAnimationSets(); }

		BodyGroup* getBodyGroup(int index) const;
		BodyGroup* addBodyGroup(const std::string& name);
		void removeBodyGroup(int index);
		size_t getNumBodyGroups() const { return m_bodyGroups.getNumBodyGroups(); }

		Request* getRequest(int index) const;
		Request* getRequestByName(const std::string& name) const;
		Request* addRequest(const std::string& name);
		void removeRequest(int index);
		size_t getNumRequests() const { return m_requests.getNumRequests(); }

		RequestPreset* getRequestPreset(int index) const;
		RequestPreset* addRequestPreset(Request request);
		void removeRequestPreset(int index);
		size_t getNumRequestPresets() const { return m_requestPresets.getNumRequestPresets(); }

		CharacterStartPoint* getCharacterStartPoint(int index) const;
		CharacterStartPoint* addCharacterStartPoint(AnimationSet* animationSet);
		void removeCharacterStartPoint(int index);
		size_t getNumCharacterStartPoints() const { return m_characterStartPoints.getNumCharacterStartPoints(); }
	};
}