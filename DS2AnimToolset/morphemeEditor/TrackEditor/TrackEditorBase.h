#pragma once
#include "Track/Track.h"
#include "AnimPlayer/AnimPlayer.h"
#include "RCore.h"
#include <imgui/imgui.h>

namespace TrackEditor
{
    enum MovingPart
    {
        kMovingPartNone,
        kMovingPartStart,
        kMovingPartEnd,
        kMovingPartAll
    };

    enum LabelAlignment
    {
        kLeft,
        kCenter,
        kRight
    };

    enum TimeCodeFormat
    {
        kSeconds,
        kMilliseconds,
        kFrames
    };

    enum TrackEditorOptions
    {
        kEditorNone = 0,
        kEditorChangeEventStartEnd = 1 << 1,
        kEditorChangeFrame = 1 << 3,
        kEditorAddTrack = 1 << 4,
        kEditorAddEvent = 1 << 5,
        kEditorCopyPaste = 1 << 6,
        kEditorCollapse = 1 << 7,
        kEditorMarkActiveEvents = 1 << 8,
        kEditorRenameTrack = 1 << 9,
        kEditorHighlightSelectedTrack = 1 << 10,
        kEditorHighlightSelectedEvent = 1 << 11,
        kEditorEditAll = kEditorChangeEventStartEnd | kEditorAddTrack | kEditorAddEvent,
        kEditorHighlight = kEditorHighlightSelectedTrack | kEditorHighlightSelectedEvent
    };

    class TrackEditorBase
    {
    public:
        struct Colors
        {
            ImVec4 m_trackColor;
            ImVec4 m_trackColorInactive;
            ImVec4 m_trackColorActive;
            ImVec4 m_trackBoundingBox;
            ImVec4 m_trackBoundingBoxActive;
            ImVec4 m_highlight;
            ImVec4 m_trackTextColor;
            ImVec4 m_cursorColor;
        };

        TrackEditorBase();
        ~TrackEditorBase() {}

        int getNumTracks() const { return this->m_tracks.size(); }
        TrackEditorOptions getOptions() const { return this->m_editorOptions; }
        float getCurrentTime() const { return RMath::frameToTime(this->m_currentFrame, this->m_fps); }
        int getFps() const { return this->m_fps; }
        int getClipStart() const { return this->m_firstFrame; }
        int getClipEnd() const { return this->m_lastFrame; }
        int getFrameMin() const { return this->m_frameMin; }
        int getFrameMax() const { return this->m_frameMax; }
        bool isReload() const { return this->m_reload; }
        TimeCodeFormat getTimeCodeFormat() const { return this->m_timeCodeFormat; }
        Track* getTrack(int idx) const { return this->m_tracks[idx]; }
        Event* getSelectedEvent(int trackIdx, int eventIdx) const { return this->m_tracks[trackIdx]->events[eventIdx]; }
        bool isEdited() const { return this->m_edited; }
        Colors* getColors() { return &this->m_colors; }
        int getSelectedTrackIdx() const { return this->m_selectedTrack; }
        int getSelectedEventIdx() const { return this->m_selectedEvent; }
        Track* getSelectedTrack() const;
        Event* getSelectedEvent() const;

        void setEditorOptions(TrackEditorOptions options) { this->m_editorOptions = options; }
        void setCurrentTime(float time) { this->m_currentFrame = RMath::timeToFrame(time, this->m_fps); }
        void setFps(int fps) { this->m_fps = fps; }
        void setFrameMin(int value) { this->m_frameMin = value; }
        void setFrameMax(int value) { this->m_frameMax = value; }
        void setReload(bool status) { this->m_reload = status; }
        void setTimeCodeFormat(TimeCodeFormat format) { this->m_timeCodeFormat = format; }
        void setEdited(bool status) { this->m_edited = status; }
        void selectTrack(int trackIdx) { this->m_selectedTrack = trackIdx; }
        void selectEvent(int trackIdx, int eventIdx) { this->m_selectedTrack = trackIdx;  this->m_selectedTrack = eventIdx; }

        void loadColorsFromXML(const char* filename);
        void loadColorsFromINI(const char* filename);
        bool editorGUI();

        void registerListener(TrackEditorBase* listener);
        void notifyListeners();

        virtual void update(float dt);
        virtual void reset();
        virtual void destroy() = 0;

        virtual std::string getTrackName(int idx) = 0;
        virtual std::string getEventLabel(int trackIdx, int eventIdx, bool arguments) = 0;
        virtual void addTrack(int userData, std::string name, bool discrete) = 0;
        virtual void deleteTrack(int idx) = 0;
        virtual void addEvent(int trackIdx, float startTime, float endTime, int userData) = 0;
        virtual void deleteEvent(int trackIdx, int eventIdx) = 0;
        virtual bool addTrackGUI() = 0;
        virtual bool deleteTrackGUI() = 0;
        virtual bool addEventGUI() = 0;
        virtual bool deleteEventGUI() = 0;
        virtual bool renameTrackGUI() = 0;

        virtual bool saveTracks() = 0;

    protected:
        void clearTracks();
        virtual void reload() = 0;

        Colors m_colors;

        Track* trackBuffer;
        Event* eventBuffer;

        std::vector<Track*> m_tracks;
        void* m_source;

        LabelAlignment m_eventLabelAlignment = kCenter;
        TrackEditorOptions m_editorOptions = kEditorNone;
        TimeCodeFormat m_timeCodeFormat = kSeconds;
        int m_currentFrame = 0;
        int m_fps = 30;
        int m_firstFrame = 0;
        int m_lastFrame = 30;
        int m_frameMin = 0;
        int m_frameMax = 30;
        bool m_reload = false;
        bool m_edited = false;

        int m_selectedTrack = -1;
        int m_selectedEvent = -1;

    private:
        //Editor GUI variables
        bool m_expanded = true;

        bool m_resizeLegend = false;
        int m_legendWidth = 210;

        bool m_popupOpened = false;
        float m_zoomLevel = 10.f;

        int m_movingTrack = -1;
        int m_movingEvent = -1;
        int m_movingPos = -1;
        bool m_movingScrollBar = false;
        bool m_movingCurrentFrame = false;
        bool m_panningView = false;
        ImVec2 m_panningViewSource = ImVec2(0, 0);
        int m_panningViewFrame = 0;

        int m_movingPart = 0;

        std::vector<TrackEditorBase*> m_listeners;
    };
}