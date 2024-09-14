#include "TrackEditorBase.h"
#include <Windows.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imstb_rectpack.h>
#include "extern.h"
#include "framework.h"

namespace
{
    int drawDurationEvent(ImDrawList* draw_list, ImVec2 startOffset, int itemHeight, float framePixelWidth, int frameStart, int frameEnd, std::string label, TrackEditor::LabelAlignment alignment, ImU32 trackColor, ImU32 boundColor, ImU32 textColor, bool interactable)
    {
        ImVec2 slotP1(startOffset.x + frameStart * framePixelWidth - 1, startOffset.y + 2);
        ImVec2 slotP2(startOffset.x + frameEnd * framePixelWidth - 1, startOffset.y + itemHeight - 2);

        ImVec2 textSize = ImGui::CalcTextSize(label.c_str());

        ImVec2 textPos(0, 0);

        switch (alignment)
        {
        case TrackEditor::kLeft:
            textPos = ImVec2(slotP1.x + 2, slotP2.y + (slotP1.y - slotP2.y - textSize.y) / 2);
            break;
        case TrackEditor::kCenter:
            textPos = ImVec2(slotP1.x + (slotP2.x - slotP1.x - textSize.x) / 2, slotP2.y + (slotP1.y - slotP2.y - textSize.y) / 2);
            break;
        case TrackEditor::kRight:
            textPos = ImVec2(slotP1.x + (slotP2.x - slotP1.x - textSize.x) - 2, slotP2.y + (slotP1.y - slotP2.y - textSize.y) / 2);
            break;
        default:
            break;
        }

        if (textPos.x < startOffset.x)
            label = "<-" + label;

        draw_list->AddRectFilled(slotP1, slotP2, trackColor, 0);
        draw_list->AddRect(slotP1, slotP2, boundColor, 0);
        draw_list->AddText(textPos, textColor, label.c_str()); //Event Value

        if (!interactable)
            return 0;

        float resizeHandleWidth = std::fmax(framePixelWidth / 3, 3);

        ImRect rects[3] = { ImRect(ImVec2(slotP1.x - resizeHandleWidth, slotP1.y), ImVec2(slotP1.x, slotP2.y))
        , ImRect(ImVec2(slotP2.x, slotP1.y), ImVec2(slotP2.x + resizeHandleWidth, slotP2.y))
        , ImRect(slotP1, slotP2) };

        for (size_t i = 0; i < 3; i++)
        {
            ImRect& rc = rects[i];

            if (!rc.Contains(ImGui::GetIO().MousePos))
                continue;

            switch (i)
            {
            case 0:
            case 1:
                if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
                break;
            case 2:
                if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                break;
            default:
                break;
            }

            if (ImGui::IsMouseClicked(0))
                return i + 1;
        }

        return 0;
    }

    int drawDiscreteEvent(ImDrawList* draw_list, ImVec2 startOffset, int itemHeight, float framePixelWidth, int frameStart, std::string label, int idx, TrackEditor::LabelAlignment alignment, ImU32 trackColor, ImU32 boundColor, ImU32 textColor, bool interactable)
    {
        ImVec2 slotD1(startOffset.x + frameStart * framePixelWidth - 6, startOffset.y + 2);
        ImVec2 slotD2(startOffset.x + frameStart * framePixelWidth + 4, startOffset.y + itemHeight - 7);
        ImVec2 slotT1 = ImVec2(slotD1.x, startOffset.y + itemHeight - 7);
        ImVec2 slotT2 = ImVec2(slotD2.x, startOffset.y + itemHeight - 7);
        ImVec2 slotT3 = ImVec2((slotT2.x + slotT1.x) / 2, (slotT2.y + slotT1.y) / 2 + 5);

        ImVec2 textSize = ImGui::CalcTextSize(label.c_str());
        ImVec2 textSizeIdx = ImGui::CalcTextSize(std::to_string(idx).c_str());

        ImVec2 textPos(0, 0);
        ImVec2 textPosIdx(0, 0);

        switch (alignment)
        {
        case TrackEditor::kLeft:
            textPosIdx = ImVec2(slotD1.x + 2, slotD2.y + (slotD1.y - slotD2.y - textSizeIdx.y) / 2);
            textPos = ImVec2(slotD2.x + 2, slotT3.y  - textSize.y);
            break;
        case TrackEditor::kCenter:
            textPosIdx = ImVec2(slotD1.x + (slotD2.x - slotD1.x - textSizeIdx.x) / 2, slotD2.y + (slotD1.y - slotD2.y - textSizeIdx.y) / 2);
            textPos = ImVec2(slotD2.x + 2, slotT3.y - textSize.y);
            break;
        case TrackEditor::kRight:
            textPosIdx = ImVec2(slotD1.x + (slotD2.x - slotD1.x - textSizeIdx.x) - 2, slotD2.y + (slotD1.y - slotD2.y - textSizeIdx.y) / 2);
            textPos = ImVec2(slotD2.x + 2, slotT3.y - textSize.y);
            break;
        default:
            break;
        }

        draw_list->AddRectFilled(slotD1, slotD2, trackColor, 0); //Track Box
        draw_list->AddLine(slotD1, ImVec2(slotD2.x, slotD1.y), boundColor);
        draw_list->AddLine(slotD2, ImVec2(slotD2.x, slotD1.y), boundColor);
        draw_list->AddLine(slotD1, ImVec2(slotD1.x, slotD2.y), boundColor);

        draw_list->AddTriangleFilled(slotT1, slotT2, slotT3, trackColor);
        draw_list->AddLine(slotT1, slotT3, boundColor);
        draw_list->AddLine(slotT2, slotT3, boundColor);

        ImRect track_box(slotD1, slotD2);

        draw_list->AddText(textPosIdx, textColor, std::to_string(idx).c_str()); //Event Value
        draw_list->AddText(textPos, textColor, label.c_str());           //Event Idx

        if (!interactable)
            return 0;

        ImRect rect_discrete = ImRect(slotD1, slotD2);

        if (rect_discrete.Contains(ImGui::GetMousePos()))
        {
            if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

            if (ImGui::IsMouseClicked(0))
            {
                if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                return 3;
            }
        }

        return 0;
    }

    bool editorAddDeleteButton(ImDrawList* draw_list, ImVec2 pos, ImVec2 size, bool add = true)
    {
        ImGuiIO& io = ImGui::GetIO();

        ImRect delRect(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        bool overDel = delRect.Contains(io.MousePos);

        int delColor = 0xFFFF9E61;

        if (overDel && io.MouseDown[0])
            delColor = 0xFFFFC099;

        float midy = pos.y + size.x / 2 - 0.5f;
        float midx = pos.x + size.y / 2 - 0.5f;

        //draw_list->AddRectFilled(delRect.Min, delRect.Max, buttonCol, 0);
        //draw_list->AddRect(delRect.Min, delRect.Max, delColor, 0);
        if (add)
        {
            draw_list->AddLine(ImVec2(delRect.Min.x + 5, midy), ImVec2(delRect.Max.x - 5, midy), 0xFF000000, 5);
            draw_list->AddLine(ImVec2(midx, delRect.Min.y + 5), ImVec2(midx, delRect.Max.y - 5), 0xFF000000, 5);

            draw_list->AddLine(ImVec2(delRect.Min.x + 3, midy), ImVec2(delRect.Max.x - 3, midy), delColor, 2);
            draw_list->AddLine(ImVec2(midx, delRect.Min.y + 3), ImVec2(midx, delRect.Max.y - 3), delColor, 2);
        }
        else
        {
            draw_list->AddLine(ImVec2(delRect.Min.x + 5, midy), ImVec2(delRect.Max.x - 5, midy), 0xFF000000, 5);

            draw_list->AddLine(ImVec2(delRect.Min.x + 3, midy), ImVec2(delRect.Max.x - 3, midy), delColor, 2);
        }

        return overDel;
    }

    bool editorRemoveButton(ImDrawList* draw_list, ImVec2 pos, ImVec2 size)
    {
        ImGuiIO& io = ImGui::GetIO();

        ImRect delRect(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        bool overDel = delRect.Contains(io.MousePos);

        int delColor = 0xFFFF9E61;

        if (overDel && io.MouseDown[0])
            delColor = 0xFFFFC099;

        //draw_list->AddRectFilled(delRect.Min, delRect.Max, buttonCol, 0);
        //draw_list->AddRect(delRect.Min, delRect.Max, delColor, 0);

        draw_list->AddLine(ImVec2(delRect.Min.x + 5, delRect.Min.y + 5), ImVec2(delRect.Max.x - 5, delRect.Max.y - 5), 0xFF000000, 5);
        draw_list->AddLine(ImVec2(delRect.Min.x + 5, delRect.Max.y - 5), ImVec2(delRect.Max.x - 5, delRect.Min.y + 5), 0xFF000000, 5);

        draw_list->AddLine(ImVec2(delRect.Min.x + 3, delRect.Min.y + 3), ImVec2(delRect.Max.x - 3, delRect.Max.y - 3), delColor, 2);
        draw_list->AddLine(ImVec2(delRect.Min.x + 3, delRect.Max.y - 3), ImVec2(delRect.Max.x - 3, delRect.Min.y + 3), delColor, 2);

        return overDel;
    }

    bool editorAddTrackButton(ImDrawList* draw_list, ImVec2 pos, ImVec2 size)
    {
        ImGuiIO& io = ImGui::GetIO();

        ImVec2 textSize = ImGui::CalcTextSize("Add Track");
        pos.x -= textSize.x;

        ImRect delRect(ImVec2(pos.x - size.x, pos.y), ImVec2(pos.x + textSize.x + size.x, pos.y + size.y));
        ImVec2 textP(delRect.Min.x + (delRect.Max.x - delRect.Min.x - textSize.x) / 2, delRect.Max.y + (delRect.Min.y - delRect.Max.y - textSize.y) / 2);

        bool overDel = delRect.Contains(io.MousePos);

        int buttonCol = 0xFF303030;

        if (overDel && io.MouseDown[0])
            buttonCol = 0xFF606060;

        draw_list->AddRectFilled(delRect.Min, delRect.Max, buttonCol, 0);
        draw_list->AddRect(delRect.Min, delRect.Max, 0xFF000000, 0);
        draw_list->AddText(textP, 0xFFFFFFFF, "Add Track");

        return overDel;
    }

    bool editorTrackLabel(ImDrawList* draw_list, ImVec2 pos, ImVec2 size, char* label)
    {
        ImGuiIO& io = ImGui::GetIO();

        ImVec2 textSize = ImGui::CalcTextSize(label);

        ImRect delRect(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + size.y));

        bool overDel = delRect.Contains(io.MousePos);

        int buttonCol = 0xFF303030;

        draw_list->AddRectFilled(delRect.Min, delRect.Max, buttonCol, 0);
        draw_list->AddRect(delRect.Min, delRect.Max, 0xFF000000, 0);

        ImGui::RenderTextClipped(delRect.Min + ImVec2(5, 0), delRect.Max - ImVec2(5, 0), label, NULL, &textSize, ImVec2(0.f, 0.5f), &delRect);

        return overDel;
    }

    inline float calculateZoomLevel(int width, int targetNumFrames)
    {
        return (float)width / (float)targetNumFrames;
    }
}

namespace TrackEditor
{
#ifndef IMGUI_DEFINE_MATH_OPERATORS
    static ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
        return ImVec2(a.x + b.x, a.y + b.y);
    }
#endif

    TrackEditorBase::TrackEditorBase()
    {
        this->trackBuffer = new Track;
        this->eventBuffer = new Event;
    }

    Track* TrackEditorBase::getSelectedTrack() const
    { 
        if (this->m_selectedTrack != -1 && (this->m_selectedTrack < this->m_tracks.size()))
            return this->m_tracks[this->m_selectedTrack];

        return nullptr;
    }

    Event* TrackEditorBase::getSelectedEvent() const
    {
        Track* track = this->getSelectedTrack();

        if (track != nullptr && (this->m_selectedEvent < track->events.size()))
            return this->m_tracks[this->m_selectedTrack]->events[this->m_selectedEvent];

        return nullptr;
    }

    void TrackEditorBase::loadColorsFromXML(const char* filename)
    {
        RXML::XMLFileObj* xml = RXML::XMLFileObj::create(filename);

        if (xml == nullptr)
        {
            g_appLog->alertMessage(MsgLevel_Error, "Failed to load %s\n", filename);

            this->m_colors.m_trackColor = { 0.33f, 0.33f, 0.33f, 1.f };
            this->m_colors.m_trackColorInactive = { 0.33f, 0.33f, 0.33f, 1.f };
            this->m_colors.m_trackColorActive = { 0.f, 0.4f, 0.8f, 1.f };
            this->m_colors.m_trackBoundingBox = { 0.f, 0.f, 0.f, 1.f };
            this->m_colors.m_trackBoundingBoxActive = { 1.f, 1.f, 0.f, 1.f };
            this->m_colors.m_highlight = { 1.f,0.f, 0.f, 1.f };
            this->m_colors.m_trackTextColor = { 1.f, 1.f, 1.f, 1.f };
            this->m_colors.m_cursorColor = { 0.f, 0.f, 0.f, 1.f };
        }

        RXML::getVector4Element(xml->getRootElement()->getXmlElement(), "Highlight", &this->m_colors.m_highlight.x);
        RXML::getVector4Element(xml->getRootElement()->getXmlElement(), "Track", &this->m_colors.m_trackColor.x);
        RXML::getVector4Element(xml->getRootElement()->getXmlElement(), "TrackInactive", &this->m_colors.m_trackColorInactive.x);
        RXML::getVector4Element(xml->getRootElement()->getXmlElement(), "TrackActive", &this->m_colors.m_trackColorActive.x);
        RXML::getVector4Element(xml->getRootElement()->getXmlElement(), "TrackBoundingBox", &this->m_colors.m_trackBoundingBox.x);
        RXML::getVector4Element(xml->getRootElement()->getXmlElement(), "TrackActiveBoundingBox", &this->m_colors.m_trackBoundingBoxActive.x);
        RXML::getVector4Element(xml->getRootElement()->getXmlElement(), "TrackText", &this->m_colors.m_trackTextColor.x);
        RXML::getVector4Element(xml->getRootElement()->getXmlElement(), "TrackCursor", &this->m_colors.m_cursorColor.x);
    }

    void TrackEditorBase::loadColorsFromINI(const char* filename)
    {
        this->m_source = nullptr;
        RINI* ini = RINI::open(filename);

        if (ini == nullptr)
        {
            g_appLog->alertMessage(MsgLevel_Error, "Failed to load %s\n", filename);

            this->m_colors.m_trackColor = { 0.33f, 0.33f, 0.33f, 1.f };
            this->m_colors.m_trackColorInactive = { 0.33f, 0.33f, 0.33f, 1.f };
            this->m_colors.m_trackColorActive = { 0.f, 0.4f, 0.8f, 1.f };
            this->m_colors.m_trackBoundingBox = { 0.f, 0.f, 0.f, 1.f };
            this->m_colors.m_trackBoundingBoxActive = { 1.f, 1.f, 0.f, 1.f };
            this->m_colors.m_highlight = { 1.f,0.f, 0.f, 1.f };
            this->m_colors.m_trackTextColor = { 1.f, 1.f, 1.f, 1.f };
            this->m_colors.m_cursorColor = { 0.f, 0.f, 0.f, 1.f };
        }

        this->m_colors.m_highlight = { (float)ini->getFloat("Highlight", "r", 1.f), (float)ini->getFloat("Highlight", "g", 0.f), (float)ini->getFloat("Highlight", "b", 0.f), (float)ini->getFloat("Highlight", "a", 1.f) };
        this->m_colors.m_trackColor = { (float)ini->getFloat("Track", "r", 0.33f), (float)ini->getFloat("Track", "g", 0.33f), (float)ini->getFloat("Track", "b", 0.33f), (float)ini->getFloat("Track", "a", 1.f) };
        this->m_colors.m_trackColorInactive = { (float)ini->getFloat("TrackInactive", "r", 0.33f), (float)ini->getFloat("TrackInactive", "g", 0.33f), (float)ini->getFloat("TrackInactive", "b", 0.33f), (float)ini->getFloat("TrackInactive", "a", 1.f) };
        this->m_colors.m_trackColorActive = { (float)ini->getFloat("TrackActive", "r", 0.f), (float)ini->getFloat("TrackActive", "g", 0.4f), (float)ini->getFloat("TrackActive", "b", 0.8f), (float)ini->getFloat("TrackActive", "a", 1.f) };
        this->m_colors.m_trackBoundingBox = { (float)ini->getFloat("TrackBoundingBox", "r", 0.f), (float)ini->getFloat("TrackBoundingBox", "g", 0.f), (float)ini->getFloat("TrackBoundingBox", "b", 0.f), (float)ini->getFloat("TrackBoundingBox", "a", 1.f) };
        this->m_colors.m_trackBoundingBoxActive = { (float)ini->getFloat("TrackActiveBoundingBox", "r", 1.f), (float)ini->getFloat("TrackActiveBoundingBox", "g", 1.f), (float)ini->getFloat("TrackActiveBoundingBox", "b", 0.f), (float)ini->getFloat("TrackActiveBoundingBox", "a", 1.f) };
        this->m_colors.m_trackTextColor = { (float)ini->getFloat("TrackText", "r", 1.f), (float)ini->getFloat("TrackText", "g", 1.f), (float)ini->getFloat("TrackText", "b", 1.f), (float)ini->getFloat("TrackText", "a", 1.f) };
        this->m_colors.m_cursorColor = { (float)ini->getFloat("TrackCursor", "r", 1.f), (float)ini->getFloat("TrackCursor", "g", 1.f), (float)ini->getFloat("TrackCursor", "b", 1.f), (float)ini->getFloat("TrackCursor", "a", 1.f) };
        
        ini->destroy();
    }

    void TrackEditorBase::update(float dt)
    {
        if (this->m_lastFrame > this->m_frameMax)
            this->m_lastFrame = this->m_frameMax;
        else if (this->m_lastFrame < this->m_frameMin)
            this->m_lastFrame = this->m_frameMin;

        if (this->m_firstFrame > this->m_frameMax)
            this->m_firstFrame = this->m_frameMax;
        else if (this->m_firstFrame < this->m_frameMin)
            this->m_firstFrame = this->m_frameMin;

        if (this->m_currentFrame < this->m_firstFrame)
            this->m_currentFrame = this->m_firstFrame;

        if (this->m_currentFrame > this->m_lastFrame)
            this->m_currentFrame = this->m_lastFrame;

        if (this->m_currentFrame < this->m_firstFrame)
            this->m_currentFrame = this->m_firstFrame;

        for (size_t i = 0; i < this->m_tracks.size(); i++)
        {
            for (size_t j = 0; j < this->m_tracks[i]->events.size(); j++)
            {
                Event* event = this->m_tracks[i]->events[j];

                if (!this->m_tracks[i]->discrete)
                {
                    if (event->frameEnd < this->m_firstFrame)
                        event->frameEnd = this->m_firstFrame;

                    if (event->frameStart > this->m_lastFrame)
                        event->frameStart = this->m_lastFrame;

                    if (event->frameStart > event->frameEnd)
                        event->frameStart = event->frameEnd;
                }
                else
                {
                    if (event->frameStart < this->m_firstFrame)
                        event->frameStart = this->m_firstFrame;

                    if (event->frameStart > this->m_lastFrame)
                        event->frameStart = this->m_lastFrame;
                }
            }
        }
    }

    void TrackEditorBase::reset()
    {
        this->clearTracks();

        this->m_source = nullptr;

        this->m_edited = false;
        this->m_frameMin = 0;
        this->m_frameMax = 0;

        this->m_firstFrame = 0;
        this->m_lastFrame = this->m_frameMax;

        this->m_selectedTrack = -1;
        this->m_selectedEvent = -1;
    }

    bool TrackEditorBase::editorGUI()
    {
        const bool hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
        const bool focused = ImGui::IsWindowFocused(ImGuiHoveredFlags_RootAndChildWindows);

        bool ret = false;

        ImGuiIO& io = ImGui::GetIO();
        int mouseX = (int)(io.MousePos.x);
        int mouseY = (int)(io.MousePos.y);

        bool reload = false;

        bool addTrack = false;
        bool removeTrack = false;
        bool renameTrack = false;
        bool addEvent = false;
        bool delEvent = false;

        const int ItemHeight = 23;

        ImGui::BeginGroup();

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
        ImVec2 availableSpace = ImGui::GetWindowSize();        // Resize canvas to what's available

        const int firstFrameUsed = this->m_firstFrame;
        const int controlHeight = ImMax(this->getNumTracks() * ItemHeight, ItemHeight);
        const int frameCount = ImMax((int)((this->m_frameMax - this->m_frameMin) * 1.1f), 30);

        availableSpace.y = controlHeight + 2.5 * ItemHeight;

        const int editorCanvasSize = (availableSpace.x - this->m_legendWidth);

        this->m_zoomLevel = std::fmax(calculateZoomLevel(editorCanvasSize, frameCount), 1.f);

        float framePixelWidth = this->m_zoomLevel;
        float framePixelWidthTarget = this->m_zoomLevel;

        struct CustomDraw
        {
            int index;
            ImRect customRect;
            ImRect legendResizeRect;
            ImRect clippingRect;
            ImRect legendClippingRect;
        };
        ImVector<CustomDraw> customDraws;
        ImVector<CustomDraw> compactCustomDraws;

        // zoom in/out
        const int visibleFrameCount = (int)floorf(editorCanvasSize / framePixelWidth);
        const float barWidthRatio = ImMin(visibleFrameCount / (float)frameCount, 1.f);
        const float barWidthInPixels = barWidthRatio * editorCanvasSize;

        ImRect regionRect(canvas_pos, canvas_pos + availableSpace);

        if (ImGui::IsWindowFocused() && io.KeyAlt && io.MouseDown[2])
        {
            if (!this->m_panningView)
            {
                this->m_panningViewSource = io.MousePos;
                this->m_panningView = true;
                this->m_panningViewFrame = this->m_firstFrame;
            }
            this->m_firstFrame = this->m_panningViewFrame - int((io.MousePos.x - this->m_panningViewSource.x) / framePixelWidth);
            this->m_firstFrame = ImClamp(this->m_firstFrame, this->m_frameMin, this->m_frameMax - visibleFrameCount);
        }

        if (this->m_panningView && !io.MouseDown[2])
            this->m_panningView = false;

        framePixelWidthTarget = ImClamp(framePixelWidthTarget, 0.1f, 50.f);

        framePixelWidth = ImLerp(framePixelWidth, framePixelWidthTarget, 0.33f);

        if (visibleFrameCount >= frameCount)
            this->m_firstFrame = this->m_frameMin;

        // --
        if (!this->m_expanded)
        {
            ImGui::InvisibleButton("canvas", ImVec2(availableSpace.x - canvas_pos.x, (float)ItemHeight));
            draw_list->AddRectFilled(canvas_pos, ImVec2(availableSpace.x + canvas_pos.x, canvas_pos.y + ItemHeight), 0xFF3D3837, 0);
            char tmps[512];
            ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%d Frames / %d tracks", frameCount, this->getNumTracks());
            draw_list->AddText(ImVec2(canvas_pos.x + 26, canvas_pos.y + 2), 0xFFFFFFFF, tmps);
        }
        else
        {
            bool hasScrollBar(false);

            // test scroll area
            ImVec2 headerSize(availableSpace.x, (float)ItemHeight);
            ImVec2 scrollBarSize(availableSpace.x, 14.f);
            ImGui::InvisibleButton("topBar", headerSize);
            draw_list->AddRectFilled(canvas_pos, canvas_pos + headerSize, 0xFFFF0000, 0);
            ImVec2 childFramePos = ImGui::GetCursorScreenPos();
            ImVec2 childFrameSize(availableSpace.x, availableSpace.y - 8.f - headerSize.y - (hasScrollBar ? scrollBarSize.y : 0));

            ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);

            ImGui::BeginChild("trackEditorCanvas", childFrameSize);
            ImGui::InvisibleButton("contentBar", ImVec2(availableSpace.x, float(controlHeight)));
            const ImVec2 contentMin = ImGui::GetItemRectMin();
            const ImVec2 contentMax = ImGui::GetItemRectMax();
            const ImRect contentRect(contentMin, contentMax);
            const float contentHeight = contentMax.y - contentMin.y;

            const int editorCanvasStart = contentMin.x + this->m_legendWidth;

            // full background
            draw_list->AddRectFilled(canvas_pos, canvas_pos + availableSpace, 0xFF242424, 0);

            // current frame top
            ImRect topRect(ImVec2(canvas_pos.x + this->m_legendWidth, canvas_pos.y), ImVec2(canvas_pos.x + availableSpace.x, canvas_pos.y + ItemHeight));
            ImRect legendResizeRect(ImVec2(canvas_pos.x + this->m_legendWidth - 2, canvas_pos.y), ImVec2(canvas_pos.x + this->m_legendWidth + 2, canvas_pos.y + ItemHeight));

            if (hovered && focused && (!this->m_popupOpened && !this->m_movingCurrentFrame && !this->m_movingScrollBar && this->m_movingTrack == -1 && legendResizeRect.Contains(io.MousePos)) || this->m_resizeLegend)
            {
                if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

                if (io.MouseDown[0])
                    this->m_resizeLegend = true;
            }

            if (this->m_resizeLegend)
                this->m_legendWidth = (int)((io.MousePos.x - contentMin.x));

            if (this->m_resizeLegend && io.MouseReleased[0])
                this->m_resizeLegend = false;

            //header
            draw_list->AddRectFilled(canvas_pos, ImVec2(availableSpace.x + canvas_pos.x, canvas_pos.y + ItemHeight), 0xFF404040, 0);

            if (focused && this->m_editorOptions & kEditorChangeFrame && ((topRect.Contains(io.MousePos) && !this->m_popupOpened && !this->m_movingScrollBar && this->m_movingTrack == -1 && !legendResizeRect.Contains(io.MousePos) && !this->m_resizeLegend) || this->m_movingCurrentFrame))
            {
                if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                if (!this->m_resizeLegend && !this->m_popupOpened && !this->m_movingScrollBar && this->m_movingTrack == -1 && io.MouseDown[0])
                    this->m_movingCurrentFrame = true;
            }
            else
            {
                this->m_movingCurrentFrame = false;
            }

            if (this->m_movingCurrentFrame)
            {
                if (frameCount)
                {
                    this->m_currentFrame = (int)((io.MousePos.x - topRect.Min.x) / framePixelWidth) + firstFrameUsed;

                    if (this->m_currentFrame < this->m_firstFrame)
                        this->m_currentFrame = this->m_firstFrame;
                    if (this->m_currentFrame >= this->m_lastFrame)
                        this->m_currentFrame = this->m_lastFrame;
                }

                if (!io.MouseDown[0])
                    this->m_movingCurrentFrame = false;

                this->notifyListeners();
            }

            //header frame number and lines
            int modFrameCount = frameCount * 4;
            int frameStep = 1;
            while ((modFrameCount * framePixelWidth) < 150)
            {
                modFrameCount *= 2;
                frameStep *= 2;
            };
            int halfModFrameCount = modFrameCount / 2;

            auto drawLine = [&](int i, int regionHeight) {
                bool baseIndex = ((i % modFrameCount) == 0) || (i == this->getFrameMax() || i == this->getFrameMin());
                bool halfIndex = (i % halfModFrameCount) == 0;
                ImVec2 pos = ImVec2(editorCanvasStart - firstFrameUsed * framePixelWidth, contentMin.y + ItemHeight * i + 1);
                float px = pos.x + i * framePixelWidth - 1;
                //int px = (int)canvas_pos.x + int(i * framePixelWidth) + this->m_legendWidth + int(firstFrameUsed * framePixelWidth) + 3;
                int tiretStart = baseIndex ? 4 : (halfIndex ? 10 : 14);
                int tiretEnd = baseIndex ? regionHeight : ItemHeight;

                if (px <= (availableSpace.x + canvas_pos.x) && px >= (canvas_pos.x + this->m_legendWidth))
                {
                    draw_list->AddLine(ImVec2((float)px, canvas_pos.y + (float)tiretStart), ImVec2((float)px, canvas_pos.y + (float)tiretEnd - 1), 0xFF606060, 1);

                    draw_list->AddLine(ImVec2((float)px, canvas_pos.y + (float)ItemHeight), ImVec2((float)px, canvas_pos.y + (float)regionHeight - 1), 0x30606060, 1);
                }

                if (baseIndex && px > (canvas_pos.x + this->m_legendWidth))
                {
                    char tmps[512];

                    switch (this->getTimeCodeFormat())
                    {
                    case TrackEditor::kSeconds:
                        ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%.3f", RMath::frameToTime(i, this->getFps()));
                        break;
                    case TrackEditor::kMilliseconds:
                        ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%.3f", RMath::frameToTime(i, this->getFps()) * 1000.f);
                        break;
                    case TrackEditor::kFrames:
                        ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%d", i);
                        break;
                    default:
                        throw("Unhandled TimeCode format (%d)", this->getTimeCodeFormat());
                        break;
                    }

                    draw_list->AddText(ImVec2((float)(px + 6), canvas_pos.y), 0xFFFFFFFF, tmps);
                }
            };

            auto drawLineContent = [&](int i, int /*regionHeight*/) {
                //int px = (int)canvas_pos.x + int(i * framePixelWidth) + this->m_legendWidth + int(firstFrameUsed * framePixelWidth) + 3;
                ImVec2 pos = ImVec2(editorCanvasStart - firstFrameUsed * framePixelWidth, contentMin.y + ItemHeight * i + 1);
                float px = pos.x + i * framePixelWidth - 1;
                int tiretStart = int(contentMin.y);
                int tiretEnd = int(contentMax.y);

                if (px <= (availableSpace.x + canvas_pos.x) && px >= (canvas_pos.x + this->m_legendWidth))
                {
                    //draw_list->AddLine(ImVec2((float)px, canvas_pos.y + (float)tiretStart), ImVec2((float)px, canvas_pos.y + (float)tiretEnd - 1), 0xFF606060, 1);

                    draw_list->AddLine(ImVec2(float(px), float(tiretStart)), ImVec2(float(px), float(tiretEnd)), 0x30606060, 1);
                }
            };

            for (int i = this->m_frameMin + 1; i <= this->m_frameMax; i += frameStep)
                drawLine(i, ItemHeight);

            drawLine(this->m_frameMin, ItemHeight);
            drawLine(this->m_frameMax, ItemHeight);

            for (int i = this->m_frameMax + 1; i <= 500; i += frameStep)
                drawLine(i, ItemHeight);

            // clip content
            draw_list->PushClipRect(childFramePos, childFramePos + childFrameSize, true);

            // draw item names in the legend rect on the left
            size_t customHeight = 0;
            for (int i = 0; i < this->getNumTracks(); i++)
            {
                ImVec2 tpos(contentMin.x + 3, contentMin.y + i * ItemHeight + 2 + customHeight);

                ImVec2 labelEndPos = ImVec2(this->m_legendWidth - 15, ItemHeight * 0.9f);

                if (this->m_editorOptions & kEditorAddTrack)
                    labelEndPos.x -= ItemHeight;

                if (this->m_editorOptions & kEditorAddEvent)
                    labelEndPos.x -= ItemHeight;

                bool overLabel = editorTrackLabel(draw_list, ImVec2(tpos.x, tpos.y), labelEndPos, this->m_tracks[i]->name);

                if (this->m_editorOptions & kEditorAddTrack)
                {
                    if (editorRemoveButton(draw_list, ImVec2(editorCanvasStart - ItemHeight - ItemHeight + 2 - 10, tpos.y + 2), ImVec2(ItemHeight * 0.8f, ItemHeight * 0.8f)) && focused && !this->m_popupOpened && !this->m_movingCurrentFrame && !this->m_movingScrollBar && this->m_movingTrack == -1 && !legendResizeRect.Contains(io.MousePos))
                    {
                        if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                        if (io.MouseReleased[0])
                        {
                            this->m_selectedTrack = i;
                            this->m_selectedEvent = -1;
                            this->m_popupOpened = true;

                            removeTrack = true;
                        }
                    }
                }

                if (overLabel && focused && !this->m_popupOpened && !this->m_movingCurrentFrame && !this->m_movingScrollBar && this->m_movingTrack == -1)
                {
                    if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                    if (io.MouseReleased[0])
                    {
                        this->m_selectedTrack = i;
                        this->m_selectedEvent = -1;
                    }

                    if (this->m_editorOptions & kEditorRenameTrack)
                    {
                        if (io.MouseReleased[1])
                        {
                            renameTrack = true;
                        }
                    }
                }

                if (this->m_editorOptions & kEditorAddEvent)
                {
                    bool addButton = editorAddDeleteButton(draw_list, ImVec2(editorCanvasStart - ItemHeight + 2 - 10, tpos.y + 2), ImVec2(ItemHeight * 0.8f, ItemHeight * 0.8f), true);

                    if (addButton && focused && !this->m_popupOpened && !this->m_movingCurrentFrame && !this->m_movingScrollBar && this->m_movingTrack == -1)
                    {
                        if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                        if (io.MouseReleased[0])
                        {
                            this->m_selectedTrack = i;
                            this->m_selectedEvent = -1;

                            addEvent = true;
                        }
                    }

                    if (!this->m_popupOpened && !this->m_movingCurrentFrame && !this->m_movingScrollBar && this->m_movingTrack == -1)
                    {
                        for (size_t j = 0; j < this->m_tracks[i]->events.size(); j++)
                        {
                            if (this->m_selectedTrack == i && this->m_selectedEvent == j && focused && hovered)
                            {
                                if (RInput::isKeyStateChanged(VK_DELETE))
                                    delEvent = true;
                            }
                        }
                    }
                }
            }

            if (!reload)
            {
                // clipping rect so items bars are not visible in the legend on the left when scrolled

                // slots background
                for (int i = 0; i < this->getNumTracks(); i++)
                {
                    unsigned int col = 0xFF313131;

                    ImVec2 pos = ImVec2(editorCanvasStart, contentMin.y + ItemHeight * i + 1 + customHeight);
                    ImVec2 sz = ImVec2(availableSpace.x + canvas_pos.x, pos.y + ItemHeight - 1);

                    draw_list->AddRectFilled(pos, sz, col, 0);

                    if (!this->m_popupOpened && mouseY >= pos.y && mouseY < pos.y + ItemHeight && this->m_movingTrack == -1 && mouseX>contentMin.x && mouseX < contentMin.x + availableSpace.x)
                    {
                        draw_list->AddRectFilled(pos, sz, 0x10FFFFFF, 0);
                    }
                }

                if (this->m_editorOptions & kEditorHighlightSelectedTrack)
                {
                    if (this->m_selectedTrack != -1 && this->m_selectedEvent == -1)
                    {
                        ImVec2 trackMin = ImVec2(contentMin.x + 3, contentMin.y + this->m_selectedTrack * ItemHeight + 2 + customHeight);
                        ImVec2 trackMax = ImVec2(trackMin.x + this->m_legendWidth + this->m_frameMax * framePixelWidth, trackMin.y + ItemHeight - 1);

                        draw_list->AddRect(trackMin, trackMax, ImGui::ColorConvertFloat4ToU32(this->m_colors.m_highlight));
                    }
                }

                draw_list->PushClipRect(childFramePos + ImVec2(float(this->m_legendWidth - 5), 0.f), childFramePos + childFrameSize, true);

                // vertical frame lines in content area
                for (int i = this->m_frameMin + 1; i <= this->m_frameMax; i += frameStep)
                    drawLineContent(i, int(contentHeight));

                drawLineContent(this->m_frameMin, int(contentHeight));
                drawLineContent(this->m_frameMax, int(contentHeight));

                for (int i = this->m_frameMax + 1; i <= 500; i += frameStep)
                    drawLineContent(i, ItemHeight);

                draw_list->AddRectFilled(ImVec2(editorCanvasStart - firstFrameUsed * framePixelWidth + this->m_frameMax * framePixelWidth, canvas_pos.y), canvas_pos + availableSpace, 0x40000000, 0);

                // tracks
                customHeight = 0;

                for (int trackIndex = 0; trackIndex < this->getNumTracks(); trackIndex++)
                {
                    unsigned int color;
                    unsigned int slotColor = ImGui::ColorConvertFloat4ToU32(this->m_colors.m_trackColor);
                    unsigned int boundColor = ImGui::ColorConvertFloat4ToU32(this->m_colors.m_trackBoundingBox);
                    unsigned int textColor = ImGui::ColorConvertFloat4ToU32(this->m_colors.m_trackTextColor);

                    if (!focused)
                        slotColor = ImGui::ColorConvertFloat4ToU32(this->m_colors.m_trackColorInactive);

                    Track* track = this->m_tracks[trackIndex];

                    bool isDiscrete = track->discrete;

                    for (int eventIdx = 0; eventIdx < track->events.size(); eventIdx++)
                    {
                        std::string eventValueStr = this->getEventLabel(trackIndex, eventIdx, true);

                        if ((this->m_editorOptions & kEditorMarkActiveEvents) && this->m_currentFrame > -1)
                        {
                            if (track->events[eventIdx]->isEventActive(this->m_currentFrame))
                            {
                                slotColor = ImGui::ColorConvertFloat4ToU32(this->m_colors.m_trackColorActive);
                                boundColor = ImGui::ColorConvertFloat4ToU32(this->m_colors.m_trackBoundingBoxActive);
                            }
                            else
                            {
                                slotColor = ImGui::ColorConvertFloat4ToU32(this->m_colors.m_trackColor);
                                boundColor = ImGui::ColorConvertFloat4ToU32(this->m_colors.m_trackBoundingBox);
                            }
                        }

                        if (this->m_editorOptions & kEditorHighlightSelectedEvent)
                        {
                            if ((this->m_selectedTrack == trackIndex) && (this->m_selectedEvent == eventIdx))
                                boundColor = ImGui::ColorConvertFloat4ToU32(this->m_colors.m_highlight);
                        }

                        ImVec2 startPos = ImVec2(editorCanvasStart - firstFrameUsed * framePixelWidth, contentMin.y + ItemHeight * trackIndex + 1 + customHeight);
                        
                        int movingPart = 0;

                        if (isDiscrete)
                            movingPart = drawDiscreteEvent(draw_list, startPos, ItemHeight, framePixelWidth, this->m_tracks[trackIndex]->events[eventIdx]->frameStart, this->getEventLabel(trackIndex, eventIdx, true), eventIdx, this->m_eventLabelAlignment, slotColor, boundColor, textColor, focused && hovered);
                        else
                            movingPart = drawDurationEvent(draw_list, startPos, ItemHeight, framePixelWidth, this->m_tracks[trackIndex]->events[eventIdx]->frameStart, this->m_tracks[trackIndex]->events[eventIdx]->frameEnd, this->getEventLabel(trackIndex, eventIdx, true), this->m_eventLabelAlignment, slotColor, boundColor, textColor, focused && hovered);

                        if (!this->m_popupOpened && !this->m_movingScrollBar && !this->m_movingCurrentFrame)
                        {
                            if (movingPart)
                            {
                                this->m_movingTrack = trackIndex;
                                this->m_movingEvent = eventIdx;

                                if ((this->m_editorOptions & kEditorChangeEventStartEnd))
                                {
                                    this->m_movingPos = mouseX;
                                    this->m_movingPart = movingPart;
                                }
                            }
                        }
                        else if (ImGui::GetMouseCursor() != ImGuiMouseCursor_Arrow)
                            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
                    }
                }

                // moving
                if (!this->m_popupOpened)
                {
                    if (this->m_movingTrack >= 0 && this->m_movingEvent >= 0)
                    {
                        this->m_selectedTrack = this->m_movingTrack;
                        this->m_selectedEvent = this->m_movingEvent;

                        ImGui::SetNextFrameWantCaptureMouse(true);
                        int diffFrame = int((mouseX - this->m_movingPos) / framePixelWidth);

                        if (diffFrame != 0)
                            this->m_edited = true;

                        if (std::abs(diffFrame) > 0)
                        {
                            Track* track = this->m_tracks[this->m_movingTrack];

                            if (this->m_movingPart == kMovingPartEnd)
                            {
                                if (track->events[this->m_movingEvent]->frameEnd + diffFrame >= 0)
                                    track->events[this->m_movingEvent]->frameEnd += diffFrame;

                                this->m_movingPos += int(diffFrame * framePixelWidth);
                            }
                            else if ((track->events[this->m_movingEvent]->frameStart + diffFrame >= -this->m_frameMax) && (track->events[this->m_movingEvent]->frameEnd + diffFrame <= 2 * this->m_frameMax))
                            {
                                if (this->m_movingPart == kMovingPartStart)
                                {
                                    if (track->events[this->m_movingEvent]->frameEnd - diffFrame >= 0)
                                        track->events[this->m_movingEvent]->frameStart += diffFrame;
                                }
                                else if (this->m_movingPart == kMovingPartAll)
                                {
                                    track->events[this->m_movingEvent]->frameStart += diffFrame;
                                    track->events[this->m_movingEvent]->frameEnd += diffFrame;
                                }

                                this->m_movingPos += int(diffFrame * framePixelWidth);
                            }
                            else
                            {
                                if (track->events[this->m_movingEvent]->frameStart < -this->m_frameMax)
                                    track->events[this->m_movingEvent]->frameStart = -this->m_frameMax;

                                if (track->events[this->m_movingEvent]->frameEnd > 2 * this->m_frameMax)
                                    track->events[this->m_movingEvent]->frameStart = (2 * this->m_frameMax - (track->events[this->m_movingEvent]->frameEnd - track->events[this->m_movingEvent]->frameStart));
                            }

                        }
                        if (!io.MouseDown[0] && !this->m_popupOpened)
                        {
                            // single select
                            if (!diffFrame && this->m_movingPart && this->m_selectedTrack && this->m_selectedEvent)
                            {
                                this->m_selectedTrack = this->m_movingTrack;
                                this->m_selectedEvent = this->m_movingEvent;
                                ret = true;
                            }

                            this->m_movingTrack = -1;
                            this->m_movingEvent = -1;
                        }
                    }
                }

                const float cursorWidth = 1.f;

                float firstFramePos = editorCanvasStart + this->m_firstFrame * framePixelWidth - cursorWidth * 0.5f;
                float lastFramePos = editorCanvasStart + this->m_lastFrame * framePixelWidth - cursorWidth * 0.5f;
                
                draw_list->AddLine(ImVec2(firstFramePos, canvas_pos.y), ImVec2(firstFramePos, contentMax.y), IM_COL32(255, 0, 0, 255), 1.f);
                draw_list->AddLine(ImVec2(lastFramePos, canvas_pos.y), ImVec2(lastFramePos, contentMax.y), IM_COL32(255, 0, 0, 255), 1.f);

                // cursor
                if ((this->m_currentFrame >= this->m_firstFrame) && (this->m_currentFrame <= this->m_lastFrame))
                {
                    float cursorOffset = editorCanvasStart + (this->m_currentFrame - firstFrameUsed) * framePixelWidth - cursorWidth * 0.5f;
                    draw_list->AddLine(ImVec2(cursorOffset, canvas_pos.y), ImVec2(cursorOffset, contentMax.y), ImGui::ColorConvertFloat4ToU32(this->m_colors.m_cursorColor), cursorWidth);

                    char tmps[512];

                    if (this->m_currentFrame != this->m_frameMin && this->m_currentFrame != this->m_frameMax)
                    {
                        draw_list->PopClipRect();
                        draw_list->PopClipRect();

                        switch (this->m_timeCodeFormat)
                        {
                        case TrackEditor::kSeconds:
                            ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%.3f", this->getCurrentTime());
                            break;
                        case TrackEditor::kMilliseconds:
                            ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%.3f", this->getCurrentTime() * 1000.f);
                            break;
                        case TrackEditor::kFrames:
                            ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%d", this->m_currentFrame);
                            break;
                        default:
                            break;
                        }

                        draw_list->AddText(ImVec2(cursorOffset, canvas_pos.y), 0xFFFFFFFF, tmps);

                        draw_list->PushClipRect(childFramePos, childFramePos + childFrameSize, true);
                        draw_list->PushClipRect(childFramePos + ImVec2(float(this->m_legendWidth), 0.f), childFramePos + childFrameSize, true);
                    }
                }

                draw_list->PopClipRect();
                draw_list->PopClipRect();

                // copy paste
                if (this->m_editorOptions & kEditorCopyPaste)
                {
                    ImRect rectCopy(ImVec2(contentMin.x + 100, canvas_pos.y + 2)
                        , ImVec2(contentMin.x + 100 + 30, canvas_pos.y + ItemHeight - 2));
                    bool inRectCopy = rectCopy.Contains(io.MousePos);
                    unsigned int copyColor = inRectCopy ? 0xFF1080FF : 0xFF000000;
                    draw_list->AddText(rectCopy.Min, copyColor, "Copy");

                    ImRect rectPaste(ImVec2(contentMin.x + 140, canvas_pos.y + 2)
                        , ImVec2(contentMin.x + 140 + 30, canvas_pos.y + ItemHeight - 2));
                    bool inRectPaste = rectPaste.Contains(io.MousePos);
                    unsigned int pasteColor = inRectPaste ? 0xFF1080FF : 0xFF000000;
                    draw_list->AddText(rectPaste.Min, pasteColor, "Paste");

                    if (inRectCopy && io.MouseReleased[0] && !this->m_popupOpened)
                    {
                        //this->Copy();
                    }
                    if (inRectPaste && io.MouseReleased[0] && !this->m_popupOpened)
                    {
                        //this->Paste();
                    }
                }
            }

            if (this->m_source != nullptr)
            {
                //Add & Delete components
                if (this->m_editorOptions & kEditorAddTrack)
                {
                    addTrack = editorAddTrackButton(draw_list, ImVec2(canvas_pos.x + this->m_legendWidth - 8, canvas_pos.y + 2), ImVec2(4, ItemHeight * 0.8f)) && focused && !this->m_popupOpened && !this->m_movingCurrentFrame && !this->m_movingScrollBar && this->m_movingTrack == -1 && !legendResizeRect.Contains(io.MousePos);

                    if (addTrack && !reload)
                    {
                        if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                        if (io.MouseReleased[0])
                        {
                            this->m_popupOpened = true;
                            ImGui::OpenPopup("addTrack");
                        }
                    }

                    addTrack = this->addTrackGUI();

                    if (removeTrack && !reload)
                    {
                        this->m_popupOpened = true;
                        ImGui::OpenPopup("deleteTrack");
                    }

                    removeTrack = this->deleteTrackGUI();
                }

                if (this->m_editorOptions & kEditorRenameTrack)
                {
                    if (renameTrack && !reload)
                    {
                        this->m_popupOpened = true;
                        ImGui::OpenPopup("renameTrack");
                    }

                    renameTrack = this->renameTrackGUI();
                }

                if (this->m_editorOptions & kEditorAddEvent)
                {
                    if (delEvent && !reload)
                    {
                        this->m_popupOpened = true;
                        ImGui::OpenPopup("deleteEvent");
                    }

                    delEvent = this->deleteEventGUI();

                    if (addEvent && !reload)
                    {
                        this->m_popupOpened = true;
                        ImGui::OpenPopup("addEvent");

                        this->eventBuffer->frameStart = this->m_currentFrame;
                    }

                    addEvent = this->addEventGUI();
                }
            }

            ImGui::EndChild();

            ImGui::PopStyleColor();
            if (hasScrollBar)
            {
                ImGui::InvisibleButton("scrollBar", scrollBarSize);
                ImVec2 scrollBarMin = ImGui::GetItemRectMin();
                ImVec2 scrollBarMax = ImGui::GetItemRectMax();

                // ratio = number of frames visible in control / number to total frames

                float startFrameOffset = ((float)(firstFrameUsed - this->m_frameMin) / (float)frameCount) * editorCanvasSize;
                ImVec2 scrollBarA(scrollBarMin.x + this->m_legendWidth, scrollBarMin.y - 2);
                ImVec2 scrollBarB(scrollBarMin.x + availableSpace.x, scrollBarMax.y - 1);
                draw_list->AddRectFilled(scrollBarA, scrollBarB, 0xFF222222, 0);

                ImRect scrollBarRect(scrollBarA, scrollBarB);
                bool inScrollBar = scrollBarRect.Contains(io.MousePos);

                draw_list->AddRectFilled(scrollBarA, scrollBarB, 0xFF101010, 8);

                ImVec2 scrollBarC(scrollBarMin.x + this->m_legendWidth + startFrameOffset, scrollBarMin.y);
                ImVec2 scrollBarD(scrollBarMin.x + this->m_legendWidth + barWidthInPixels + startFrameOffset, scrollBarMax.y - 2);
                draw_list->AddRectFilled(scrollBarC, scrollBarD, (inScrollBar || this->m_movingScrollBar) ? 0xFF606060 : 0xFF505050, 6);

                ImRect scrollBarThumb(scrollBarC, scrollBarD);
                const float MinBarWidth = 44.f;

                if (this->m_movingScrollBar)
                {
                    if (!io.MouseDown[0])
                    {
                        this->m_movingScrollBar = false;
                    }
                    else
                    {
                        float framesPerPixelInBar = barWidthInPixels / (float)visibleFrameCount;
                        this->m_firstFrame = int((io.MousePos.x - this->m_panningViewSource.x) / framesPerPixelInBar) - this->m_panningViewFrame;
                        this->m_firstFrame = ImClamp(this->m_firstFrame, this->m_frameMin, ImMax(this->m_frameMax - visibleFrameCount + 10, this->m_frameMin));
                    }
                }
                else if (focused)
                {
                    if (scrollBarThumb.Contains(io.MousePos) && ImGui::IsMouseClicked(0) && !this->m_movingCurrentFrame && this->m_movingTrack == -1 && !this->m_popupOpened && !this->m_resizeLegend)
                    {
                        this->m_movingScrollBar = true;
                        this->m_panningViewSource = io.MousePos;
                        this->m_panningViewFrame = -this->m_firstFrame;
                    }
                }
            }

            if (!this->m_popupOpened)
            {
                if (regionRect.Contains(io.MousePos))
                {
                    bool overCustomDraw = false;
                    for (auto& custom : customDraws)
                    {
                        overCustomDraw = true;
                        if (custom.customRect.Contains(io.MousePos))
                        {
                            overCustomDraw = true;
                        }
                    }

                    int frameOverCursor = (int)((io.MousePos.x - topRect.Min.x) / framePixelWidth) + firstFrameUsed;

                    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
                    {
                        if (io.MouseWheel < -FLT_EPSILON)
                        {
                            framePixelWidthTarget -= 0.3f;
                            framePixelWidth -= 0.3f;
                            int newFrameOverCursor = (int)((io.MousePos.x - topRect.Min.x) / framePixelWidth) + firstFrameUsed;
                            this->m_firstFrame += frameOverCursor - newFrameOverCursor;
                        }

                        if (io.MouseWheel > FLT_EPSILON)
                        {
                            framePixelWidthTarget += 0.3f;
                            framePixelWidth += 0.3f;
                            int newFrameOverCursor = (int)((io.MousePos.x - topRect.Min.x) / framePixelWidth) + firstFrameUsed;
                            this->m_firstFrame += frameOverCursor - newFrameOverCursor;
                        }
                    }
                }
            }
        }

        ImGui::EndGroup();

        if (this->m_expanded)
        {
            if (this->m_editorOptions & kEditorCollapse)
            {
                bool overExpanded = editorAddDeleteButton(draw_list, ImVec2(canvas_pos.x + 2, canvas_pos.y + 2), ImVec2(4, ItemHeight), !this->m_expanded);
                if (overExpanded && io.MouseReleased[0] && !this->m_popupOpened)
                    this->m_expanded = !this->m_expanded;
            }
        }

        if (!ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel))
            this->m_popupOpened = false;

        if (framePixelWidth > 1.f && framePixelWidth < 50.f)
            this->m_zoomLevel = framePixelWidth;

        return ret;
    }

    void TrackEditorBase::registerListener(TrackEditorBase* listener)
    {
        this->m_listeners.push_back(listener);
    }

    void TrackEditorBase::notifyListeners()
    {
        for (size_t i = 0; i < this->m_listeners.size(); i++)
            this->m_listeners[i]->setCurrentTime(this->getCurrentTime());
    }

    void TrackEditorBase::clearTracks()
    {
        for (size_t i = 0; i < this->m_tracks.size(); i++)
            this->m_tracks[i]->destroy();

        this->m_tracks.clear();
    }
}