#pragma once
// include EVERYTHING here
// standard includes
#include <Windows.h>
#include <dwmapi.h>
#include <TlHelp32.h>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <xstring>
#include <string>
#include <chrono>
#include <thread>
#include <cstdint>
#include <string_view>
#include <vector>
#include <deque>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <any>
#include <array>
#include <format>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <cmath>
#include <limits>
#include <map>
#include <filesystem>
#include <shlobj_core.h>
#include <KnownFolders.h>
#include <Psapi.h>

// directx
#include <d3d11.h>
#include <D3D11.h>

// imgui
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_freetype.h>
#include <cpp/imgui_stdlib.h>

// json
#include <json.hpp>

// stb
#include <stb_image.h>
#include <stb_sprintf.h>

// crypt
#include "memory/crypt/XorStr.h"
#include "memory/crypt/FNV1A.h"

// logging
#include "utilities/Logging.h"

// enums and definitions
#include "Enums.h"
#include "Definitions.h"

// CRT
#include "utilities/CRT.h"

// memory
#include "memory/HandleHijack.h"
#include "memory/Memory.h"
#include "memory/datatypes/Vector.h"
#include "memory/datatypes/Matrix.h"
#include "memory/datatypes/Quaternion.h"
#include "memory/datatypes/QAngle.h"
#include "memory/datatypes/Color.h"
#include "memory/datatypes/CUtlVector.h"
#include "memory/datatypes/CUtlHash.h"
#include "memory/datatypes/CBaseHandle.h"

// configs
#include "config/Config.h"
#include "config/Variables.h"

// globals
#include "Globals.h"

// utilities
#include "utilities/Math.h"

// valve
#include "valve/Schema.h"
#include "valve/Offsets.h"
#include "valve/Entity.h"

// drawing
#include "utilities/Draw.h"

// input
#include "utilities/InputSytem.h"

// entity list
#include "features/entities/EntityList.h"

// features
#include "features/visuals/PlayerESP.h"
#include "features/aimbot/Aimbot.h"
#include "features/aimbot/Triggerbot.h"

// gui
#include "gui/Gui.h"
#include "gui/Window.h"

#define WIN32_LEAN_AND_MEAN
