// header.h: file di inclusione per file di inclusione di sistema standard
// o file di inclusione specifici del progetto
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Escludere gli elementi usati raramente dalle intestazioni di Windows
// File di intestazione di Windows
#include <windows.h>
// File di intestazione Runtime C
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>
#include <filesystem>
#include <string>
#include <shtypes.h>
#include <ShObjIdl_core.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>
#include "imgui_custom/imgui_custom_logic.h"
#include "imgui_custom/imgui_custom_widget.h"

//utils
#include "utils/NMDX/NMDX.h"

#include "RCore.h"