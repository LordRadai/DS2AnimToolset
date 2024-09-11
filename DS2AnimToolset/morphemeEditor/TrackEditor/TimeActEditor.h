#pragma once
#include "TrackEditorBase.h"
#include "FromSoftware/TimeAct/TaeExport/TaeExport.h"
#include "FromSoftware/TimeAct/TaeTemplate/TaeTemplate.h"

namespace TrackEditor
{
	class TimeActEditor : public TrackEditorBase
	{
	public:
		static TimeActEditor* create(int options, TimeCodeFormat timeCodeFormat, TimeAct::TaeTemplate* templateRes);

		void setTemplate(TimeAct::TaeTemplate* resource) { this->m_template = resource; }

		void update(float dt);
		void destroy();

		TimeAct::TaeExport::TimeActTrackExportXML* getSource() { return static_cast<TimeAct::TaeExport::TimeActTrackExportXML*>(this->m_source); }
		void setTimeAct(TimeAct::TaeExport::TimeActTrackExportXML* resource);
		void reset();

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
		TimeActEditor() {}
		~TimeActEditor() {}

		void reload();
		TimeAct::TaeTemplate* m_template;
	};
}