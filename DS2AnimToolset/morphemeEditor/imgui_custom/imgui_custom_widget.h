#include "imgui/imgui.h"

namespace ImGui
{
	IMGUI_API bool          InputUInt(const char* label, ImU32* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
	IMGUI_API bool          InputInt64(const char* label, ImS64* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
	IMGUI_API bool          InputUInt64(const char* label, ImU64* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
	IMGUI_API bool          InputByte(const char* label, ImS8* v, char step = 1, char step_fast = 100, ImGuiInputTextFlags flags = 0);
	IMGUI_API bool          InputUByte(const char* label, ImU8* v, char step = 1, char step_fast = 100, ImGuiInputTextFlags flags = 0);
	IMGUI_API bool          InputShort(const char* label, ImS16* v, short step = 1, short step_fast = 100, ImGuiInputTextFlags flags = 0);
	IMGUI_API bool          InputUShort(const char* label, ImU16* v, short step = 1, short step_fast = 100, ImGuiInputTextFlags flags = 0);
	IMGUI_API bool          InputPtr(const char* label, unsigned long long int* v, ImGuiInputTextFlags flags = 0);
	IMGUI_API void			CompositeProgressBar(const char* label, int step, int numSteps, const char* stepName = nullptr);
}