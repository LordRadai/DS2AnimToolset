#pragma once
#include <vector>
#include <string>
#include "Event/Event.h"

namespace TrackEditor
{
	class Track
	{
		friend class TrackEditorBase;
		friend class TimeActEditor;
		friend class EventTrackEditor;

	public:
		static Track* create(int channelId, int userData, std::string name, bool discrete);
		void destroy();

		int channelId = 0;
		int userData = 0;
		std::vector<Event*> events;
		char name[256];
		bool discrete = false;
	protected:
		Track() {}
		~Track() {}
	};
}