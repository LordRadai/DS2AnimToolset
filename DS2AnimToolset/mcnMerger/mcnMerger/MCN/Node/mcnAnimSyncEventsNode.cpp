#include "mcnAnimSyncEventsNode.h"

namespace MCN
{
	namespace Node
	{
		void AnimWithEvents::addAnimTake(std::string animSetName, std::string filename, std::string takeName, std::string format, std::string syncTrack)
		{
			AnimationSetAttribute animTakeAttrib("AnimationTake");

			AttributePlaceholder animSet(animSetName, "string");

			animSet.addAttribute(AnimationTakeAttribute(takeName, filename, format, syncTrack));

			animTakeAttrib.addAttributePlaceholder(animSet);
			
			addAttribute(animTakeAttrib);
		}
	}
}