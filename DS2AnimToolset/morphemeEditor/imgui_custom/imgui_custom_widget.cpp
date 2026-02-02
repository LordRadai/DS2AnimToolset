#include "imgui_custom_widget.h"
#include <stdio.h>

bool ImGui::InputInt64(const char* label, ImS64* v, int step, int step_fast, ImGuiInputTextFlags flags)
{
    // Hexadecimal input provided as a convenience but the flag name is awkward. Typically you'd use InputText() to parse your own data, if you want to handle prefixes.
    const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%d";
    return InputScalar(label, ImGuiDataType_S64, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
}

bool ImGui::InputUInt64(const char* label, ImU64* v, int step, int step_fast, ImGuiInputTextFlags flags)
{
    // Hexadecimal input provided as a convenience but the flag name is awkward. Typically you'd use InputText() to parse your own data, if you want to handle prefixes.
    const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%d";
    return InputScalar(label, ImGuiDataType_U64, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
}

bool ImGui::InputUInt(const char* label, ImU32* v, int step, int step_fast, ImGuiInputTextFlags flags)
{
    // Hexadecimal input provided as a convenience but the flag name is awkward. Typically you'd use InputText() to parse your own data, if you want to handle prefixes.
    const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%d";
    return InputScalar(label, ImGuiDataType_U32, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
}

bool ImGui::InputByte(const char* label, ImS8* v, char step, char step_fast, ImGuiInputTextFlags flags)
{
    // Hexadecimal input provided as a convenience but the flag name is awkward. Typically you'd use InputText() to parse your own data, if you want to handle prefixes.
    const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%d";
    return InputScalar(label, ImGuiDataType_S8, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
}

bool ImGui::InputUByte(const char* label, ImU8* v, char step, char step_fast, ImGuiInputTextFlags flags)
{
    // Hexadecimal input provided as a convenience but the flag name is awkward. Typically you'd use InputText() to parse your own data, if you want to handle prefixes.
    const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%d";
    return InputScalar(label, ImGuiDataType_U8, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
}

bool ImGui::InputShort(const char* label, ImS16* v, short step, short step_fast, ImGuiInputTextFlags flags)
{
    // Hexadecimal input provided as a convenience but the flag name is awkward. Typically you'd use InputText() to parse your own data, if you want to handle prefixes.
    const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%d";
    return InputScalar(label, ImGuiDataType_S16, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
}

bool ImGui::InputUShort(const char* label, ImU16* v, short step, short step_fast, ImGuiInputTextFlags flags)
{
    // Hexadecimal input provided as a convenience but the flag name is awkward. Typically you'd use InputText() to parse your own data, if you want to handle prefixes.
    const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%d";
    return InputScalar(label, ImGuiDataType_U16, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
}

bool ImGui::InputPtr(const char* label, unsigned long long int* v, ImGuiInputTextFlags flags)
{
    const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%16llX" : "%llu";
    return InputScalar(label, ImGuiDataType_U64, v, NULL, NULL, format, flags);
}

void ImGui::CompositeProgressBar(const char* label, int step, int numSteps, const char* stepName)
{
    const float progress = float(step) / float(numSteps);

    if (step < numSteps)
        ImGui::Text("%s: %s", label, stepName);
    else
        ImGui::Text("%s: finished", label);

    char buf[256];
    sprintf_s(buf, "%d/%d", step, numSteps);

    ImGui::ProgressBar(progress, ImVec2(0, 0), buf);
}

void ImGui::InputDragFloat(const char* label, float* v, float dragSpeed, float min, float max, const char* format, ImGuiInputFlags flags)
{
    char dragID[256];
    sprintf(dragID, "##%sDragFloat", label);

    ImGui::SetNextItemWidth(50);
    ImGui::DragFloat(dragID, v, dragSpeed, min, max, format, flags);

    ImGui::SameLine();

    ImGui::SliderFloat(label, v, min, max, "");
}

void ImGui::InputDragInt(const char* label, int* v, float dragSpeed, int min, int max, const char* format, ImGuiInputFlags flags)
{
    char dragID[256];
    sprintf(dragID, "##%sDragInt", label);

    ImGui::SetNextItemWidth(50);
    ImGui::DragInt(dragID, v, dragSpeed, min, max, format, flags);

    ImGui::SameLine();

    ImGui::SliderInt(label, v, min, max, "");
}

bool ImGui::Label(const char* labelText, ImGuiInputTextFlags flags)
{
	char textID[256];
	sprintf_s(textID, "##%sLabel", labelText);

    char label[256];
	sprintf_s(label, "%s", labelText);

	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

	return ImGui::InputText(textID, label, 256, flags);
}

bool ImGui::NamedLabel(const char* label, const char* labelText, ImGuiInputTextFlags flags)
{
    char textID[256];
    sprintf_s(textID, "##%sLabel", label);
    char labelBuffer[256];
    sprintf_s(labelBuffer, "%s", labelText);

	ImGui::TextUnformatted(label);

	ImGui::SameLine();

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

    return Label(labelText, flags);
}

bool ImGui::ColorEditUInt(const char* label, ImU32* color, ImGuiColorEditFlags flags)
{
    if (!color)
        return false;

    ImVec4 col = ImGui::ColorConvertU32ToFloat4(*color);

    bool changed = ImGui::ColorEdit4(label, &col.x, flags);

    if (changed)
        *color = ImGui::ColorConvertFloat4ToU32(col);

    return changed;
}

bool ImGui::RightAlignedCheckbox(const char* label, bool* v)
{
	ImGui::TextUnformatted(label);

    ImGui::SameLine();

    return ImGui::Checkbox("", v);
}

bool ImGui::RightAlignedDragFloat(const char* label, float* v, float v_speed, float v_min, float v_max, const char* format, ImGuiInputFlags flags)
{
	ImGui::TextUnformatted(label);

	ImGui::SameLine();

    return ImGui::DragFloat("", v, v_speed, v_min, v_max, format, flags);
}

bool ImGui::RightAlignedDragInt(const char* label, int* v, float v_speed, int v_min, int v_max, const char* format, ImGuiInputFlags flags)
{
    ImGui::TextUnformatted(label);

    ImGui::SameLine();

    return ImGui::DragInt("", v, v_speed, v_min, v_max, format, flags);
}

bool ImGui::RightAlignedInputFloat(const char* label, float* v, const char* format, ImGuiInputFlags flags)
{
    ImGui::TextUnformatted(label);

    ImGui::SameLine();

    return ImGui::InputFloat("", v, 0.f, 0.f, format, flags);
}

bool ImGui::RightAlignedInputInt(const char* label, int* v, ImGuiInputFlags flags)
{
    ImGui::TextUnformatted(label);

    ImGui::SameLine();

    return ImGui::InputInt("", v, 0, 0, flags);
}