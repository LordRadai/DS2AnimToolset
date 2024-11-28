#include "imgui_custom_widget.h"
#include <stdio.h>
#include <string>
#include <Windows.h>
#include <shobjidl.h>
#include "IconsFontAwesome6.h"

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
    ImGui::SetNextItemWidth(50);
    ImGui::DragFloat("##dragBox", v, dragSpeed, min, max, format, flags);

    ImGui::SameLine();

    ImGui::SliderFloat(label, v, min, max, "");
}

void ImGui::InputDragInt(const char* label, int* v, float dragSpeed, int min, int max, const char* format, ImGuiInputFlags flags)
{
    ImGui::SetNextItemWidth(50);
    ImGui::DragInt("##dragBox", v, dragSpeed, min, max, format, flags);

    ImGui::SameLine();

    ImGui::SliderInt(label, v, min, max, "");
}

std::wstring browseForFolder()
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileDialog* pFileDialog = nullptr;

        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog));
        if (SUCCEEDED(hr))
        {
            DWORD dwOptions;
            if (SUCCEEDED(pFileDialog->GetOptions(&dwOptions)))
                pFileDialog->SetOptions(dwOptions | FOS_PICKFOLDERS);

            hr = pFileDialog->Show(NULL);
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem = nullptr;
                hr = pFileDialog->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFolderPath = nullptr;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);
                    if (SUCCEEDED(hr))
                    {
                        return pszFolderPath;

                        CoTaskMemFree(pszFolderPath);
                    }
                    pItem->Release();
                }
            }
            pFileDialog->Release();
        }
        CoUninitialize();
    }

    return L"";
}

std::wstring browseForFile()
{
    COMDLG_FILTERSPEC ComDlgFS[] = { {L"All Files",L"*.*"} };

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);

    if (SUCCEEDED(hr))
    {
        IFileOpenDialog* pFileOpen = NULL;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            pFileOpen->SetFileTypes(1, ComDlgFS);

            // Show the Open dialog box.
            hr = pFileOpen->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);

                if (SUCCEEDED(hr))
                {
                    PWSTR pszOutFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszOutFilePath);

                    if (SUCCEEDED(hr))
                        return pszOutFilePath;

                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }

    return L"";
}

void ImGui::PathSelection(const char* label, char* v, ImGuiInputFlags flags)
{
    ImGui::Text(label);

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 25);

    char textBoxId[256];
    sprintf_s(textBoxId, "##%s_textBox", label);

    ImGui::InputText(textBoxId, v, 256, flags);

    ImGui::SameLine();

    char buttonLabel[256];
    sprintf_s(buttonLabel, "%s##_%s", ICON_FA_FOLDER, label);

    if (ImGui::Button(buttonLabel))
    {
        std::wstring selectedFolder = browseForFolder();

        if (selectedFolder != L"")
            sprintf(v, "%ws", selectedFolder.c_str());
    }
}

void ImGui::FileSelection(const char* label, char* v, ImGuiInputFlags flags)
{
    ImGui::Text(label);

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 25);

    char textBoxId[256];
    sprintf_s(textBoxId, "##%s_textBox", label);

    ImGui::InputText(textBoxId, v, 256, flags);

    ImGui::SameLine();

    char buttonLabel[256];
    sprintf_s(buttonLabel, "%s##_%s", ICON_FA_FOLDER, label);

    if (ImGui::Button(buttonLabel))
    {
        std::wstring selectedFile = browseForFile();

        if (selectedFile != L"")
            sprintf(v, "%ws", selectedFile.c_str());
    }
}