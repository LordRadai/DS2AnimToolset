#pragma once
#include "mcnNode.h"

namespace MCN
{
	namespace Node
	{
		class AnimWithEvents : public mcnNode
		{
		public:
			AnimWithEvents(float xpos, float ypos, float width, float height, bool bLoop, bool bPlayBackwards, bool bGenerateDeltas, bool bPreComputeSyncEventTracks)
				: mcnNode(xpos, ypos, width, height)
			{
				setNodeType("AnimWithEvents");
				setManifestVersion(9);

				this->addAttribute(BoolAttribute("Loop", bLoop));
				this->addAttribute(BoolAttribute("PlayBackwards", bPlayBackwards));
				this->addAttribute(BoolAttribute("GenerateDeltas", bGenerateDeltas));
				this->addAttribute(BoolAttribute("PreComputeSyncEventTracks", bPreComputeSyncEventTracks));
			}

			void addAnimTake(std::string animSetName, std::string filename, std::string takeName, std::string format, std::string syncTrack);
			//void addClipFraction(float startFraction, float endFraction, std::string animSetName);
			//void addStartEventIndex(int startEventIndex, std::string animSetName);
		};
	}
}