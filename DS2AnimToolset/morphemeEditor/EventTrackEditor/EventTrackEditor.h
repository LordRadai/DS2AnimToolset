#pragma once
#include <math.h>
#include <cstddef>
#include <cstdlib>
#include <vector>
#include <string>

#include "framework.h"
#include "extern.h"
#include "MorphemeSystem/MorphemeSystem.h"

struct EventTrackEditor
{
    bool m_load = false;
    bool m_save = false;
    int m_selectedAnimIdx = -1;
    std::vector<bool> m_edited;

    int m_targetAnimIdx = -1;
    float m_lenMult = 1.f;
    bool m_loadTae = false;
    std::vector<std::wstring> m_taeList;

    int m_selectedTrack = -1;
    int m_selectedEvent = -1;

    float m_eventTrackActionTimeActStart = 0.f;
    float m_eventTrackActionTimeActDuration = 0.f;
    int m_eventTrackActionTimeActValue = -1;

    struct EventTrack
    {
        struct Event
        {
            int m_frameStart = 0;
            int m_duration = 0;
            int m_value = 0;
        };

        ME::EventTrackExport* m_source;

        int m_numEvents;
        int m_eventId;
        Event* m_event;
        std::string m_name;
        bool m_discrete;

        EventTrack(int numEvents, int eventId, Event* event, char* name, bool is_discrete);
        EventTrack(ME::EventTrackExport* src, float len);

        bool IsEventActive(int event_idx, int frame);
    };

    AnimSourceInterface* m_animSource;

    int m_fps = 60;
    int m_animIdx = -1;

    std::vector<EventTrack> m_eventTracks;
    int m_frameMin, m_frameMax;
    bool focused = false;
    bool m_reload = false;
    bool m_showTimecode = true;

    struct EventTrackColor
    {
        ImVec4 m_trackColor;
        ImVec4 m_trackColorInactive;
        ImVec4 m_trackColorActive;
        ImVec4 m_trackBoundingBox;
        ImVec4 m_trackBoundingBoxActive;
        ImVec4 m_trackTextColor;
        ImVec4 m_cursorColor;
    } m_colors;

    EventTrackEditor();

    int GetFrameMin() const;
    int GetFrameMax() const;
    int GetTrackCount() const;

    std::string GetTrackName(int idx) const;
    std::string GetEventLabel(int track_idx, int event_idx) const;

    void AddTrack(int event_id, char* name, bool duration);
    void DeleteTrack(int idx);

    void AddEvent(int track_idx, EventTrack::Event event);
    void DeleteEvent(int track_idx, int event_idx);

    void ReloadTracks();
    void SetEditedState(bool state);

    void SaveEventTracks();

    void Clear();

    void ResetSelection();
};
