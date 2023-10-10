#pragma once
// include EVERYTHING here
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500

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

// directx
#include <d3d11.h>
#include <D3D11.h>
#include <d3dx9math.h>

// imgui
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_freetype.h>

// crypt
#include "memory/crypt/XorStr.h"

// logging
#include "utilities/Logging.h"

// memory
#include "memory/Memory.h"
#include "memory/datatypes/Matrix.h"
#include "memory/datatypes/Vector.h"
#include "memory/datatypes/QAngle.h"
#include "memory/datatypes/Color.h"
#include "memory/datatypes/CUtlVector.h"

// globals
#include "Globals.h"
#include "config/Variables.h"
#include "utilities/Math.h"

// valve
#include "valve/Offsets.h"
#include "valve/Entity.h"

// drawing
#include "utilities/Draw.h"

// input
#include "utilities/InputSytem.h"

// features
#include "features/visuals/PlayerESP.h"
#include "features/aimbot/Aimbot.h"
#include "features/aimbot/Triggerbot.h"

// gui
#include "gui/Gui.h"
#include "gui/Window.h"