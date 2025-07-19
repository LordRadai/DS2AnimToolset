#pragma once
#include "RCore.h"
#include "../AnimationLocation/AnimationLocation.h"
#include "../PreviewScript/PreviewScript.h"
#include "../Request/Request.h"
#include "../BodyGroup/BodyGroup.h"
#include "../AnimationSet/AnimationSet.h"

namespace db
{
	class Network
	{
		GUID m_GUID;
		std::string m_animLibraryGUID;
		std::string m_assetManagerSelectedSet;
		//std::vector<Node> m_nodes;
		std::vector<AnimationLocation> m_animationLocations;
		std::vector<PreviewScript> m_previewScripts;
		std::vector<Request> m_requests;
		std::vector<Request> m_requestPresets;
		std::vector<BodyGroup> m_bodyGroups;
		std::vector<AnimationSet> m_animationSets;
	};
}