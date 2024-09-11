#pragma once
#include "TrackEditorBase.h"
#include "AnimObject/AnimObject.h"
#include "export/include/export/mcExportXml.h"

namespace TrackEditor
{
	class EventTrackEditor : public TrackEditorBase
	{
	public:
		static EventTrackEditor* create(int options, TimeCodeFormat timeCodeFormat);

		void update(float dt);
		void reset();
		void destroy();

		AnimObject* getSource() { return static_cast<AnimObject*>(this->m_source); }
		void setEventTrack(AnimObject* resource);

		std::string getTrackName(int idx);
		std::string getEventLabel(int trackIdx, int eventIdx, bool arguments);
		void addTrack(int userData, std::string name, bool discrete);
		void deleteTrack(int idx);
		void addEvent(int trackIdx, float startTime, float endTime, int userData);
		void deleteEvent(int trackIdx, int eventIdx);
		bool addTrackGUI();
		bool deleteTrackGUI();
		bool addEventGUI();
		bool deleteEventGUI();
		bool renameTrackGUI();

		bool saveTracks();

	private:
		EventTrackEditor() {}
		~EventTrackEditor() {}

		void reload();
	};
}