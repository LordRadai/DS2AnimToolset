#include "Track.h"

namespace TrackEditor
{
	Track* Track::create(int channelId, int userData, std::string name, bool discrete)
	{
		Track* track = new Track;

		track->channelId = channelId;
		track->userData = userData;
		strcpy_s(track->name, name.c_str());
		track->discrete = discrete;

		return track;
	}

	void Track::destroy()
	{
		for (size_t i = 0; i < this->events.size(); i++)
			this->events[i]->destroy();

		this->events.clear();

		delete this;
	}
}