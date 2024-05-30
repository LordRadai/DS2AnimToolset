#pragma once
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN 

//System
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>
#include <commdlg.h>
#include <shobjidl.h> 
#include <iostream>
#include <fstream>

//DirectXTK
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <DirectXColors.h>

#include <nlohmann/json.hpp>
#include <fbxsdk.h>

//ImGui
#include "imgui/imgui.h"
#include "imgui/imconfig.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"
#include "imgui/imstb_rectpack.h"
#include "imgui/imstb_textedit.h"
#include "imgui/imstb_truetype.h"
#include "imgui/custom/imgui_custom_logic.h"
#include "imgui/custom/imgui_custom_widget.h"

//IniReader
#include "inih/ini.h"
#include "inih/INIReader.h"

#include "RLog/RLog.h"
#include "RMath/RMath.h"
#include "RString/RString.h"