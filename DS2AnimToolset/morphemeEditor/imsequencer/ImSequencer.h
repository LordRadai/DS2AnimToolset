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
#pragma once

#include <math.h>
#include <cstddef>
#include <cstdlib>
#include "../imgui/imgui_internal.h"
#include "../extern.h"
#include "../EventTrackEditor/EventTrackEditor.h"
#include "../TimeActEditor/TimeActEditor.h"

struct ImDrawList;
struct ImRect;
class Application;

namespace ImSequencer
{
    enum SEQUENCER_OPTIONS
    {
        EDITOR_EDIT_NONE = 0,
        EDITOR_EVENT_EDIT_STARTEND = 1 << 1,
        EDITOR_CHANGE_FRAME = 1 << 3,
        EDITOR_TRACK_ADD = 1 << 4,
        EDITOR_EVENT_ADD = 1 << 5,
        EDITOR_COPYPASTE = 1 << 6,
        EDITOR_EVENT_LOOP = 1 << 7,
        EDITOR_COLLAPSE = 1 << 8,
        EDITOR_MARK_ACTIVE_EVENTS = 1 << 9,
        EDITOR_TRACK_RENAME = 1 << 10,
        EDITOR_EDIT_ALL = EDITOR_EVENT_EDIT_STARTEND | EDITOR_CHANGE_FRAME
    };

    // return true if selection is made
    bool Sequencer(EventTrackEditor* eventTrackEditor, int* currentFrame, int* selectedTrack, int* selectedEvent, bool* expanded, bool focused, int* firstFrame, float* zoomLevel, int sequenceOptions);
    bool Sequencer(TimeActEditor* timeActEditor, int* currentFrame, int* selectedTrack, int* selectedEvent, bool* expanded, bool focused, int* firstFrame, float* zoomLevel, int sequenceOptions);
}
