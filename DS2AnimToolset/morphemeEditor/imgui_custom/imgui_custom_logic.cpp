#include "imgui_custom_logic.h"

float ImGui::GetWindowContentWidth()
{
    return (ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
}

float ImGui::GetWindowContentHeight()
{
    return (ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);
}