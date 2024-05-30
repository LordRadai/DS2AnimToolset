#pragma once
#include <math.h>
#include <cstddef>
#include <cstdlib>
#include <vector>
#include <string>

#include "../FromsoftFormat/TimeActReader/TimeActReader.h"
#include "../framework.h"
#include "../extern.h"

struct TimeActEditor
{
    struct TimeActTrack
    {
        struct Event
        {
            int m_frameStart = 0;
            int m_duration = 0;
            int m_value = 0;

            TimeActEventData* m_args = nullptr;
        };

        EventGroup* m_source;
        int m_count;
        int m_eventGroup;
        std::vector<Event> m_event;
        char m_name[50];

        TimeActTrack(int eventId);
        TimeActTrack(EventGroup* src);

        void SaveTimeActTrack();
        bool IsEventActive(int idx, int frame);
    };

    TimeAct* m_source;

    int m_fps = 30;
    int m_taeIdx = -1;
    std::vector<TimeActTrack> m_tracks;
    int m_frameMin, m_frameMax;
    bool focused = false;
    bool m_reload = false;

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

    TimeActEditor();

    int GetFrameMin() const;
    int GetFrameMax() const;
    int GetTrackCount() const;

    std::string GetTrackName(int idx);
    std::string GetEventLabel(int idx, int event_idx, bool arguments = true) const;

    void AddGroup(int id);
    void DeleteGroup(int idx);

    void AddEvent(int group_idx, TimeActEvent event);
    void DeleteEvent(int group_idx, int event_idx);

    void ReloadTracks();
    void SetEditedState(bool state);

    void Clear();
};
