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
	IMGUI_API bool			InputDragFloat(const char* label, float* v, float min = -1E5, float max = 1E5, const char* format = "%.3f", ImGuiInputFlags flags = 0);
	IMGUI_API bool			InputDragVector3(const char* label, float v[3], float min = -1E5, float max = 1E5, const char* format = "%.3f", ImGuiInputFlags flags = 0);
	IMGUI_API bool			InputDragVector4(const char* label, float v[4], float min = -1E5, float max = 1E5, const char* format = "%.3f", ImGuiInputFlags flags = 0);
	IMGUI_API bool			InputDragInt(const char* label, int* v, int min = -1E5, int max = 1E5, const char* format = "%d", ImGuiInputFlags flags = 0);
	IMGUI_API bool			InputDragUInt(const char* label, unsigned int* v, unsigned int min = 0, unsigned int max = 1E5, const char* format = "%u", ImGuiInputFlags flags = 0);
	IMGUI_API bool			Label(const char* labelText, ImGuiInputTextFlags flags = 0);
	IMGUI_API bool			NamedLabel(const char* label, const char* labelText, ImGuiInputTextFlags flags = 0);
	IMGUI_API bool			ColorEditUInt(const char* label, ImU32* color, ImGuiColorEditFlags flags = 0);
	IMGUI_API bool          RightAlignedCheckbox(const char* label, bool* v);
	IMGUI_API bool          RightAlignedInputFloat(const char* label, float* v, const char* format = "%.3f", ImGuiInputFlags flags = 0);
	IMGUI_API bool          RightAlignedInputInt(const char* label, int* v, ImGuiInputFlags flags = 0);
	IMGUI_API bool			RightAlignedCombo(const char* label, int* current_item, const char* const items[], int items_count, ImGuiComboFlags flags = 0);
}