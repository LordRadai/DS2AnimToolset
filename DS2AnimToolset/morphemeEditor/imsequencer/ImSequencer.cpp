// https://github.com/CedricGuillemet/ImGuizmo
// v 1.83
//
// The MIT License(MIT)
//
// Copyright(c) 2021 Cedric Guillemet
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include "ImSequencer.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include <cstdlib>
#include "../Application/Application.h"

namespace ImSequencer
{
#ifndef IMGUI_DEFINE_MATH_OPERATORS
    static ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
        return ImVec2(a.x + b.x, a.y + b.y);
    }
#endif

    static bool SequencerAddDelButton(ImDrawList* draw_list, ImVec2 pos, ImVec2 size, bool add = true)
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

    static bool SequencerAddRemoveButton(ImDrawList* draw_list, ImVec2 pos, ImVec2 size)
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

    static bool SequencerAddTrackButton(ImDrawList* draw_list, ImVec2 pos, ImVec2 size)
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

    static bool SequencerAddTrackLabel(ImDrawList* draw_list, ImVec2 pos, ImVec2 size, char* label)
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

    bool Sequencer(EventTrackEditor* eventTrackEditor, int* currentFrame, int* selectedTrack, int* selectedEvent, bool* expanded, bool focused, int* firstFrame, float* zoomLevel, int sequenceOptions)
    {
        static bool hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

        bool ret = false;
        ImGuiIO& io = ImGui::GetIO();
        int cx = (int)(io.MousePos.x);
        int cy = (int)(io.MousePos.y);
        float framePixelWidth = *zoomLevel;
        float framePixelWidthTarget = *zoomLevel;

        static bool resizeLegend = false;
        static int legendWidth = 210;

        static int movingTrack = -1;
        static int movingEvent = -1;
        static int movingPos = -1;

        enum MovingPart
        {
            MovingPart_None,
            MovingPart_Start,
            MovingPart_End,
            MovingPart_All
        };

        static int movingPart;

        bool reload = false;
        bool removeTrack = false;
        bool renameTrack = false;
        static char trackRename[50] = "";

        bool addTrack = false;
        static int addTrackEventId = 0;
        static char addTrackName[50] = "MyTrack";
        static bool addTrackIsDuration = false;

        static float addEventStart = 0.f;
        static float addEventDur = 0.f;
        static int addEventUserData = 0;

        bool delEvent = false;

        int ItemHeight = 23;

        static bool popupOpened = false;

        ImGui::BeginGroup();

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
         
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
        ImVec2 canvas_size = ImGui::GetContentRegionAvail();        // Resize canvas to what's available

        int firstFrameUsed = firstFrame ? *firstFrame : 0;

        int controlHeight = ImMax(eventTrackEditor->GetTrackCount() * ItemHeight, ItemHeight);

        int frameCount = ImMax(eventTrackEditor->GetFrameMax() - eventTrackEditor->GetFrameMin() + 10, 1);

        canvas_size.y = controlHeight + 2.5 * ItemHeight;

        static bool MovingScrollBar = false;
        static bool MovingCurrentFrame = false;
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
        const int visibleFrameCount = (int)floorf((canvas_size.x - legendWidth) / framePixelWidth);
        const float barWidthRatio = ImMin(visibleFrameCount / (float)frameCount, 1.f);
        const float barWidthInPixels = barWidthRatio * (canvas_size.x - legendWidth);

        ImRect regionRect(canvas_pos, canvas_pos + canvas_size);

        static bool panningView = false;
        static ImVec2 panningViewSource;
        static int panningViewFrame;
        if (ImGui::IsWindowFocused() && io.KeyAlt && io.MouseDown[2])
        {
            if (!panningView)
            {
                panningViewSource = io.MousePos;
                panningView = true;
                panningViewFrame = *firstFrame;
            }
            *firstFrame = panningViewFrame - int((io.MousePos.x - panningViewSource.x) / framePixelWidth);
            *firstFrame = ImClamp(*firstFrame, eventTrackEditor->GetFrameMin(), eventTrackEditor->GetFrameMax() - visibleFrameCount);
        }
        if (panningView && !io.MouseDown[2])
        {
            panningView = false;
        }
        framePixelWidthTarget = ImClamp(framePixelWidthTarget, 0.1f, 50.f);

        framePixelWidth = ImLerp(framePixelWidth, framePixelWidthTarget, 0.33f);

        frameCount = eventTrackEditor->GetFrameMax() - eventTrackEditor->GetFrameMin() + 10;
        if (visibleFrameCount >= frameCount && firstFrame)
            *firstFrame = eventTrackEditor->GetFrameMin();

        // --
        if (expanded && !*expanded)
        {
            ImGui::InvisibleButton("canvas", ImVec2(canvas_size.x - canvas_pos.x, (float)ItemHeight));
            draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_size.x + canvas_pos.x, canvas_pos.y + ItemHeight), 0xFF3D3837, 0);
            char tmps[512];
            ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%d Frames / %d tracks", frameCount, eventTrackEditor->GetTrackCount());
            draw_list->AddText(ImVec2(canvas_pos.x + 26, canvas_pos.y + 2), 0xFFFFFFFF, tmps);
        }
        else
        {
            bool hasScrollBar(true);

            // test scroll area
            ImVec2 headerSize(canvas_size.x, (float)ItemHeight);
            ImVec2 scrollBarSize(canvas_size.x, 14.f);
            ImGui::InvisibleButton("topBar", headerSize);
            draw_list->AddRectFilled(canvas_pos, canvas_pos + headerSize, 0xFFFF0000, 0);
            ImVec2 childFramePos = ImGui::GetCursorScreenPos();
            ImVec2 childFrameSize(canvas_size.x, canvas_size.y - 8.f - headerSize.y - (hasScrollBar ? scrollBarSize.y : 0));

            ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);

            ImGui::BeginChildFrame(889, childFrameSize);
            eventTrackEditor->focused = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
            ImGui::InvisibleButton("contentBar", ImVec2(canvas_size.x, float(controlHeight)));
            const ImVec2 contentMin = ImGui::GetItemRectMin();
            const ImVec2 contentMax = ImGui::GetItemRectMax();
            const ImRect contentRect(contentMin, contentMax);
            const float contentHeight = contentMax.y - contentMin.y;

            // full background
            draw_list->AddRectFilled(canvas_pos, canvas_pos + canvas_size, 0xFF242424, 0);

            // current frame top
            ImRect topRect(ImVec2(canvas_pos.x + legendWidth, canvas_pos.y), ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + ItemHeight));
            ImRect legendResizeRect(ImVec2(canvas_pos.x + legendWidth - 2, canvas_pos.y), ImVec2(canvas_pos.x + legendWidth + 2, canvas_pos.y + ItemHeight));

            if (hovered && focused && (!popupOpened && !MovingCurrentFrame && !MovingScrollBar && movingTrack == -1 && legendResizeRect.Contains(io.MousePos)) || resizeLegend)
            {
                if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

                if (io.MouseDown[0])
                    resizeLegend = true;
            }

            if (resizeLegend)
            {
                legendWidth = (int)((io.MousePos.x - contentMin.x));
            }

            if (resizeLegend && io.MouseReleased[0])
                resizeLegend = false;

            //header
            draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_size.x + canvas_pos.x, canvas_pos.y + ItemHeight), 0xFF404040, 0);
            if (sequenceOptions & EDITOR_TRACK_ADD && (eventTrackEditor->m_nodeSource != nullptr))
            {
                if (SequencerAddTrackButton(draw_list, ImVec2(canvas_pos.x + legendWidth - 8, canvas_pos.y + 2), ImVec2(4, ItemHeight * 0.8f)) && focused && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) && !popupOpened && !MovingCurrentFrame && !MovingScrollBar && movingTrack == -1 && !legendResizeRect.Contains(io.MousePos))
                {
                    if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                    if (io.MouseReleased[0])
                    {
                        popupOpened = true;
                        ImGui::OpenPopup("addTrack");
                    }
                }

                ImGui::PopStyleColor();
                if (ImGui::BeginPopup("addTrack") && popupOpened)
                {
                    ImGui::Text("Add Track");
                    ImGui::Separator();

                    ImGui::InputText("Name", addTrackName, 50);
                    ImGui::InputInt("Event ID", &addTrackEventId);

                    const char* types[2] = {"Discrete", "Duration"};

                    if (ImGui::BeginCombo("Type", types[addTrackIsDuration]))
                    {
                        for (size_t i = 0; i < 2; i++)
                        { 
                            const bool selected = (addTrackIsDuration == i);
                            if (ImGui::Selectable(types[i], addTrackIsDuration))
                                addTrackIsDuration = i;
                        }

                        ImGui::EndCombo();
                    }

                    if (ImGui::Button("Add Track") || GetAsyncKeyState(VK_RETURN) & 1)
                    {
                        eventTrackEditor->AddTrack(addTrackEventId, addTrackName, addTrackIsDuration);
                        ImGui::CloseCurrentPopup();

                        *selectedTrack = -1;
                        *selectedEvent = -1;

                        reload = true;
                    }

                    ImGui::EndPopup();
                }

                ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);
            }

            if (sequenceOptions & EDITOR_CHANGE_FRAME && currentFrame && ((topRect.Contains(io.MousePos) && !popupOpened && !MovingScrollBar && movingTrack == -1 && !legendResizeRect.Contains(io.MousePos) && !resizeLegend) || MovingCurrentFrame))
            {
                if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                if (!resizeLegend && !popupOpened && !MovingScrollBar && movingTrack == -1 && io.MouseDown[0])
                    MovingCurrentFrame = true;
            }

            if (MovingCurrentFrame)
            {
                if (frameCount)
                {
                    *currentFrame = (int)((io.MousePos.x - topRect.Min.x) / framePixelWidth) + firstFrameUsed;
                    if (*currentFrame < eventTrackEditor->GetFrameMin())
                        *currentFrame = eventTrackEditor->GetFrameMin();
                    if (*currentFrame >= eventTrackEditor->GetFrameMax())
                        *currentFrame = eventTrackEditor->GetFrameMax();
                }
                if (!io.MouseDown[0])
                    MovingCurrentFrame = false;
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
                bool baseIndex = ((i % modFrameCount) == 0) || (i == eventTrackEditor->GetFrameMax() || i == eventTrackEditor->GetFrameMin());
                bool halfIndex = (i % halfModFrameCount) == 0;
                ImVec2 pos = ImVec2(contentMin.x + legendWidth - firstFrameUsed * framePixelWidth, contentMin.y + ItemHeight * i + 1);
                float px = pos.x + i * framePixelWidth - 1;
                //int px = (int)canvas_pos.x + int(i * framePixelWidth) + legendWidth + int(firstFrameUsed * framePixelWidth) + 3;
                int tiretStart = baseIndex ? 4 : (halfIndex ? 10 : 14);
                int tiretEnd = baseIndex ? regionHeight : ItemHeight;              

                if (px <= (canvas_size.x + canvas_pos.x) && px >= (canvas_pos.x + legendWidth))
                {
                    draw_list->AddLine(ImVec2((float)px, canvas_pos.y + (float)tiretStart), ImVec2((float)px, canvas_pos.y + (float)tiretEnd - 1), 0xFF606060, 1);

                    draw_list->AddLine(ImVec2((float)px, canvas_pos.y + (float)ItemHeight), ImVec2((float)px, canvas_pos.y + (float)regionHeight - 1), 0x30606060, 1);
                }

                if (baseIndex && px > (canvas_pos.x + legendWidth))
                {
                    char tmps[512];
                    ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%.3f", RMath::FrameToTime(i));
                    draw_list->AddText(ImVec2((float)(px + 6), canvas_pos.y), 0xFFFFFFFF, tmps);
                }
            };

            auto drawLineContent = [&](int i, int /*regionHeight*/) {
                //int px = (int)canvas_pos.x + int(i * framePixelWidth) + legendWidth + int(firstFrameUsed * framePixelWidth) + 3;
                ImVec2 pos = ImVec2(contentMin.x + legendWidth - firstFrameUsed * framePixelWidth, contentMin.y + ItemHeight * i + 1);
                float px = pos.x + i * framePixelWidth - 1;
                int tiretStart = int(contentMin.y);
                int tiretEnd = int(contentMax.y);

                if (px <= (canvas_size.x + canvas_pos.x) && px >= (canvas_pos.x + legendWidth))
                {
                    //draw_list->AddLine(ImVec2((float)px, canvas_pos.y + (float)tiretStart), ImVec2((float)px, canvas_pos.y + (float)tiretEnd - 1), 0xFF606060, 1);

                    draw_list->AddLine(ImVec2(float(px), float(tiretStart)), ImVec2(float(px), float(tiretEnd)), 0x30606060, 1);
                }
            };

            for (int i = eventTrackEditor->GetFrameMin() + 1; i <= eventTrackEditor->GetFrameMax(); i += frameStep)
            {
                drawLine(i, ItemHeight);
            }

            drawLine(eventTrackEditor->GetFrameMin(), ItemHeight);
            drawLine(eventTrackEditor->GetFrameMax(), ItemHeight);

            for (int i = eventTrackEditor->GetFrameMax() + 1; i <= 500; i += frameStep)
            {
                drawLine(i, ItemHeight);
            }

            // clip content
            draw_list->PushClipRect(childFramePos, childFramePos + childFrameSize, true);

            // draw item names in the legend rect on the left
            size_t customHeight = 0;
            for (int i = 0; i < eventTrackEditor->GetTrackCount(); i++)
            {
                ImVec2 tpos(contentMin.x + 3, contentMin.y + i * ItemHeight + 2 + customHeight);

                ImVec2 labelEndPos = ImVec2(legendWidth - 15, ItemHeight * 0.9f);

                if (sequenceOptions & EDITOR_TRACK_ADD)
                    labelEndPos.x -= ItemHeight;

                if (sequenceOptions & EDITOR_EVENT_ADD)
                    labelEndPos.x -= ItemHeight;

                bool overLabel = SequencerAddTrackLabel(draw_list, ImVec2(tpos.x, tpos.y), labelEndPos, (char*)eventTrackEditor->GetTrackName(i).c_str());

                if (sequenceOptions & EDITOR_TRACK_ADD)
                {                    
                    if (SequencerAddRemoveButton(draw_list, ImVec2(contentMin.x + legendWidth - ItemHeight - ItemHeight + 2 - 10, tpos.y + 2), ImVec2(ItemHeight * 0.8f, ItemHeight * 0.8f)) && focused && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) && !popupOpened && !MovingCurrentFrame && !MovingScrollBar && movingTrack == -1)
                    {
                        if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                        if (io.MouseReleased[0])
                        {
                            *selectedTrack = i;
                            *selectedEvent = -1;
                            removeTrack = true;
                        }
                    }                
                }

                if (sequenceOptions & EDITOR_TRACK_RENAME)
                {
                    if (overLabel && focused && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) && !popupOpened && !MovingCurrentFrame && !MovingScrollBar && movingTrack == -1)
                    {
                        if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                        if (io.MouseReleased[0])
                        {
                            *selectedTrack = i;
                            *selectedEvent = -1;
                            renameTrack = true;
                        }
                    }
                }

                if (sequenceOptions & EDITOR_EVENT_ADD)
                {
                    bool addButton = SequencerAddDelButton(draw_list, ImVec2(contentMin.x + legendWidth - ItemHeight + 2 - 10, tpos.y + 2), ImVec2(ItemHeight * 0.8f, ItemHeight * 0.8f), true);
                    
                    if (addButton && focused && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) && !popupOpened && !MovingCurrentFrame && !MovingScrollBar && movingTrack == -1)
                    {
                        if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                        if (io.MouseReleased[0])
                        {
                            addTrack = true;
                            *selectedTrack = i;
                        }
                    }

                    if (!popupOpened && !MovingCurrentFrame && !MovingScrollBar && movingTrack == -1)
                    {
                        for (size_t j = 0; j < eventTrackEditor->m_eventTracks[i].m_numEvents; j++)
                        {
                            if (*selectedTrack == i && *selectedEvent == j && focused && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows))
                            {
                                if (GetAsyncKeyState(VK_DELETE) & 1)
                                    delEvent = true;
                            }
                        }
                    }
                }
            }

            ImGui::PopStyleColor();
            if (removeTrack && !reload)
            {
                popupOpened = true;
                ImGui::OpenPopup("deleteTrack");
            }

            if (ImGui::BeginPopup("deleteTrack") && popupOpened)
            {
                removeTrack = false;

                std::string header = std::string(eventTrackEditor->m_eventTracks[*selectedTrack].m_name);

                ImGui::Text(header.c_str());
                ImGui::Separator();

                if (ImGui::Button("Delete Track") || GetAsyncKeyState(VK_RETURN) & 1)
                {
                    reload = true;

                    eventTrackEditor->DeleteTrack(*selectedTrack);
                    *selectedTrack = -1;
                    *selectedEvent = -1;

                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            if (renameTrack && !reload)
            {
                popupOpened = true;
                ImGui::OpenPopup("renameTrack");
            }

            if (ImGui::BeginPopup("renameTrack") && popupOpened)
            {
                renameTrack = false;

                std::string header = std::string(eventTrackEditor->m_eventTracks[*selectedTrack].m_name);

                ImGui::Text(header.c_str());
                ImGui::Separator();

                ImGui::InputText("Name", (char*)eventTrackEditor->m_eventTracks[*selectedTrack].m_name.c_str(), 50);

                if (GetAsyncKeyState(VK_RETURN))
                    ImGui::CloseCurrentPopup();

                ImGui::EndPopup();
            }

            if (delEvent && !reload)
            {
                popupOpened = true;
                ImGui::OpenPopup("deleteEvent");
            }

            if (ImGui::BeginPopup("deleteEvent") && popupOpened)
            {
                delEvent = false;

                std::string header = std::string(eventTrackEditor->m_eventTracks[*selectedTrack].m_name) + " [" + std::to_string(*selectedEvent) + "]";

                ImGui::Text(header.c_str());
                ImGui::Separator();

                if (ImGui::Button("Delete Event") || GetAsyncKeyState(VK_RETURN) & 1)
                {
                    eventTrackEditor->DeleteEvent(*selectedTrack, *selectedEvent);

                    *selectedTrack = -1;
                    *selectedEvent = -1;

                    ImGui::CloseCurrentPopup();

                    reload = true;
                }

                ImGui::EndPopup();
            }

            if (addTrack && !reload)
            {
                popupOpened = true;
                ImGui::OpenPopup("addEvent");
            }

            if (ImGui::BeginPopup("addEvent") && popupOpened)
            {
                addTrack = false;

                ImGui::Text((char*)eventTrackEditor->m_eventTracks[*selectedTrack].m_name.c_str());
                ImGui::Separator();

                ImGui::InputFloat("Start", &addEventDur, 1.f / 60.f);

                if (addEventDur < 0.f)
                    addEventDur = 0.f;

                if (eventTrackEditor->m_eventTracks[*selectedTrack].m_discrete == false)
                    ImGui::InputFloat("Duration", &addEventDur, 1.f / 60.f);
                else
                    addEventDur = 0.f;

                *currentFrame = RMath::TimeToFrame(addEventStart);

                ImGui::InputInt("Value", &addEventUserData);

                if (ImGui::Button("Add Event") || GetAsyncKeyState(VK_RETURN) & 1)
                {
                    eventTrackEditor->AddEvent(*selectedTrack, EventTrackEditor::EventTrack::Event{ RMath::TimeToFrame(addEventStart), RMath::TimeToFrame(addEventDur), addEventUserData});
                    ImGui::CloseCurrentPopup();

                    reload = true;
                }

                ImGui::EndPopup();
            }

            ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);

            if (!reload)
            {
                if (*currentFrame > 0 && !popupOpened)
                    addEventStart = RMath::FrameToTime(*currentFrame);

                // clipping rect so items bars are not visible in the legend on the left when scrolled

                // slots background
                for (int i = 0; i < eventTrackEditor->GetTrackCount(); i++)
                {
                    unsigned int col = 0xFF313131;

                    ImVec2 pos = ImVec2(contentMin.x + legendWidth, contentMin.y + ItemHeight * i + 1 + customHeight);
                    ImVec2 sz = ImVec2(canvas_size.x + canvas_pos.x, pos.y + ItemHeight - 1);

                    draw_list->AddRectFilled(pos, sz, col, 0);

                    if (!popupOpened && cy >= pos.y && cy < pos.y + ItemHeight && movingTrack == -1 && cx>contentMin.x && cx < contentMin.x + canvas_size.x)
                    {
                        draw_list->AddRectFilled(pos, sz, 0x10FFFFFF, 0);
                    }
                }

                draw_list->PushClipRect(childFramePos + ImVec2(float(legendWidth), 0.f), childFramePos + childFrameSize, true);

                // vertical frame lines in content area
                for (int i = eventTrackEditor->GetFrameMin() + 1; i <= eventTrackEditor->GetFrameMax(); i += frameStep)
                {
                    drawLineContent(i, int(contentHeight));
                }
                drawLineContent(eventTrackEditor->GetFrameMin(), int(contentHeight));
                drawLineContent(eventTrackEditor->GetFrameMax(), int(contentHeight));

                for (int i = eventTrackEditor->GetFrameMax() + 1; i <= 500; i += frameStep)
                {
                    drawLineContent(i, ItemHeight);
                }

                draw_list->AddRectFilled(ImVec2(contentMin.x + legendWidth - firstFrameUsed * framePixelWidth + eventTrackEditor->GetFrameMax() * framePixelWidth, canvas_pos.y), canvas_pos + canvas_size, 0x40000000, 0);

                // tracks
                customHeight = 0;
                for (int trackIndex = 0; trackIndex < eventTrackEditor->GetTrackCount(); trackIndex++)
                {
                    unsigned int color;
                    unsigned int slotColor = ImGui::ColorConvertFloat4ToU32(eventTrackEditor->m_colors.m_trackColor);
                    unsigned int slotColorHalf = slotColor | 0x40000000;
                    unsigned int boundColor = ImGui::ColorConvertFloat4ToU32(eventTrackEditor->m_colors.m_trackBoundingBox);

                    if (focused == false)
                        slotColor = ImGui::ColorConvertFloat4ToU32(eventTrackEditor->m_colors.m_trackColorInactive);

                    EventTrackEditor::EventTrack* track = &eventTrackEditor->m_eventTracks[trackIndex];

                    bool isDiscrete = track->m_discrete;

                    for (int eventIdx = 0; eventIdx < track->m_numEvents; eventIdx++)
                    {
                        std::string event_value = eventTrackEditor->GetEventLabel(trackIndex, eventIdx);

                        if ((sequenceOptions & EDITOR_MARK_ACTIVE_EVENTS) && *currentFrame > -1)
                        {
                            if (track->IsEventActive(eventIdx, *currentFrame))
                            {
                                slotColor = ImGui::ColorConvertFloat4ToU32(eventTrackEditor->m_colors.m_trackColorActive);
                                boundColor = ImGui::ColorConvertFloat4ToU32(eventTrackEditor->m_colors.m_trackBoundingBoxActive);
                            }
                            else
                            {
                                slotColor = ImGui::ColorConvertFloat4ToU32(eventTrackEditor->m_colors.m_trackColor);
                                boundColor = ImGui::ColorConvertFloat4ToU32(eventTrackEditor->m_colors.m_trackBoundingBox);
                            }
                        }

                        ImVec2 pos = ImVec2(contentMin.x + legendWidth - firstFrameUsed * framePixelWidth, contentMin.y + ItemHeight * trackIndex + 1 + customHeight);
                        ImVec2 slotP1(pos.x + track->m_event[eventIdx].m_frameStart * framePixelWidth - 1, pos.y + 2);
                        ImVec2 slotP2(pos.x + (track->m_event[eventIdx].m_duration + track->m_event[eventIdx].m_frameStart) * framePixelWidth - 1, pos.y + ItemHeight - 2);
                        ImVec2 slotP1Loop;
                        ImVec2 slotP2Loop;
                        ImVec2 slotD1Loop;
                        ImVec2 slotD2Loop;
                        ImVec2 slotT1Loop;
                        ImVec2 slotT2Loop;
                        ImVec2 slotT3Loop;

                        ImVec2 slotD1(pos.x + track->m_event[eventIdx].m_frameStart * framePixelWidth - 1, pos.y + 2);
                        ImVec2 slotD2(pos.x + track->m_event[eventIdx].m_frameStart * framePixelWidth - 1, pos.y + ItemHeight - 7);
                        ImVec2 slotT1 = ImVec2(pos.x + track->m_event[eventIdx].m_frameStart * framePixelWidth - 1, pos.y + ItemHeight - 7);
                        ImVec2 slotT2 = ImVec2(pos.x + track->m_event[eventIdx].m_frameStart * framePixelWidth - 1, pos.y + ItemHeight - 7);
                        ImVec2 slotT3 = ImVec2((slotT2.x + slotT1.x) / 2, (slotT2.y + slotT1.y) / 2 + 5);

                        ImVec2 frameMin = ImVec2(pos.x - 1, 0);
                        ImVec2 frameMax = ImVec2(frameMin.x + eventTrackEditor->GetFrameMax() * framePixelWidth, 0);

                        ImVec2 textSize = ImGui::CalcTextSize(event_value.c_str());
                        ImVec2 textSizeIdx = ImGui::CalcTextSize(std::to_string(eventIdx).c_str());
                        ImVec2 textP(slotP1.x + (slotP2.x - slotP1.x - textSize.x) / 2, slotP2.y + (slotP1.y - slotP2.y - textSize.y) / 2);
                        ImVec2 textD(slotD1.x + (slotD2.x - slotD1.x - textSize.x) / 2, slotD2.y + (slotD1.y - slotD2.y - textSize.y) / 2);
                        ImVec2 textDIdx(slotD1.x + 5.f + textSize.x / 3, slotT3.y - textSizeIdx.y);
                        ImVec2 textPLoop;
                        ImVec2 textDLoop;

                        if ((slotP1.x <= (canvas_size.x + contentMin.x) || slotP1.x >= (contentMin.x + legendWidth)) && (slotP2.x >= (contentMin.x + legendWidth) || slotP2.x <= (canvas_size.x + contentMin.x)))
                        {
                            if (isDiscrete)
                            {
                                slotD1.x -= 5.f;
                                slotD2.x += 5.f;

                                slotT1.x -= 5.f;
                                slotT2.x += 5.f;

                                draw_list->AddRectFilled(slotD1, slotD2, slotColor, 0); //Track Box
                                draw_list->AddLine(slotD1, ImVec2(slotD2.x, slotD1.y), boundColor);
                                draw_list->AddLine(slotD2, ImVec2(slotD2.x, slotD1.y), boundColor);
                                draw_list->AddLine(slotD1, ImVec2(slotD1.x, slotD2.y), boundColor);

                                draw_list->AddTriangleFilled(slotT1, slotT2, slotT3, slotColor);
                                draw_list->AddLine(slotT1, slotT3, boundColor);
                                draw_list->AddLine(slotT2, slotT3, boundColor);

                                ImRect track_box(slotD1, slotD2);

                                draw_list->AddText(textD, ImGui::ColorConvertFloat4ToU32(eventTrackEditor->m_colors.m_trackTextColor), event_value.c_str()); //Event Value
                                draw_list->AddText(textDIdx, ImGui::ColorConvertFloat4ToU32(eventTrackEditor->m_colors.m_trackTextColor), std::to_string(eventIdx).c_str()); //Event Idx
                            }
                            else
                            {
                                draw_list->AddRectFilled(slotP1, slotP2, slotColor, 0); //Track Box
                                draw_list->AddRect(slotP1, slotP2, boundColor, 0); //Track Bounding Box
                                draw_list->AddText(textP, ImGui::ColorConvertFloat4ToU32(eventTrackEditor->m_colors.m_trackTextColor), event_value.c_str()); //Event Value
                            }

                            if (sequenceOptions & EDITOR_EVENT_LOOP)
                            {
                                if (!isDiscrete)
                                {
                                    if (slotP2.x > frameMax.x)
                                    {
                                        if (slotP1.x < frameMax.x)
                                        {
                                            slotP1Loop = ImVec2(frameMin.x, slotP1.y);
                                            slotP2Loop = ImVec2(slotP1Loop.x + fmin(abs(slotP2.x - slotP1.x), abs(slotP2.x - frameMax.x)), slotP2.y);
                                        }
                                        else
                                        {
                                            slotP1Loop = ImVec2(frameMin.x + abs(slotP1.x - frameMax.x), slotP1.y);
                                            slotP2Loop = ImVec2(slotP1Loop.x + fmin(abs(slotP2.x - slotP1.x), abs(slotP2.x - frameMax.x)), slotP2.y);
                                        }

                                        textPLoop = ImVec2(slotP1Loop.x + (slotP2Loop.x - slotP1Loop.x - textSize.x) / 2, slotP2Loop.y + (slotP1Loop.y - slotP2Loop.y - textSize.y) / 2);

                                        draw_list->AddRectFilled(slotP1Loop, slotP2Loop, slotColor, 0); //Track Box
                                        draw_list->AddRect(slotP1Loop, slotP2Loop, ImGui::ColorConvertFloat4ToU32(eventTrackEditor->m_colors.m_trackBoundingBox), 0); //Track Bounding Box
                                        draw_list->AddText(textPLoop, ImGui::ColorConvertFloat4ToU32(eventTrackEditor->m_colors.m_trackTextColor), event_value.c_str()); //Event Value
                                    }
                                    else if (slotP1.x < frameMin.x)
                                    {
                                        if (slotP2.x > frameMin.x)
                                        {
                                            slotP2Loop = ImVec2(frameMax.x, slotP2.y);
                                            slotP1Loop = ImVec2(slotP2Loop.x - fmin(abs(slotP2.x - slotP1.x), abs(slotP1.x - frameMin.x)), slotP1.y);
                                        }
                                        else
                                        {
                                            slotP2Loop = ImVec2(frameMax.x - abs(slotP2.x - frameMin.x), slotP2.y);
                                            slotP1Loop = ImVec2(slotP2Loop.x - fmin(abs(slotP2.x - slotP1.x), abs(slotP1.x - frameMin.x)), slotP1.y);
                                        }

                                        textPLoop = ImVec2(slotP1Loop.x + (slotP2Loop.x - slotP1Loop.x - textSize.x) / 2, slotP2Loop.y + (slotP1Loop.y - slotP2Loop.y - textSize.y) / 2);

                                        draw_list->AddRectFilled(slotP1Loop, slotP2Loop, slotColor, 0); //Track Box
                                        draw_list->AddRect(slotP1Loop, slotP2Loop, ImGui::ColorConvertFloat4ToU32(eventTrackEditor->m_colors.m_trackBoundingBox), 0); //Track Bounding Box
                                        draw_list->AddText(textPLoop, ImGui::ColorConvertFloat4ToU32(eventTrackEditor->m_colors.m_trackTextColor), event_value.c_str()); //Event Value
                                    }
                                }
                                else
                                {
                                    if (slotD1.x + 5.f > frameMax.x)
                                    {
                                        slotD1Loop = ImVec2(frameMin.x + abs(slotD1.x - frameMax.x), slotD1.y);
                                        slotD2Loop = ImVec2(slotD1Loop.x + framePixelWidth, slotD2.y);

                                        slotT1Loop = ImVec2(slotD1Loop.x, slotD2Loop.y);
                                        slotT2Loop = ImVec2(slotD2Loop.x, slotD2Loop.y);

                                        slotT3Loop = ImVec2((slotT2Loop.x + slotT1Loop.x) / 2, (slotT2Loop.y + slotT1Loop.y) / 2 + 5);

                                        textPLoop = ImVec2(slotD1Loop.x + (slotD2Loop.x - slotD1Loop.x - textSize.x) / 2, slotD2Loop.y + (slotD1Loop.y - slotD2Loop.y - textSize.y) / 2);
                                        textDLoop = ImVec2(slotT2Loop.x + textSize.x / 3, slotT3Loop.y - textSizeIdx.y);

                                        draw_list->AddRectFilled(slotD1Loop, slotD2Loop, slotColor, 0); //Track Box
                                        draw_list->AddLine(slotD1Loop, ImVec2(slotD2Loop.x, slotD1Loop.y), boundColor);
                                        draw_list->AddLine(slotD2Loop, ImVec2(slotD2Loop.x, slotD1Loop.y), boundColor);
                                        draw_list->AddLine(slotD1Loop, ImVec2(slotD1Loop.x, slotD2Loop.y), boundColor);

                                        //draw_list->AddRect(slotD1, slotD2, boundColor, 0); //Track Bounding Box

                                        draw_list->AddTriangleFilled(slotT1Loop, slotT2Loop, slotT3Loop, slotColor);
                                        draw_list->AddLine(slotT1Loop, slotT3Loop, boundColor);
                                        draw_list->AddLine(slotT2Loop, slotT3Loop, boundColor);

                                        draw_list->AddText(textPLoop, ImGui::ColorConvertFloat4ToU32(eventTrackEditor->m_colors.m_trackTextColor), event_value.c_str()); //Event Value
                                        draw_list->AddText(textDLoop, ImGui::ColorConvertFloat4ToU32(eventTrackEditor->m_colors.m_trackTextColor), std::to_string(eventIdx).c_str()); //Event Idx
                                    }
                                    else if (slotD1.x + 5.f < frameMin.x)
                                    {
                                        slotD2Loop = ImVec2(frameMax.x - abs(frameMin.x - slotD2.x), slotD2.y);
                                        slotD1Loop = ImVec2(slotD2Loop.x - framePixelWidth, slotD1.y);

                                        slotT1Loop = ImVec2(slotD1Loop.x, slotD2Loop.y);
                                        slotT2Loop = ImVec2(slotD2Loop.x, slotD2Loop.y);

                                        slotT3Loop = ImVec2((slotT2Loop.x + slotT1Loop.x) / 2, (slotT2Loop.y + slotT1Loop.y) / 2 + 5);

                                        textPLoop = ImVec2(slotD1Loop.x + (slotD2Loop.x - slotD1Loop.x - textSize.x) / 2, slotD2Loop.y + (slotD1Loop.y - slotD2Loop.y - textSize.y) / 2);
                                        textDLoop = ImVec2(slotT2Loop.x + textSize.x / 3, slotT3Loop.y - textSizeIdx.y);

                                        draw_list->AddRectFilled(slotD1Loop, slotD2Loop, slotColor, 0); //Track Box
                                        draw_list->AddLine(slotD1Loop, ImVec2(slotD2Loop.x, slotD1Loop.y), boundColor);
                                        draw_list->AddLine(slotD2Loop, ImVec2(slotD2Loop.x, slotD1Loop.y), boundColor);
                                        draw_list->AddLine(slotD1Loop, ImVec2(slotD1Loop.x, slotD2Loop.y), boundColor);

                                        //draw_list->AddRect(slotD1, slotD2, boundColor, 0); //Track Bounding Box

                                        draw_list->AddTriangleFilled(slotT1Loop, slotT2Loop, slotT3Loop, slotColor);
                                        draw_list->AddLine(slotT1Loop, slotT3Loop, boundColor);
                                        draw_list->AddLine(slotT2Loop, slotT3Loop, boundColor);

                                        draw_list->AddText(textPLoop, ImGui::ColorConvertFloat4ToU32(eventTrackEditor->m_colors.m_trackTextColor), event_value.c_str()); //Event Value
                                        draw_list->AddText(textDLoop, ImGui::ColorConvertFloat4ToU32(eventTrackEditor->m_colors.m_trackTextColor), std::to_string(eventIdx).c_str()); //Event Idx
                                    }
                                }                              
                            }
                        }

                        //Drag
                        {
                            ImRect rects[3] = { ImRect(ImVec2(slotP1.x - framePixelWidth / 3, slotP1.y), ImVec2(slotP1.x, slotP2.y))
                                , ImRect(ImVec2(slotP2.x, slotP1.y), ImVec2(slotP2.x + framePixelWidth / 3, slotP2.y))
                                , ImRect(slotP1, slotP2) };

                            ImRect rect_discrete = ImRect(slotD1, slotD2);

                            ImRect rects_loop[3] = { ImRect(ImVec2(slotP1Loop.x - framePixelWidth / 3, slotP1Loop.y), ImVec2(slotP1Loop.x, slotP2Loop.y))
                                , ImRect(ImVec2(slotP2Loop.x, slotP1Loop.y), ImVec2(slotP2Loop.x + framePixelWidth / 3, slotP2Loop.y))
                                , ImRect(slotP1Loop, slotP2Loop) };

                            ImRect rect_discrete_loop = ImRect(slotD1Loop, slotD2Loop);

                            //rect[1] = start drag
                            //rect[2] = end drag
                            //rect[3] = event drag

                            if (slotP1.x > slotP2.x)
                                rects[2] = ImRect(ImVec2(slotP2.x, slotP1.y), ImVec2(slotP1.x, slotP2.y));

                            const unsigned int quadColor[] = { 0x20FFFFFF, 0x20FFFFFF, 0x20FFFFFF };

                            //Tracks
                            if (eventTrackEditor->focused && focused && movingTrack == -1 && (sequenceOptions & EDITOR_EVENT_EDIT_STARTEND))// TODOFOCUS && backgroundRect.Contains(io.MousePos))
                            {
                                if (!isDiscrete)
                                {
                                    for (int j = 0; j < 3; j++)
                                    {
                                        ImRect& rc = rects[j];
                                        if (!rc.Contains(io.MousePos) || popupOpened || MovingScrollBar || MovingCurrentFrame)
                                            continue;
                                        if (!ImRect(childFramePos, childFramePos + childFrameSize).Contains(io.MousePos))
                                            continue;

                                        if (j == 2 && (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow))
                                            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                                        else if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                                            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

                                        if (ImGui::IsMouseClicked(0))
                                        {
                                            if (j == 2 && (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow))
                                                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                                            else if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                                                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

                                            movingTrack = trackIndex;
                                            movingEvent = eventIdx;
                                            movingPos = cx;
                                            movingPart = j + 1;

                                            break;
                                        }
                                    }
                                }
                                else
                                {
                                    ImRect& rc = rect_discrete;

                                    if (rc.Contains(io.MousePos) && !popupOpened && !MovingScrollBar && !MovingCurrentFrame)
                                    {
                                        if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                                            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                                        if (!ImRect(childFramePos, childFramePos + childFrameSize).Contains(io.MousePos))
                                            continue;

                                        if (ImGui::IsMouseClicked(0))
                                        {
                                            if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                                                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                                            movingTrack = trackIndex;
                                            movingEvent = eventIdx;
                                            movingPos = cx;
                                            movingPart = 3;

                                            break;
                                        }
                                    }
                                }
                            }

                            //Looped entries
                            if (sequenceOptions & EDITOR_EVENT_LOOP)
                            {
                                if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && focused && movingTrack == -1 && (sequenceOptions & EDITOR_EVENT_EDIT_STARTEND))// TODOFOCUS && backgroundRect.Contains(io.MousePos))
                                {
                                    if (!isDiscrete)
                                    {
                                        for (int j = 0; j < 3; j++)
                                        {
                                            ImRect& rc = rects_loop[j];

                                            if (!rc.Contains(io.MousePos) || popupOpened || MovingScrollBar || MovingCurrentFrame)
                                                continue;

                                            if (j == 2 && (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow))
                                                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                                            else if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                                                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

                                            if (ImGui::IsMouseClicked(0) && !MovingScrollBar && !MovingCurrentFrame)
                                            {
                                                if (j == 2 && (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow))
                                                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                                                else if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                                                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

                                                movingTrack = trackIndex;
                                                movingEvent = eventIdx;
                                                movingPos = cx;
                                                movingPart = j + 1;

                                                break;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        ImRect& rc = rect_discrete_loop;

                                        if (rc.Contains(io.MousePos) && !popupOpened && !MovingScrollBar && !MovingCurrentFrame)
                                        {
                                            if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                                                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                                            if (!ImRect(childFramePos, childFramePos + childFrameSize).Contains(io.MousePos))
                                                continue;

                                            if (ImGui::IsMouseClicked(0))
                                            {
                                                if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                                                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                                                movingTrack = trackIndex;
                                                movingEvent = eventIdx;
                                                movingPos = cx;
                                                movingPart = 3;

                                                break;
                                            }
                                        }                                      
                                    }
                                }
                            }
                        }
                    }
                }

                // moving
                if (!popupOpened)
                {
                    if (movingTrack >= 0 && movingEvent >= 0)
                    {
                        ImGui::CaptureMouseFromApp();
                        int diffFrame = int((cx - movingPos) / framePixelWidth);

                        if (diffFrame != 0)
                            eventTrackEditor->SetEditedState(true);

                        if (std::abs(diffFrame) > 0)
                        {
                            if (selectedTrack)
                                *selectedTrack = movingTrack;

                            if (selectedEvent)
                                *selectedEvent = movingEvent;

                            EventTrackEditor::EventTrack* track = &eventTrackEditor->m_eventTracks[movingTrack];

                            if (movingPart == MovingPart_End)
                            {
                                if (track->m_event[movingEvent].m_duration + diffFrame >= 0)
                                    track->m_event[movingEvent].m_duration += diffFrame;
                                else
                                    track->m_event[movingEvent].m_duration = 0;

                                movingPos += int(diffFrame * framePixelWidth);
                            }
                            else if (track->m_event[movingEvent].m_frameStart + diffFrame >= -eventTrackEditor->GetFrameMax() && (track->m_event[movingEvent].m_frameStart + track->m_event[movingEvent].m_duration) + diffFrame <= 2 * eventTrackEditor->GetFrameMax())
                            {
                                if (movingPart == MovingPart_Start)
                                {
                                    if (track->m_event[movingEvent].m_duration - diffFrame >= 0)
                                    {
                                        track->m_event[movingEvent].m_frameStart += diffFrame;
                                        track->m_event[movingEvent].m_duration -= diffFrame;
                                    }
                                }
                                else if (movingPart == MovingPart_All)
                                    track->m_event[movingEvent].m_frameStart += diffFrame;

                                movingPos += int(diffFrame * framePixelWidth);
                            }
                            else
                            {
                                if (track->m_event[movingEvent].m_frameStart < -eventTrackEditor->GetFrameMax())
                                    track->m_event[movingEvent].m_frameStart = -eventTrackEditor->GetFrameMax();

                                if ((track->m_event[movingEvent].m_frameStart + track->m_event[movingEvent].m_duration) > 2 * eventTrackEditor->GetFrameMax())
                                    track->m_event[movingEvent].m_frameStart = (2 * eventTrackEditor->GetFrameMax() - track->m_event[movingEvent].m_duration);
                            }

                        }
                        if (!io.MouseDown[0] && !popupOpened)
                        {
                            // single select
                            if (!diffFrame && movingPart && selectedTrack && selectedEvent)
                            {
                                *selectedTrack = movingTrack;
                                *selectedEvent = movingEvent;
                                ret = true;
                            }

                            movingTrack = -1;
                            movingEvent = -1;
                        }
                    }
                }

                // clip start
                if (firstFrame)
                {
                    static const float cursorWidth = 1.f;

                    float cursorOffset = contentMin.x + legendWidth + (eventTrackEditor->GetFrameMin() - firstFrameUsed) * framePixelWidth - cursorWidth * 0.5f;
                    draw_list->AddLine(ImVec2(cursorOffset, canvas_pos.y), ImVec2(cursorOffset, contentMax.y), IM_COL32(255, 0, 0, 255), cursorWidth);
                }

                // clip end
                if (firstFrame)
                {
                    static const float cursorWidth = 1.f;

                    float cursorOffset = contentMin.x + legendWidth + (eventTrackEditor->GetFrameMax() - firstFrameUsed) * framePixelWidth - cursorWidth * 0.5f;
                    draw_list->AddLine(ImVec2(cursorOffset, canvas_pos.y), ImVec2(cursorOffset, contentMax.y), IM_COL32(255, 0, 0, 255), cursorWidth);
                }

                // cursor
                if (currentFrame && firstFrame && *currentFrame >= *firstFrame && *currentFrame <= eventTrackEditor->GetFrameMax())
                {
                    static const float cursorWidth = 1.f;

                    float cursorOffset = contentMin.x + legendWidth + (*currentFrame - firstFrameUsed) * framePixelWidth - cursorWidth * 0.5f;
                    draw_list->AddLine(ImVec2(cursorOffset, canvas_pos.y), ImVec2(cursorOffset, contentMax.y), ImGui::ColorConvertFloat4ToU32(eventTrackEditor->m_colors.m_cursorColor), cursorWidth);

                    char tmps[512];

                    if (*currentFrame != eventTrackEditor->GetFrameMin() && *currentFrame != eventTrackEditor->GetFrameMax())
                    {
                        draw_list->PopClipRect();
                        draw_list->PopClipRect();

                        ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%.3f", RMath::FrameToTime(*currentFrame));
                        draw_list->AddText(ImVec2(cursorOffset, canvas_pos.y), 0xFFFFFFFF, tmps);

                        draw_list->PushClipRect(childFramePos, childFramePos + childFrameSize, true);
                        draw_list->PushClipRect(childFramePos + ImVec2(float(legendWidth), 0.f), childFramePos + childFrameSize, true);
                    }
                }

                draw_list->PopClipRect();
                draw_list->PopClipRect();

                // copy paste
                if (sequenceOptions & EDITOR_COPYPASTE)
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

                    if (inRectCopy && io.MouseReleased[0] && !popupOpened)
                    {
                        //eventTrackEditor->Copy();
                    }
                    if (inRectPaste && io.MouseReleased[0] && !popupOpened)
                    {
                        //eventTrackEditor->Paste();
                    }
                }
            }

            ImGui::EndChildFrame();
            ImGui::PopStyleColor();
            if (hasScrollBar)
            {
                ImGui::InvisibleButton("scrollBar", scrollBarSize);
                ImVec2 scrollBarMin = ImGui::GetItemRectMin();
                ImVec2 scrollBarMax = ImGui::GetItemRectMax();

                // ratio = number of frames visible in control / number to total frames

                float startFrameOffset = ((float)(firstFrameUsed - eventTrackEditor->GetFrameMin()) / (float)frameCount) * (canvas_size.x - legendWidth);
                ImVec2 scrollBarA(scrollBarMin.x + legendWidth, scrollBarMin.y - 2);
                ImVec2 scrollBarB(scrollBarMin.x + canvas_size.x, scrollBarMax.y - 1);
                draw_list->AddRectFilled(scrollBarA, scrollBarB, 0xFF222222, 0);

                ImRect scrollBarRect(scrollBarA, scrollBarB);
                bool inScrollBar = scrollBarRect.Contains(io.MousePos);

                draw_list->AddRectFilled(scrollBarA, scrollBarB, 0xFF101010, 8);

                ImVec2 scrollBarC(scrollBarMin.x + legendWidth + startFrameOffset, scrollBarMin.y);
                ImVec2 scrollBarD(scrollBarMin.x + legendWidth + barWidthInPixels + startFrameOffset, scrollBarMax.y - 2);
                draw_list->AddRectFilled(scrollBarC, scrollBarD, (inScrollBar || MovingScrollBar) ? 0xFF606060 : 0xFF505050, 6);

                static bool sizingRBar = false;
                static bool sizingLBar = false;

                ImRect scrollBarThumb(scrollBarC, scrollBarD);
                static const float MinBarWidth = 44.f;

                if (MovingScrollBar)
                {
                    if (!io.MouseDown[0])
                    {
                        MovingScrollBar = false;
                    }
                    else
                    {
                        float framesPerPixelInBar = barWidthInPixels / (float)visibleFrameCount;
                        *firstFrame = int((io.MousePos.x - panningViewSource.x) / framesPerPixelInBar) - panningViewFrame;
                        *firstFrame = ImClamp(*firstFrame, eventTrackEditor->GetFrameMin(), ImMax(eventTrackEditor->GetFrameMax() - visibleFrameCount + 10, eventTrackEditor->GetFrameMin()));
                    }
                }
                else if (focused && eventTrackEditor->focused)
                {
                    if (scrollBarThumb.Contains(io.MousePos) && ImGui::IsMouseClicked(0) && firstFrame && !MovingCurrentFrame && movingTrack == -1 && !popupOpened && !resizeLegend)
                    {
                        MovingScrollBar = true;
                        panningViewSource = io.MousePos;
                        panningViewFrame = -*firstFrame;
                    }
                }
            }

            if (!popupOpened)
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


                    if (overCustomDraw)
                    {
                    }
                    else
                    {
                        int frameOverCursor = (int)((io.MousePos.x - topRect.Min.x) / framePixelWidth) + firstFrameUsed;

                        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
                        {
                            if (io.MouseWheel < -FLT_EPSILON)
                            {
                                framePixelWidthTarget -= 0.3f;
                                framePixelWidth -= 0.3f;
                                int newFrameOverCursor = (int)((io.MousePos.x - topRect.Min.x) / framePixelWidth) + firstFrameUsed;
                                *firstFrame += frameOverCursor - newFrameOverCursor;
                            }

                            if (io.MouseWheel > FLT_EPSILON)
                            {
                                framePixelWidthTarget += 0.3f;
                                framePixelWidth += 0.3f;
                                int newFrameOverCursor = (int)((io.MousePos.x - topRect.Min.x) / framePixelWidth) + firstFrameUsed;
                                *firstFrame += frameOverCursor - newFrameOverCursor;
                            }
                        }
                    }
                }
            }
        }

        ImGui::EndGroup();

        if (expanded)
        {
            if (sequenceOptions & EDITOR_COLLAPSE)
            {
                bool overExpanded = SequencerAddDelButton(draw_list, ImVec2(canvas_pos.x + 2, canvas_pos.y + 2), ImVec2(4, ItemHeight), !*expanded);
                if (overExpanded && io.MouseReleased[0] && !popupOpened)
                    *expanded = !*expanded;
            }
        }

        if (!ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel))
            popupOpened = false;

        if (framePixelWidth > 1.f && framePixelWidth < 50.f)
            *zoomLevel = framePixelWidth;

        return ret;
    }
    
    bool Sequencer(TimeActEditor* timeActEditor, int* currentFrame, int* selectedTrack, int* selectedEvent, bool* expanded, bool focused, int* firstFrame, float* zoomLevel, int sequenceOptions)
    {
        bool ret = false;
        ImGuiIO& io = ImGui::GetIO();
        int cx = (int)(io.MousePos.x);
        int cy = (int)(io.MousePos.y);
        float framePixelWidth = *zoomLevel;
        float framePixelWidthTarget = *zoomLevel;

        static bool resizeLegend = false;
        static int legendWidth = 210;

        static int movingTrack = -1;
        static int movingEvent = -1;
        static int movingPos = -1;

        enum MovingPart
        {
            MovingPart_None,
            MovingPart_Start,
            MovingPart_End,
            MovingPart_All
        };

        static int movingPart;

        bool reload = false;
        bool removeTrack = false;
        bool renameTrack = false;

        bool addTrack = false;
        static int addGroupId = 0;

        static TimeActEvent addEvent;

        bool delEvent = false;

        int ItemHeight = 23;

        static bool popupOpened = false;

        ImGui::BeginGroup();

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
        ImVec2 canvas_size = ImGui::GetContentRegionAvail();        // Resize canvas to what's available

        int firstFrameUsed = firstFrame ? *firstFrame : 0;

        int controlHeight = ImMax(timeActEditor->GetTrackCount() * ItemHeight, ItemHeight);

        int frameCount = ImMax(timeActEditor->GetFrameMax() - timeActEditor->GetFrameMin() + 10, 1);

        canvas_size.y = controlHeight + 2.5 * ItemHeight;

        static bool MovingScrollBar = false;
        static bool MovingCurrentFrame = false;
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
        const int visibleFrameCount = (int)floorf((canvas_size.x - legendWidth) / framePixelWidth);
        const float barWidthRatio = ImMin(visibleFrameCount / (float)frameCount, 1.f);
        const float barWidthInPixels = barWidthRatio * (canvas_size.x - legendWidth);

        ImRect regionRect(canvas_pos, canvas_pos + canvas_size);

        static bool panningView = false;
        static ImVec2 panningViewSource;
        static int panningViewFrame;
        if (ImGui::IsWindowFocused() && io.KeyAlt && io.MouseDown[2])
        {
            if (!panningView)
            {
                panningViewSource = io.MousePos;
                panningView = true;
                panningViewFrame = *firstFrame;
            }
            *firstFrame = panningViewFrame - int((io.MousePos.x - panningViewSource.x) / framePixelWidth);
            *firstFrame = ImClamp(*firstFrame, timeActEditor->GetFrameMin(), timeActEditor->GetFrameMax() - visibleFrameCount);
        }
        if (panningView && !io.MouseDown[2])
        {
            panningView = false;
        }
        framePixelWidthTarget = ImClamp(framePixelWidthTarget, 0.1f, 50.f);

        framePixelWidth = ImLerp(framePixelWidth, framePixelWidthTarget, 0.33f);

        frameCount = timeActEditor->GetFrameMax() - timeActEditor->GetFrameMin() + 10;
        if (visibleFrameCount >= frameCount && firstFrame)
            *firstFrame = timeActEditor->GetFrameMin();

        // --
        if (expanded && !*expanded)
        {
            ImGui::InvisibleButton("canvas", ImVec2(canvas_size.x - canvas_pos.x, (float)ItemHeight));
            draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_size.x + canvas_pos.x, canvas_pos.y + ItemHeight), 0xFF3D3837, 0);
            char tmps[512];
            ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%d Frames / %d tracks", frameCount, timeActEditor->GetTrackCount());
            draw_list->AddText(ImVec2(canvas_pos.x + 26, canvas_pos.y + 2), 0xFFFFFFFF, tmps);
        }
        else
        {
            bool hasScrollBar(true);

            // test scroll area
            ImVec2 headerSize(canvas_size.x, (float)ItemHeight);
            ImVec2 scrollBarSize(canvas_size.x, 14.f);
            ImGui::InvisibleButton("topBar", headerSize);
            draw_list->AddRectFilled(canvas_pos, canvas_pos + headerSize, 0xFFFF0000, 0);
            ImVec2 childFramePos = ImGui::GetCursorScreenPos();
            ImVec2 childFrameSize(canvas_size.x, canvas_size.y - 8.f - headerSize.y - (hasScrollBar ? scrollBarSize.y : 0));

            ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);

            ImGui::BeginChildFrame(889, childFrameSize);
            timeActEditor->focused = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
            ImGui::InvisibleButton("contentBar", ImVec2(canvas_size.x, float(controlHeight)));
            const ImVec2 contentMin = ImGui::GetItemRectMin();
            const ImVec2 contentMax = ImGui::GetItemRectMax();
            const ImRect contentRect(contentMin, contentMax);
            const float contentHeight = contentMax.y - contentMin.y;

            // full background
            draw_list->AddRectFilled(canvas_pos, canvas_pos + canvas_size, 0xFF242424, 0);

            // current frame top
            ImRect topRect(ImVec2(canvas_pos.x + legendWidth, canvas_pos.y), ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + ItemHeight));
            ImRect legendResizeRect(ImVec2(canvas_pos.x + legendWidth - 2, canvas_pos.y), ImVec2(canvas_pos.x + legendWidth + 2, canvas_pos.y + ItemHeight));

            if (focused && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) && (!popupOpened && !MovingCurrentFrame && !MovingScrollBar && movingTrack == -1 && legendResizeRect.Contains(io.MousePos)) || resizeLegend)
            {
                if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

                if (io.MouseDown[0])
                    resizeLegend = true;
            }

            if (resizeLegend)
            {
                legendWidth = (int)((io.MousePos.x - contentMin.x));
            }

            if (resizeLegend && io.MouseReleased[0])
                resizeLegend = false;

            //header
            draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_size.x + canvas_pos.x, canvas_pos.y + ItemHeight), 0xFF404040, 0);
            if (sequenceOptions & EDITOR_TRACK_ADD && (timeActEditor->m_source != nullptr))
            {
                if (SequencerAddTrackButton(draw_list, ImVec2(canvas_pos.x + legendWidth - 8, canvas_pos.y + 2), ImVec2(4, ItemHeight * 0.8f)) && focused && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) && !popupOpened && !MovingCurrentFrame && !MovingScrollBar && movingTrack == -1 && !legendResizeRect.Contains(io.MousePos))
                {
                    if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                    if (io.MouseReleased[0])
                    {
                        popupOpened = true;
                        ImGui::OpenPopup("addTrack");
                    }
                }

                ImGui::PopStyleColor();
                if (ImGui::BeginPopup("addTrack") && popupOpened)
                {
                    ImGui::Text("Add Track");
                    ImGui::Separator();

                    ImGui::InputInt("Group ID", &addGroupId);

                    if (ImGui::Button("Add Track") || GetAsyncKeyState(VK_RETURN) & 1)
                    {
                        timeActEditor->AddGroup(addGroupId);
                        ImGui::CloseCurrentPopup();

                        *selectedTrack = -1;
                        *selectedEvent = -1;

                        reload = true;
                    }

                    ImGui::EndPopup();
                }

                ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);
            }

            if (sequenceOptions & EDITOR_CHANGE_FRAME && currentFrame && ((topRect.Contains(io.MousePos) && !popupOpened && !MovingScrollBar && movingTrack == -1 && !legendResizeRect.Contains(io.MousePos) && !resizeLegend) || MovingCurrentFrame))
            {
                if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                if (!resizeLegend && !popupOpened && !MovingScrollBar && movingTrack == -1 && io.MouseDown[0])
                    MovingCurrentFrame = true;
            }

            if (MovingCurrentFrame)
            {
                if (frameCount)
                {
                    *currentFrame = (int)((io.MousePos.x - topRect.Min.x) / framePixelWidth) + firstFrameUsed;
                    if (*currentFrame < timeActEditor->GetFrameMin())
                        *currentFrame = timeActEditor->GetFrameMin();
                    if (*currentFrame >= timeActEditor->GetFrameMax())
                        *currentFrame = timeActEditor->GetFrameMax();
                }
                if (!io.MouseDown[0])
                    MovingCurrentFrame = false;
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
                bool baseIndex = ((i % modFrameCount) == 0) || (i == timeActEditor->GetFrameMax() || i == timeActEditor->GetFrameMin());
                bool halfIndex = (i % halfModFrameCount) == 0;
                ImVec2 pos = ImVec2(contentMin.x + legendWidth - firstFrameUsed * framePixelWidth, contentMin.y + ItemHeight * i + 1);
                float px = pos.x + i * framePixelWidth - 1;
                //int px = (int)canvas_pos.x + int(i * framePixelWidth) + legendWidth + int(firstFrameUsed * framePixelWidth) + 3;
                int tiretStart = baseIndex ? 4 : (halfIndex ? 10 : 14);
                int tiretEnd = baseIndex ? regionHeight : ItemHeight;

                if (px <= (canvas_size.x + canvas_pos.x) && px >= (canvas_pos.x + legendWidth))
                {
                    draw_list->AddLine(ImVec2((float)px, canvas_pos.y + (float)tiretStart), ImVec2((float)px, canvas_pos.y + (float)tiretEnd - 1), 0xFF606060, 1);

                    draw_list->AddLine(ImVec2((float)px, canvas_pos.y + (float)ItemHeight), ImVec2((float)px, canvas_pos.y + (float)regionHeight - 1), 0x30606060, 1);
                }

                if (baseIndex && px > (canvas_pos.x + legendWidth))
                {
                    char tmps[512];
                    ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%.3f", RMath::FrameToTime(i, 30));
                    draw_list->AddText(ImVec2((float)(px + 6), canvas_pos.y), 0xFFFFFFFF, tmps);
                }
                };

            auto drawLineContent = [&](int i, int /*regionHeight*/) {
                //int px = (int)canvas_pos.x + int(i * framePixelWidth) + legendWidth + int(firstFrameUsed * framePixelWidth) + 3;
                ImVec2 pos = ImVec2(contentMin.x + legendWidth - firstFrameUsed * framePixelWidth, contentMin.y + ItemHeight * i + 1);
                float px = pos.x + i * framePixelWidth - 1;
                int tiretStart = int(contentMin.y);
                int tiretEnd = int(contentMax.y);

                if (px <= (canvas_size.x + canvas_pos.x) && px >= (canvas_pos.x + legendWidth))
                {
                    //draw_list->AddLine(ImVec2((float)px, canvas_pos.y + (float)tiretStart), ImVec2((float)px, canvas_pos.y + (float)tiretEnd - 1), 0xFF606060, 1);

                    draw_list->AddLine(ImVec2(float(px), float(tiretStart)), ImVec2(float(px), float(tiretEnd)), 0x30606060, 1);
                }
                };
            for (int i = timeActEditor->GetFrameMin() + 1; i <= timeActEditor->GetFrameMax(); i += frameStep)
            {
                drawLine(i, ItemHeight);
            }
            drawLine(timeActEditor->GetFrameMin(), ItemHeight);
            drawLine(timeActEditor->GetFrameMax(), ItemHeight);

            for (int i = timeActEditor->GetFrameMax() + 1; i <= 500; i += frameStep)
            {
                drawLine(i, ItemHeight);
            }

            // clip content
            draw_list->PushClipRect(childFramePos, childFramePos + childFrameSize, true);

            // draw item names in the legend rect on the left
            size_t customHeight = 0;
            for (int i = 0; i < timeActEditor->GetTrackCount(); i++)
            {
                ImVec2 tpos(contentMin.x + 3, contentMin.y + i * ItemHeight + 2 + customHeight);

                ImVec2 labelEndPos = ImVec2(legendWidth - 15, ItemHeight * 0.9f);

                if (sequenceOptions & EDITOR_TRACK_ADD)
                    labelEndPos.x -= ItemHeight;

                if (sequenceOptions & EDITOR_EVENT_ADD)
                    labelEndPos.x -= ItemHeight;

                bool overLabel = SequencerAddTrackLabel(draw_list, ImVec2(tpos.x, tpos.y), labelEndPos, (char*)timeActEditor->GetTrackName(i).c_str());

                if (sequenceOptions & EDITOR_TRACK_ADD)
                {
                    if (SequencerAddRemoveButton(draw_list, ImVec2(contentMin.x + legendWidth - ItemHeight - ItemHeight + 2 - 10, tpos.y + 2), ImVec2(ItemHeight * 0.8f, ItemHeight * 0.8f)) && focused && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) && !popupOpened && !MovingCurrentFrame && !MovingScrollBar && movingTrack == -1)
                    {
                        if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                        if (io.MouseReleased[0])
                        {
                            *selectedTrack = i;
                            *selectedEvent = -1;
                            removeTrack = true;
                        }
                    }                
                }

                if (sequenceOptions & EDITOR_TRACK_RENAME)
                {
                    if (focused && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) && overLabel && !popupOpened && !MovingCurrentFrame && !MovingScrollBar && movingTrack == -1)
                    {
                        if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                        if (io.MouseReleased[0])
                        {
                            *selectedTrack = i;
                            *selectedEvent = -1;
                            renameTrack = true;
                        }
                    }
                }

                if (sequenceOptions & EDITOR_EVENT_ADD)
                {
                    if (SequencerAddDelButton(draw_list, ImVec2(contentMin.x + legendWidth - ItemHeight + 2 - 10, tpos.y + 2), ImVec2(ItemHeight * 0.8f, ItemHeight * 0.8f), true) && focused && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) && !popupOpened && !MovingCurrentFrame && !MovingScrollBar && movingTrack == -1)
                    {
                        if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                        if (io.MouseReleased[0])
                        {
                            addTrack = true;
                            *selectedTrack = i;
                        }
                    }

                    if (!popupOpened && !MovingCurrentFrame && !MovingScrollBar && movingTrack == -1)
                    {
                        for (size_t j = 0; j < timeActEditor->m_tracks[i].m_count; j++)
                        {
                            if (*selectedTrack == i && *selectedEvent == j)
                            {
                                if (GetAsyncKeyState(VK_DELETE) & 1)
                                    delEvent = true;
                            }
                        }
                    }
                }
            }

            ImGui::PopStyleColor();
            if (removeTrack && !reload)
            {
                popupOpened = true;
                ImGui::OpenPopup("deleteTrack");
            }

            if (ImGui::BeginPopup("deleteTrack") && popupOpened)
            {
                removeTrack = false;

                std::string header = timeActEditor->GetTrackName(*selectedTrack);

                ImGui::Text(header.c_str());
                ImGui::Separator();

                if (ImGui::Button("Delete Track") || GetAsyncKeyState(VK_RETURN) & 1)
                {
                    reload = true;

                    timeActEditor->DeleteGroup(*selectedTrack);
                    *selectedTrack = -1;
                    *selectedEvent = -1;

                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            if (renameTrack && !reload)
            {
                popupOpened = true;
                ImGui::OpenPopup("renameTrack");
            }

            if (ImGui::BeginPopup("renameTrack") && popupOpened)
            {
                renameTrack = false;

                std::string header = timeActEditor->GetTrackName(*selectedTrack);

                ImGui::Text(header.c_str());
                ImGui::Separator();

                ImGui::InputInt("ID", &timeActEditor->m_tracks[*selectedTrack].m_eventGroup);

                if (GetAsyncKeyState(VK_RETURN))
                    ImGui::CloseCurrentPopup();

                ImGui::EndPopup();
            }

            if (delEvent && !reload)
            {
                popupOpened = true;
                ImGui::OpenPopup("deleteEvent");
            }

            if (ImGui::BeginPopup("deleteEvent") && popupOpened)
            {
                delEvent = false;

                std::string header = timeActEditor->GetTrackName(*selectedTrack) + " [" + std::to_string(*selectedEvent) + "]";

                ImGui::Text(header.c_str());
                ImGui::Separator();

                if (ImGui::Button("Delete Event") || GetAsyncKeyState(VK_RETURN) & 1)
                {
                    timeActEditor->DeleteEvent(*selectedTrack, *selectedEvent);

                    *selectedTrack = -1;
                    *selectedEvent = -1;

                    ImGui::CloseCurrentPopup();

                    reload = true;
                }

                ImGui::EndPopup();
            }

            if (addTrack && !reload)
            {
                popupOpened = true;
                ImGui::OpenPopup("addEvent");
            }

            if (ImGui::BeginPopup("addEvent") && popupOpened)
            {
                addTrack = false;

                ImGui::Text(timeActEditor->GetTrackName(*selectedTrack).c_str());
                ImGui::Separator();

                if (addEvent.m_end < addEvent.m_start)
                    addEvent.m_end = addEvent.m_start;

                ImGui::InputFloat("Start", &addEvent.m_start, 1.f / 30.f);
                ImGui::InputFloat("End", &addEvent.m_end, 1.f / 30.f);

                *currentFrame = RMath::TimeToFrame(addEvent.m_start, 30);

                ImGui::InputInt("Event ID", &addEvent.m_eventData->m_id);

                if (ImGui::Button("Add Event") || GetAsyncKeyState(VK_RETURN) & 1)
                {
                    timeActEditor->AddEvent(*selectedTrack, addEvent);
                    ImGui::CloseCurrentPopup();

                    reload = true;
                }

                ImGui::EndPopup();
            }
            ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);

            if (!reload)
            {
                if (*currentFrame > 0 && !popupOpened)
                    addEvent.m_start = RMath::FrameToTime(*currentFrame, 30);

                // clipping rect so items bars are not visible in the legend on the left when scrolled

                // slots background
                for (int i = 0; i < timeActEditor->GetTrackCount(); i++)
                {
                    unsigned int col = 0xFF313131;

                    ImVec2 pos = ImVec2(contentMin.x + legendWidth, contentMin.y + ItemHeight * i + 1 + customHeight);
                    ImVec2 sz = ImVec2(canvas_size.x + canvas_pos.x, pos.y + ItemHeight - 1);

                    draw_list->AddRectFilled(pos, sz, col, 0);

                    if (!popupOpened && cy >= pos.y && cy < pos.y + ItemHeight && movingTrack == -1 && cx>contentMin.x && cx < contentMin.x + canvas_size.x)
                    {
                        draw_list->AddRectFilled(pos, sz, 0x10FFFFFF, 0);
                    }
                }

                draw_list->PushClipRect(childFramePos + ImVec2(float(legendWidth), 0.f), childFramePos + childFrameSize, true);

                // vertical frame lines in content area
                for (int i = timeActEditor->GetFrameMin() + 1; i <= timeActEditor->GetFrameMax(); i += frameStep)
                {
                    drawLineContent(i, int(contentHeight));
                }
                drawLineContent(timeActEditor->GetFrameMin(), int(contentHeight));
                drawLineContent(timeActEditor->GetFrameMax(), int(contentHeight));

                for (int i = timeActEditor->GetFrameMax() + 1; i <= 500; i += frameStep)
                {
                    drawLineContent(i, ItemHeight);
                }

                draw_list->AddRectFilled(ImVec2(contentMin.x + legendWidth - firstFrameUsed * framePixelWidth + timeActEditor->GetFrameMax() * framePixelWidth, canvas_pos.y), canvas_pos + canvas_size, 0x40000000, 0);

                // tracks
                customHeight = 0;
                for (int trackIndex = 0; trackIndex < timeActEditor->GetTrackCount(); trackIndex++)
                {
                    unsigned int color;
                    unsigned int slotColor = ImGui::ColorConvertFloat4ToU32(timeActEditor->m_colors.m_trackColor);
                    unsigned int slotColorHalf = slotColor | 0x40000000;
                    unsigned int boundColor = ImGui::ColorConvertFloat4ToU32(timeActEditor->m_colors.m_trackBoundingBox);

                    TimeActEditor::TimeActTrack* track = &timeActEditor->m_tracks[trackIndex];

                    if (focused == false)
                        slotColor = ImGui::ColorConvertFloat4ToU32(timeActEditor->m_colors.m_trackColorInactive);

                    for (int eventIdx = 0; eventIdx < track->m_count; eventIdx++)
                    {
                        std::string event_value = timeActEditor->GetEventLabel(trackIndex, eventIdx);

                        if ((sequenceOptions & EDITOR_MARK_ACTIVE_EVENTS) && *currentFrame > -1)
                        {
                            if (track->IsEventActive(eventIdx, *currentFrame))
                            {
                                slotColor = ImGui::ColorConvertFloat4ToU32(timeActEditor->m_colors.m_trackColorActive);
                                boundColor = ImGui::ColorConvertFloat4ToU32(timeActEditor->m_colors.m_trackBoundingBoxActive);
                            }
                            else
                            {
                                slotColor = ImGui::ColorConvertFloat4ToU32(timeActEditor->m_colors.m_trackColor);
                                boundColor = ImGui::ColorConvertFloat4ToU32(timeActEditor->m_colors.m_trackBoundingBox);
                            }
                        }

                        ImVec2 pos = ImVec2(contentMin.x + legendWidth - firstFrameUsed * framePixelWidth, contentMin.y + ItemHeight * trackIndex + 1 + customHeight);
                        ImVec2 slotP1(pos.x + track->m_event[eventIdx].m_frameStart * framePixelWidth - 1, pos.y + 2);
                        ImVec2 slotP2(pos.x + (track->m_event[eventIdx].m_duration + track->m_event[eventIdx].m_frameStart) * framePixelWidth - 1, pos.y + ItemHeight - 2);
                        ImVec2 slotP1Loop;
                        ImVec2 slotP2Loop;                        

                        ImVec2 frameMin = ImVec2(pos.x - 1, 0);
                        ImVec2 frameMax = ImVec2(frameMin.x + timeActEditor->GetFrameMax() * framePixelWidth, 0);

                        ImVec2 textSize = ImGui::CalcTextSize(event_value.c_str());
                        ImVec2 textSizeIdx = ImGui::CalcTextSize(std::to_string(eventIdx).c_str());
                        ImVec2 textP(ImMax(frameMin.x, (slotP1.x + 4)), slotP2.y + (slotP1.y - slotP2.y - textSize.y) / 2);
                        ImVec2 textPLoop;

                        if ((slotP1.x <= (canvas_size.x + contentMin.x) || slotP1.x >= (contentMin.x + legendWidth)) && (slotP2.x >= (contentMin.x + legendWidth) || slotP2.x <= (canvas_size.x + contentMin.x)))
                        {                  
                            draw_list->AddRectFilled(slotP1, slotP2, slotColor, 0); //Track Box
                            draw_list->AddRect(slotP1, slotP2, boundColor, 0); //Track Bounding Box
                            draw_list->AddText(textP, ImGui::ColorConvertFloat4ToU32(timeActEditor->m_colors.m_trackTextColor), event_value.c_str()); //Event Value

                            if (sequenceOptions & EDITOR_EVENT_LOOP)
                            {
                                if (slotP2.x > frameMax.x)
                                {
                                    if (slotP1.x < frameMax.x)
                                    {
                                        slotP1Loop = ImVec2(frameMin.x, slotP1.y);
                                        slotP2Loop = ImVec2(slotP1Loop.x + fmin(abs(slotP2.x - slotP1.x), abs(slotP2.x - frameMax.x)), slotP2.y);
                                    }
                                    else
                                    {
                                        slotP1Loop = ImVec2(frameMin.x + abs(slotP1.x - frameMax.x), slotP1.y);
                                        slotP2Loop = ImVec2(slotP1Loop.x + fmin(abs(slotP2.x - slotP1.x), abs(slotP2.x - frameMax.x)), slotP2.y);
                                    }

                                    textPLoop = ImVec2(slotP1Loop.x + (slotP2Loop.x - slotP1Loop.x - textSize.x) / 2, slotP2Loop.y + (slotP1Loop.y - slotP2Loop.y - textSize.y) / 2);

                                    draw_list->AddRectFilled(slotP1Loop, slotP2Loop, slotColor, 0); //Track Box
                                    draw_list->AddRect(slotP1Loop, slotP2Loop, ImGui::ColorConvertFloat4ToU32(timeActEditor->m_colors.m_trackBoundingBox), 0); //Track Bounding Box
                                    draw_list->AddText(textPLoop, ImGui::ColorConvertFloat4ToU32(timeActEditor->m_colors.m_trackTextColor), event_value.c_str()); //Event Value
                                }
                                else if (slotP1.x < frameMin.x)
                                {
                                    if (slotP2.x > frameMin.x)
                                    {
                                        slotP2Loop = ImVec2(frameMax.x, slotP2.y);
                                        slotP1Loop = ImVec2(slotP2Loop.x - fmin(abs(slotP2.x - slotP1.x), abs(slotP1.x - frameMin.x)), slotP1.y);
                                    }
                                    else
                                    {
                                        slotP2Loop = ImVec2(frameMax.x - abs(slotP2.x - frameMin.x), slotP2.y);
                                        slotP1Loop = ImVec2(slotP2Loop.x - fmin(abs(slotP2.x - slotP1.x), abs(slotP1.x - frameMin.x)), slotP1.y);
                                    }

                                    textPLoop = ImVec2(slotP1Loop.x + (slotP2Loop.x - slotP1Loop.x - textSize.x) / 2, slotP2Loop.y + (slotP1Loop.y - slotP2Loop.y - textSize.y) / 2);

                                    draw_list->AddRectFilled(slotP1Loop, slotP2Loop, slotColor, 0); //Track Box
                                    draw_list->AddRect(slotP1Loop, slotP2Loop, ImGui::ColorConvertFloat4ToU32(timeActEditor->m_colors.m_trackBoundingBox), 0); //Track Bounding Box
                                    draw_list->AddText(textPLoop, ImGui::ColorConvertFloat4ToU32(timeActEditor->m_colors.m_trackTextColor), event_value.c_str()); //Event Value
                                }                               
                            }
                        }

                        //Drag
                        {
                            ImRect rects[3] = { ImRect(ImVec2(slotP1.x - framePixelWidth / 3, slotP1.y), ImVec2(slotP1.x, slotP2.y))
                                , ImRect(ImVec2(slotP2.x, slotP1.y), ImVec2(slotP2.x + framePixelWidth / 3, slotP2.y))
                                , ImRect(slotP1, slotP2) };

                            ImRect rects_loop[3] = { ImRect(ImVec2(slotP1Loop.x - framePixelWidth / 3, slotP1Loop.y), ImVec2(slotP1Loop.x, slotP2Loop.y))
                                , ImRect(ImVec2(slotP2Loop.x, slotP1Loop.y), ImVec2(slotP2Loop.x + framePixelWidth / 3, slotP2Loop.y))
                                , ImRect(slotP1Loop, slotP2Loop) };

                            //rect[1] = start drag
                            //rect[2] = end drag
                            //rect[3] = event drag

                            if (slotP1.x > slotP2.x)
                                rects[2] = ImRect(ImVec2(slotP2.x, slotP1.y), ImVec2(slotP1.x, slotP2.y));

                            const unsigned int quadColor[] = { 0x20FFFFFF, 0x20FFFFFF, 0x20FFFFFF };

                            //Tracks
                            if (focused && timeActEditor->focused && movingTrack == -1 && (sequenceOptions & EDITOR_EVENT_EDIT_STARTEND))// TODOFOCUS && backgroundRect.Contains(io.MousePos))
                            {
                                for (int j = 0; j < 3; j++)
                                {
                                    ImRect& rc = rects[j];
                                    if (!rc.Contains(io.MousePos) || popupOpened || MovingScrollBar || MovingCurrentFrame)
                                        continue;
                                    if (!ImRect(childFramePos, childFramePos + childFrameSize).Contains(io.MousePos))
                                        continue;

                                    if (j == 2 && (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow))
                                        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                                    else if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                                        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

                                    if (ImGui::IsMouseClicked(0))
                                    {
                                        if (j == 2 && (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow))
                                            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                                        else if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                                            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

                                        movingTrack = trackIndex;
                                        movingEvent = eventIdx;
                                        movingPos = cx;
                                        movingPart = j + 1;

                                        break;
                                    }
                                }
                            }

                            //Looped entries
                            if (sequenceOptions & EDITOR_EVENT_LOOP)
                            {
                                if (focused && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) && movingTrack == -1 && (sequenceOptions & EDITOR_EVENT_EDIT_STARTEND))// TODOFOCUS && backgroundRect.Contains(io.MousePos))
                                {
                                    for (int j = 0; j < 3; j++)
                                    {
                                        ImRect& rc = rects_loop[j];

                                        if (!rc.Contains(io.MousePos) || popupOpened || MovingScrollBar || MovingCurrentFrame)
                                            continue;

                                        if (j == 2 && (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow))
                                            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                                        else if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                                            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

                                        if (ImGui::IsMouseClicked(0) && !MovingScrollBar && !MovingCurrentFrame)
                                        {
                                            if (j == 2 && (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow))
                                                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                                            else if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
                                                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

                                            movingTrack = trackIndex;
                                            movingEvent = eventIdx;
                                            movingPos = cx;
                                            movingPart = j + 1;

                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // moving
                if (!popupOpened)
                {
                    if (movingTrack >= 0 && movingEvent >= 0)
                    {
                        ImGui::CaptureMouseFromApp();
                        int diffFrame = int((cx - movingPos) / framePixelWidth);

                        if (diffFrame != 0)
                            timeActEditor->SetEditedState(true);

                        if (std::abs(diffFrame) > 0)
                        {
                            if (selectedTrack)
                                *selectedTrack = movingTrack;

                            if (selectedEvent)
                                *selectedEvent = movingEvent;

                            TimeActEditor::TimeActTrack* track = &timeActEditor->m_tracks[movingTrack];

                            if (movingPart == MovingPart_End)
                            {
                                if (track->m_event[movingEvent].m_duration + diffFrame >= 0)
                                    track->m_event[movingEvent].m_duration += diffFrame;
                                else
                                    track->m_event[movingEvent].m_duration = 0;

                                movingPos += int(diffFrame * framePixelWidth);
                            }
                            else if (movingPart == MovingPart_Start)
                            {
                                if (track->m_event[movingEvent].m_duration - diffFrame >= 0)
                                {
                                    track->m_event[movingEvent].m_frameStart += diffFrame;
                                    track->m_event[movingEvent].m_duration -= diffFrame;
                                }
                            }
                            else if (movingPart == MovingPart_All)
                                track->m_event[movingEvent].m_frameStart += diffFrame;

                            movingPos += int(diffFrame * framePixelWidth);                      
                        }
                        if (!io.MouseDown[0] && !popupOpened)
                        {
                            // single select
                            if (!diffFrame && movingPart && selectedTrack && selectedEvent)
                            {
                                *selectedTrack = movingTrack;
                                *selectedEvent = movingEvent;
                                ret = true;
                            }

                            movingTrack = -1;
                            movingEvent = -1;
                        }
                    }
                }

                // cursor
                if (currentFrame && firstFrame && *currentFrame >= *firstFrame && *currentFrame <= timeActEditor->GetFrameMax())
                {
                    static const float cursorWidth = 1.f;

                    float cursorOffset = contentMin.x + legendWidth + (*currentFrame - firstFrameUsed) * framePixelWidth - cursorWidth * 0.5f;
                    draw_list->AddLine(ImVec2(cursorOffset, canvas_pos.y), ImVec2(cursorOffset, contentMax.y), ImGui::ColorConvertFloat4ToU32(timeActEditor->m_colors.m_cursorColor), cursorWidth);

                    char tmps[512];

                    if (*currentFrame != timeActEditor->GetFrameMin() && *currentFrame != timeActEditor->GetFrameMax())
                    {
                        draw_list->PopClipRect();
                        draw_list->PopClipRect();

                        ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%.3f", RMath::FrameToTime(*currentFrame, 30));
                        draw_list->AddText(ImVec2(cursorOffset, canvas_pos.y), 0xFFFFFFFF, tmps);

                        draw_list->PushClipRect(childFramePos, childFramePos + childFrameSize, true);
                        draw_list->PushClipRect(childFramePos + ImVec2(float(legendWidth), 0.f), childFramePos + childFrameSize, true);
                    }
                }

                draw_list->PopClipRect();
                draw_list->PopClipRect();

                // copy paste
                if (sequenceOptions & EDITOR_COPYPASTE)
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

                    if (inRectCopy && io.MouseReleased[0] && !popupOpened)
                    {
                        //timeActEditor->Copy();
                    }
                    if (inRectPaste && io.MouseReleased[0] && !popupOpened)
                    {
                        //timeActEditor->Paste();
                    }
                }
            }

            ImGui::EndChildFrame();
            ImGui::PopStyleColor();
            if (hasScrollBar)
            {
                ImGui::InvisibleButton("scrollBar", scrollBarSize);
                ImVec2 scrollBarMin = ImGui::GetItemRectMin();
                ImVec2 scrollBarMax = ImGui::GetItemRectMax();

                // ratio = number of frames visible in control / number to total frames

                float startFrameOffset = ((float)(firstFrameUsed - timeActEditor->GetFrameMin()) / (float)frameCount) * (canvas_size.x - legendWidth);
                ImVec2 scrollBarA(scrollBarMin.x + legendWidth, scrollBarMin.y - 2);
                ImVec2 scrollBarB(scrollBarMin.x + canvas_size.x, scrollBarMax.y - 1);
                draw_list->AddRectFilled(scrollBarA, scrollBarB, 0xFF222222, 0);

                ImRect scrollBarRect(scrollBarA, scrollBarB);
                bool inScrollBar = scrollBarRect.Contains(io.MousePos);

                draw_list->AddRectFilled(scrollBarA, scrollBarB, 0xFF101010, 8);

                ImVec2 scrollBarC(scrollBarMin.x + legendWidth + startFrameOffset, scrollBarMin.y);
                ImVec2 scrollBarD(scrollBarMin.x + legendWidth + barWidthInPixels + startFrameOffset, scrollBarMax.y - 2);
                draw_list->AddRectFilled(scrollBarC, scrollBarD, (inScrollBar || MovingScrollBar) ? 0xFF606060 : 0xFF505050, 6);

                ImRect scrollBarThumb(scrollBarC, scrollBarD);
                static const float MinBarWidth = 44.f;

                if (MovingScrollBar)
                {
                    if (!io.MouseDown[0])
                    {
                        MovingScrollBar = false;
                    }
                    else
                    {
                        float framesPerPixelInBar = barWidthInPixels / (float)visibleFrameCount;
                        *firstFrame = int((io.MousePos.x - panningViewSource.x) / framesPerPixelInBar) - panningViewFrame;
                        *firstFrame = ImClamp(*firstFrame, timeActEditor->GetFrameMin(), ImMax(timeActEditor->GetFrameMax() - visibleFrameCount + 10, timeActEditor->GetFrameMin()));
                    }
                }
                else if (focused && timeActEditor->focused)
                {
                    if (scrollBarThumb.Contains(io.MousePos) && ImGui::IsMouseClicked(0) && firstFrame && !MovingCurrentFrame && movingTrack == -1 && !popupOpened && !resizeLegend)
                    {
                        MovingScrollBar = true;
                        panningViewSource = io.MousePos;
                        panningViewFrame = -*firstFrame;
                    }
                }
            }

            if (!popupOpened)
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


                    if (overCustomDraw)
                    {
                    }
                    else
                    {
                        int frameOverCursor = (int)((io.MousePos.x - topRect.Min.x) / framePixelWidth) + firstFrameUsed;

                        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
                        {
                            if (io.MouseWheel < -FLT_EPSILON)
                            {
                                framePixelWidthTarget -= 0.3f;
                                framePixelWidth -= 0.3f;
                                int newFrameOverCursor = (int)((io.MousePos.x - topRect.Min.x) / framePixelWidth) + firstFrameUsed;
                                *firstFrame += frameOverCursor - newFrameOverCursor;
                            }

                            if (io.MouseWheel > FLT_EPSILON)
                            {
                                framePixelWidthTarget += 0.3f;
                                framePixelWidth += 0.3f;
                                int newFrameOverCursor = (int)((io.MousePos.x - topRect.Min.x) / framePixelWidth) + firstFrameUsed;
                                *firstFrame += frameOverCursor - newFrameOverCursor;
                            }
                        }
                    }
                }
            }
        }

        ImGui::EndGroup();

        if (expanded)
        {
            if (sequenceOptions & EDITOR_COLLAPSE)
            {
                bool overExpanded = SequencerAddDelButton(draw_list, ImVec2(canvas_pos.x + 2, canvas_pos.y + 2), ImVec2(4, ItemHeight), !*expanded);
                if (overExpanded && io.MouseReleased[0] && !popupOpened)
                    *expanded = !*expanded;
            }
        }

        if (!ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel))
            popupOpened = false;

        if (framePixelWidth > 1.f && framePixelWidth < 50.f)
            *zoomLevel = framePixelWidth;

        return ret;
    }
}