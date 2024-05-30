#include "EventTrackEditor.h"
#include "../Application/Application.h"

EventTrackEditor::EventTrack::EventTrack(int numEvents, int eventId, Event* event, char* name, bool is_discrete)
{
    this->m_source = NULL;

    this->m_numEvents = numEvents;
    this->m_eventId = eventId;

    this->m_name = name;

    this->m_discrete = is_discrete;

    this->m_event = new Event[this->m_numEvents];

    for (size_t i = 0; i < numEvents; i++)
    {
        this->m_event[i].m_frameStart = event[i].m_frameStart;
        this->m_event[i].m_duration = event[i].m_duration;
        this->m_event[i].m_value = event[i].m_value;
    }
}

EventTrackEditor::EventTrack::EventTrack(MR::EventTrackDefBase* src, float len)
{
    this->m_source = src;
    this->m_numEvents = src->getNumEvents();
    this->m_eventId = src->getUserData();
    this->m_name = src->getTrackName();
    this->m_discrete = false;

    if (src->getType() == 0)
        this->m_discrete = true;

    this->m_event = new Event[this->m_numEvents];

    MR::EventTrackDefDiscrete* discreteSrc = static_cast<MR::EventTrackDefDiscrete*>(src);

    for (size_t i = 0; i < discreteSrc->getNumEvents(); i++)
    {
        this->m_event[i].m_frameStart = RMath::TimeToFrame(discreteSrc->getEvent(i)->getStartTime() * len);
        this->m_event[i].m_duration = RMath::TimeToFrame(discreteSrc->getEvent(i)->getDuration() * len);
        this->m_event[i].m_value = discreteSrc->getEvent(i)->getUserData();
    }
}

void EventTrackEditor::EventTrack::SaveEventTrackData(float len)
{    
    for (int i = 0; i < this->m_numEvents; i++)
    {
        MR::EventTrackDefDiscrete* discreteSrc = static_cast<MR::EventTrackDefDiscrete*>(this->m_source);

        discreteSrc->getEvent(i)->setStartTime(RMath::FrameToTime(this->m_event[i].m_frameStart) / len);
        discreteSrc->getEvent(i)->setDuration(RMath::FrameToTime(this->m_event[i].m_duration) / len);
        discreteSrc->getEvent(i)->setUserData(this->m_event[i].m_value);
    }
}

bool EventTrackEditor::EventTrack::IsEventActive(int event_idx, int frame)
{
    if (!this->m_discrete)
    {
        if ((frame >= this->m_event[event_idx].m_frameStart) && frame <= (this->m_event[event_idx].m_duration + this->m_event[event_idx].m_frameStart))
            return true;
    }
    else
    {
        if (frame == this->m_event[event_idx].m_frameStart)
            return true;
    }

    return false;
}


int EventTrackEditor::GetFrameMin() const
{
    return m_frameMin;
}

int EventTrackEditor::GetFrameMax() const
{
    return m_frameMax;
}

int EventTrackEditor::GetTrackCount() const { return (int)m_eventTracks.size(); }

std::string EventTrackEditor::GetTrackName(int idx) const { return this->m_eventTracks[idx].m_name; }

std::string EventTrackEditor::GetEventLabel(int track_idx, int event_idx) const
{
    return std::to_string(this->m_eventTracks[track_idx].m_event[event_idx].m_value);
}

void EventTrackEditor::AddTrack(int event_id, char* name, bool duration)
{
    /*
    MorphemeBundle_EventTrack* new_track = g_morphemeConnect.m_nmb.AddEventTrack(this->m_nodeSource, event_id, name, duration);

    RDebug::DebuggerOut(g_logLevel, MsgLevel_Debug, "Added EventTrack %d (%s) (node=%d)\n", new_track->m_signature, name, this->m_nodeSource->m_nodeID);
    */
};

void EventTrackEditor::DeleteTrack(int idx)
{
    /*
    int delete_signature = this->m_eventTracks[idx].m_signature;
    char delete_name[50];

    strcpy(delete_name, this->m_eventTracks[idx].m_name.c_str());

    while (this->m_eventTracks[idx].m_numEvents > 0)
        this->DeleteEvent(idx, 0);

    MR::AttribDataSourceAnim* source_anim = (MR::AttribDataSourceAnim*)this->m_nodeSource->m_attributes[1]->GetAttribData();
    MR::AttribDataSourceEventTrack* event_tracks = (MR::AttribDataSourceEventTrack*)this->m_nodeSource->m_attributes[2]->GetAttribData();

    if (this->m_eventTracks[idx].m_discrete)
        event_tracks->GetDiscreteEventTrackSet().DeleteEventTrack(idx);
    else
        event_tracks->GetDurationEventTrackSet().DeleteEventTrack(idx);

    std::vector<NodeDef*> nodes = g_morphemeConnect.m_nmb.GetNodesByAnimReference(source_anim->GetAnimID());

    for (int i = 0; i < nodes.size(); i++)
    {
        if (nodes[i] != this->m_nodeSource)
        {
            MR::AttribDataSourceEventTrack* event_tracks_new = (MR::AttribDataSourceEventTrack*)nodes[i]->m_attributes[2]->GetAttribData();

            event_tracks_new->SetDiscreteEventTrackSet(event_tracks->GetDiscreteEventTrackSet());
            event_tracks_new->SetCurveEventTrackSet(event_tracks->GetCurveEventTrackSet());
            event_tracks_new->SetDurationEventTrackSet(event_tracks->GetDurationEventTrackSet());
        }
    }

    this->m_reload = true;

    RDebug::DebuggerOut(g_logLevel, MsgLevel_Debug, "Deleted track %d (%s) (node=%d)\n", delete_signature, delete_name, this->m_nodeSource->m_nodeID);
    */
}

void EventTrackEditor::AddEvent(int track_idx, EventTrack::Event event)
{
    /*
    EventTrack* track = &this->m_eventTracks[track_idx];

    MR::EventTrackDefDiscrete* discreteSrc = static_cast<MR::EventTrackDefDiscrete*>(track->m_source);

    discreteSrc->
    //discreteSrc->a(RMath::FrameToTime(event.m_frameStart) / RMath::FrameToTime(this->m_frameMax), RMath::FrameToTime(event.m_duration) / RMath::FrameToTime(this->m_frameMax), event.m_value);
        
    this->m_reload = true;

    RDebug::DebuggerOut(g_logLevel, MsgLevel_Debug, "Added event to track %d (%.3f, %.3f, %d) (node=%d)\n", track->m_signature, RMath::FrameToTime(event.m_frameStart) / RMath::FrameToTime(this->m_frameMax), RMath::FrameToTime(event.m_duration) / RMath::FrameToTime(this->m_frameMax), event.m_value, this->m_nodeSource->m_nodeID);

    return;
    */
}

void EventTrackEditor::DeleteEvent(int track_idx, int event_idx)
{
    /*
    EventTrack* track = &this->m_eventTracks[track_idx];

    track->m_source->m_data->DeleteEvent(event_idx);

    RDebug::DebuggerOut(g_logLevel, MsgLevel_Debug, "Deleted event %d from Track %d (node=%d)\n", event_idx, track->m_signature, this->m_nodeSource->m_nodeID);

    this->m_reload = true;

    return;
    */
}

void EventTrackEditor::ReloadTracks()
{
    this->m_reload = false;

    this->m_eventTracks.clear();
    this->SetEditedState(false);

    /*
    if ((g_morphemeConnect.m_nmb.IsInitialised()) && (g_morphemeConnect.m_eventTrackEditorFlags.m_targetAnimIdx != -1))
    {
        bool found = false;

        MR::AttribDataSourceAnim* source_anim = (MR::AttribDataSourceAnim*)this->m_nodeSource->m_attributes[1]->GetAttribData();
        MR::AttribDataSourceEventTrack* event_track_source = (MR::AttribDataSourceEventTrack*)this->m_nodeSource->m_attributes[2]->GetAttribData();

        if (event_track_source != NULL)
        {
            for (int i = 0; i < event_track_source->GetDiscreteEventTrackSet().m_trackCount; i++)
            {
                MorphemeBundle_EventTrack* event_tracks = g_morphemeConnect.m_nmb.GetEventTrackBundle(event_track_source->GetDiscreteEventTrackSet().m_trackSignatures[i]);

                if (event_tracks)
                    this->m_eventTracks.push_back(EventTrackEditor::EventTrack(event_tracks, RMath::FrameToTime(this->m_frameMax), true));
            }

            for (int i = 0; i < event_track_source->GetCurveEventTrackSet().m_trackCount; i++)
            {
                MorphemeBundle_EventTrack* event_tracks = g_morphemeConnect.m_nmb.GetEventTrackBundle(event_track_source->GetCurveEventTrackSet().m_trackSignatures[i]);

                if (event_tracks)
                    this->m_eventTracks.push_back(EventTrackEditor::EventTrack(event_tracks, RMath::FrameToTime(this->m_frameMax), false));
            }

            for (int i = 0; i < event_track_source->GetDurationEventTrackSet().m_trackCount; i++)
            {
                MorphemeBundle_EventTrack* event_tracks = g_morphemeConnect.m_nmb.GetEventTrackBundle(event_track_source->GetDurationEventTrackSet().m_trackSignatures[i]);

                if (event_tracks)
                    this->m_eventTracks.push_back(EventTrackEditor::EventTrack(event_tracks, RMath::FrameToTime(this->m_frameMax), false));
            }
        }
    }
    */
}

EventTrackEditor::EventTrackEditor() 
{
    this->m_nodeSource = nullptr;

    INIReader reader(".//Data//res//color//eventrack.ini");

    if (reader.ParseError() < 0)
    {
        RDebug::DebuggerOut(g_logLevel, MsgLevel_Error, "EventTrackEditor.cpp", "Failed to load eventrack.ini\n");

        this->m_colors.m_trackColor = { 0.31f, 0.31f, 0.91f, 1.f };
        this->m_colors.m_trackColorInactive = { 0.22f, 0.22f, 0.44f, 1.f };
        this->m_colors.m_trackColorActive = { 0.39f, 0.39f, 1.f, 1.f };
        this->m_colors.m_trackBoundingBox = { 0.f, 0.f, 0.f, 1.f };
        this->m_colors.m_trackBoundingBoxActive = { 0.f, 0.f, 0.f, 1.f };
        this->m_colors.m_trackTextColor = { 1.f, 1.f, 1.f, 1.f };
        this->m_colors.m_cursorColor = { 0.f, 0.f, 0.f, 0.f };
    }

    this->m_colors.m_trackColor = { (float)reader.GetReal("Track", "r", 0.31f), (float)reader.GetReal("Track", "g", 0.31f), (float)reader.GetReal("Track", "b", 0.91f), (float)reader.GetReal("Track", "a", 1.f) };
    this->m_colors.m_trackColorInactive = { (float)reader.GetReal("TrackInactive", "r", 0.22f), (float)reader.GetReal("TrackInactive", "g", 0.22f), (float)reader.GetReal("TrackInactive", "b", 0.44f), (float)reader.GetReal("TrackInactive", "a", 1.f) };
    this->m_colors.m_trackColorActive = { (float)reader.GetReal("TrackActive", "r", 0.39f), (float)reader.GetReal("TrackActive", "g", 0.39f), (float)reader.GetReal("TrackActive", "b", 1.f), (float)reader.GetReal("TrackActive", "a", 1.f) };
    this->m_colors.m_trackBoundingBox = { (float)reader.GetReal("TrackBoundingBox", "r", 0.f), (float)reader.GetReal("TrackBoundingBox", "g", 0.f), (float)reader.GetReal("TrackBoundingBox", "b", 0.f), (float)reader.GetReal("TrackBoundingBox", "a", 1.f) };
    this->m_colors.m_trackBoundingBoxActive = { (float)reader.GetReal("TrackActiveBoundingBox", "r", 0.f), (float)reader.GetReal("TrackActiveBoundingBox", "g", 0.f), (float)reader.GetReal("TrackActiveBoundingBox", "b", 0.f), (float)reader.GetReal("TrackActiveBoundingBox", "a", 1.f) };
    this->m_colors.m_trackTextColor = { (float)reader.GetReal("TrackText", "r", 1.f), (float)reader.GetReal("TrackText", "g", 1.f), (float)reader.GetReal("TrackText", "b", 1.f), (float)reader.GetReal("TrackText", "a", 1.f) };
    this->m_colors.m_cursorColor = { (float)reader.GetReal("TrackCursor", "r", 0.f), (float)reader.GetReal("TrackCursor", "g", 0.f), (float)reader.GetReal("TrackCursor", "b", 0.f), (float)reader.GetReal("TrackCursor", "a", 0.f) };

}

void EventTrackEditor::SetEditedState(bool state)
{
    if (this->m_animIdx == -1)
        return;

    if (g_morphemeConnect.m_eventTrackEditorFlags.m_edited.size() > this->m_animIdx)
        g_morphemeConnect.m_eventTrackEditorFlags.m_edited[this->m_animIdx] = state;
    else
        RDebug::SystemPanic("EventTrackEditor.cpp", "Out of bound read while setting edited state (idx=%d, size=%d)\n", this->m_animIdx, g_morphemeConnect.m_eventTrackEditorFlags.m_edited.size());
}

void EventTrackEditor::Clear()
{
    this->m_animIdx = -1;
    this->m_frameMax = 0;
    this->m_frameMin = 0;
    this->m_nodeSource = nullptr;
    this->m_eventTracks.clear();
}