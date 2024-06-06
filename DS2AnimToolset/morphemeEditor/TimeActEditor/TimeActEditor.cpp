#include "TimeActEditor.h"
#include "../Application/Application.h"
#include "../TaeTemplate/TaeTemplate.h"
#include "INI/INI.h"

TimeActEditor::TimeActTrack::TimeActTrack(int eventId)
{
	this->m_source = NULL;
	this->m_eventGroup = eventId;
}

TimeActEditor::TimeActTrack::TimeActTrack(EventGroup* src)
{
	this->m_source = src;
	this->m_count = src->m_count;

	this->m_eventGroup = src->m_groupData->m_eventType;

	for (size_t i = 0; i < src->m_count; i++)
		this->m_event.push_back(Event{ RMath::TimeToFrame(src->m_event[i]->m_start, 30), RMath::TimeToFrame(src->m_event[i]->m_end, 30) - RMath::TimeToFrame(src->m_event[i]->m_start, 30), (int)src->m_event[i]->m_eventData->m_id,  src->m_event[i]->m_eventData->m_args });
}

void TimeActEditor::TimeActTrack::SaveTimeActTrack()
{
	this->m_source->m_count = this->m_count;
	this->m_source->m_groupData->m_eventType = this->m_eventGroup;

	for (size_t i = 0; i < this->m_count; i++)
	{
		this->m_source->m_event[i]->m_start = RMath::FrameToTime(this->m_event[i].m_frameStart, 30);
		this->m_source->m_event[i]->m_end = RMath::FrameToTime(this->m_event[i].m_frameStart + this->m_event[i].m_duration, 30);
		this->m_source->m_event[i]->m_eventData->m_id = this->m_event[i].m_value;
	}
}

bool TimeActEditor::TimeActTrack::IsEventActive(int idx, int frame)
{
	if ((frame >= this->m_event[idx].m_frameStart) && frame <= (this->m_event[idx].m_duration + this->m_event[idx].m_frameStart))
		return true;

	return false;
}

int TimeActEditor::GetFrameMin() const
{
	return m_frameMin;
}

int TimeActEditor::GetFrameMax() const
{
	return m_frameMax;
}

int TimeActEditor::GetTrackCount() const { return (int)m_tracks.size(); }

TimeActEditor::TimeActEditor() 
{
	this->m_source = nullptr;
	INI ini;

	if (!ini.Open(".//Data//res//color//timeact.ini"))
	{
		g_appLog->AlertMessage(MsgLevel_Error, "TimeActEditor.cpp", "Failed to load timeact.ini\n");

		this->m_colors.m_trackColor = { 0.33f, 0.33f, 0.33f, 1.f };
		this->m_colors.m_trackColorInactive = { 0.33f, 0.33f, 0.33f, 1.f };
		this->m_colors.m_trackColorActive = { 0.f, 0.4f, 0.8f, 1.f };
		this->m_colors.m_trackBoundingBox = { 0.f, 0.f, 0.f, 1.f };
		this->m_colors.m_trackBoundingBoxActive = { 1.f, 1.f, 0.f, 1.f };
		this->m_colors.m_trackTextColor = { 1.f, 1.f, 1.f, 1.f };
		this->m_colors.m_cursorColor = { 0.f, 0.f, 0.f, 1.f };
	}

	this->m_colors.m_trackColor = { (float)ini.GetFloat("Track", "r", 0.33f), (float)ini.GetFloat("Track", "g", 0.33f), (float)ini.GetFloat("Track", "b", 0.33f), (float)ini.GetFloat("Track", "a", 1.f) };
	this->m_colors.m_trackColorInactive = { (float)ini.GetFloat("TrackInactive", "r", 0.33f), (float)ini.GetFloat("TrackInactive", "g", 0.33f), (float)ini.GetFloat("TrackInactive", "b", 0.33f), (float)ini.GetFloat("TrackInactive", "a", 1.f) };
	this->m_colors.m_trackColorActive = { (float)ini.GetFloat("TrackActive", "r", 0.f), (float)ini.GetFloat("TrackActive", "g", 0.4f), (float)ini.GetFloat("TrackActive", "b", 0.8f), (float)ini.GetFloat("TrackActive", "a", 1.f) };
	this->m_colors.m_trackBoundingBox = { (float)ini.GetFloat("TrackBoundingBox", "r", 0.f), (float)ini.GetFloat("TrackBoundingBox", "g", 0.f), (float)ini.GetFloat("TrackBoundingBox", "b", 0.f), (float)ini.GetFloat("TrackBoundingBox", "a", 1.f) };
	this->m_colors.m_trackBoundingBoxActive = { (float)ini.GetFloat("TrackActiveBoundingBox", "r", 1.f), (float)ini.GetFloat("TrackActiveBoundingBox", "g", 1.f), (float)ini.GetFloat("TrackActiveBoundingBox", "b", 0.f), (float)ini.GetFloat("TrackActiveBoundingBox", "a", 1.f) };
	this->m_colors.m_trackTextColor = { (float)ini.GetFloat("TrackText", "r", 1.f), (float)ini.GetFloat("TrackText", "g", 1.f), (float)ini.GetFloat("TrackText", "b", 1.f), (float)ini.GetFloat("TrackText", "a", 1.f) };
	this->m_colors.m_cursorColor = { (float)ini.GetFloat("TrackCursor", "r", 1.f), (float)ini.GetFloat("TrackCursor", "g", 1.f), (float)ini.GetFloat("TrackCursor", "b", 1.f), (float)ini.GetFloat("TrackCursor", "a", 1.f) };
}

std::string TimeActEditor::GetTrackName(int idx) 
{ 
	return g_taeTemplate->GetGroupName(this->m_tracks[idx].m_eventGroup);
}

std::string TimeActEditor::GetEventLabel(int idx, int event_idx, bool arguments) const
{
	if (arguments)
		return g_taeTemplate->GetEventName(this->m_tracks[idx].m_eventGroup, this->m_tracks[idx].m_event[event_idx].m_value) + this->m_tracks[idx].m_event[event_idx].m_args->GetArgumentsAsString();

	return g_taeTemplate->GetEventName(this->m_tracks[idx].m_eventGroup, this->m_tracks[idx].m_event[event_idx].m_value);
}

void TimeActEditor::AddGroup(int id)
{
	this->m_source->m_taeData->m_eventGroupCount++;
	this->m_source->m_taeData->m_groups.push_back(EventGroup(id));

	g_appLog->DebugMessage(MsgLevel_Debug, "Added group ID %d\n", id);

	this->m_reload = true;
}

void TimeActEditor::DeleteGroup(int idx)
{
	while (this->m_source->m_taeData->m_groups[idx].m_count > 0)
		this->DeleteEvent(idx, 0);

	this->m_source->m_taeData->m_eventGroupCount--;
	this->m_source->m_taeData->m_groups.erase(this->m_source->m_taeData->m_groups.begin() + idx);

	g_appLog->DebugMessage(MsgLevel_Debug, "Removed group %d\n", idx);

	this->m_reload = true;
}

void TimeActEditor::AddEvent(int group_idx, TimeActEvent event)
{
	this->m_source->m_taeData->m_eventCount++;

	TimeActEvent new_event(event.m_start, event.m_end, event.m_eventData->m_id);

	this->m_source->m_taeData->m_events.push_back(new_event);

	this->m_source->m_taeData->m_groups[group_idx].m_count++;

	this->m_source->m_taeData->m_groups[group_idx].m_eventOffset.push_back(0);
	this->m_source->m_taeData->m_groups[group_idx].m_eventIndex.push_back(this->m_source->m_taeData->m_events.size() - 1);

	g_appLog->DebugMessage(MsgLevel_Debug, "Added event (%.3f, %.3f, %d)\n", event.m_start, event.m_end, event.m_eventData->m_id);

	this->m_reload = true;
}

void TimeActEditor::DeleteEvent(int group_idx, int event_idx)
{
	int delete_index = this->m_source->m_taeData->m_groups[group_idx].m_eventIndex[event_idx];
	this->m_source->m_taeData->m_groups[group_idx].m_count--;

	this->m_source->m_taeData->m_groups[group_idx].m_eventOffset.erase(this->m_source->m_taeData->m_groups[group_idx].m_eventOffset.begin() + event_idx);
	this->m_source->m_taeData->m_groups[group_idx].m_eventIndex.erase(this->m_source->m_taeData->m_groups[group_idx].m_eventIndex.begin() + event_idx);

	this->m_source->m_taeData->m_eventCount--;

	this->m_source->m_taeData->m_events.erase(this->m_source->m_taeData->m_events.begin() + delete_index);

	for (size_t i = 0; i < this->m_source->m_taeData->m_groups.size(); i++)
	{
		for (size_t j = 0; j < this->m_source->m_taeData->m_groups[i].m_eventIndex.size(); j++)
		{
			if (this->m_source->m_taeData->m_groups[i].m_eventIndex[j] >= delete_index)
				this->m_source->m_taeData->m_groups[i].m_eventIndex[j] -= 1;
		}
	}

	g_appLog->DebugMessage(MsgLevel_Debug, "Deleted event %d (group=%d)\n", event_idx, group_idx);

	this->m_reload = true;
}

void TimeActEditor::ReloadTracks()
{
	this->m_reload = false;

	this->m_tracks.clear();
	this->SetEditedState(false);

	if ((g_appRootWindow->m_timeAct->m_init == true))
	{
		for (size_t i = 0; i < this->m_source->m_taeData->m_eventGroupCount; i++)
		{
			this->m_source->m_taeData->m_groups[i].m_event.clear();

			for (size_t j = 0; j < this->m_source->m_taeData->m_groups[i].m_count; j++)
				this->m_source->m_taeData->m_groups[i].m_event.push_back(&this->m_source->m_taeData->m_events[this->m_source->m_taeData->m_groups[i].m_eventIndex[j]]);
		}

		if (this->m_source->m_taeData->m_eventGroupCount > 0)
		{
			for (int j = 0; j < this->m_source->m_taeData->m_eventGroupCount; j++)
				this->m_tracks.push_back(&this->m_source->m_taeData->m_groups[j]);
		}
	}	
}

void TimeActEditor::SetEditedState(bool state)
{
	if (this->m_taeIdx == -1)
		return;

	if (g_appRootWindow->m_timeActEditor->m_edited.size() > this->m_taeIdx)
		g_appRootWindow->m_timeActEditor->m_edited[this->m_taeIdx] = state;
	else
		RDebug::SystemPanic("TimeActEditor.cpp", "Out of bound read while setting edited state (idx=%d, size=%d)\n", this->m_taeIdx, g_appRootWindow->m_timeActEditor->m_edited.size());
}

void TimeActEditor::Clear()
{
	this->m_taeIdx = -1;
	this->m_frameMax = 0;
	this->m_frameMin = 0;
	this->m_source = nullptr;
	this->m_tracks.clear();
}

void TimeActEditor::ResetSelection()
{
	this->m_selectedEvent = -1;
	this->m_selectedTrack = -1;
}