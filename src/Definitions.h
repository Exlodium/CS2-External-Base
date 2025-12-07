#pragma once

// game modules
#define CLIENT_DLL X("client.dll")
#define ENGINE2_DLL X("engine2.dll")
#define NAVSYSTEM_DLL X("navsystem.dll")
#define SCHEMASYSTEM_DLL X("schemasystem.dll")
// other modules
#define NTDLL_DLL X("ntdll.dll")

constexpr float INTERVAL_PER_TICK = 0.015625f;
#define TIME_TO_TICKS(TIME) (static_cast<int>(0.5f + static_cast<float>(TIME) / INTERVAL_PER_TICK))
#define TICKS_TO_TIME(TICKS) (INTERVAL_PER_TICK * static_cast<float>(TICKS))
#define ROUND_TO_TICKS(TIME) (INTERVAL_PER_TICK * TIME_TO_TICKS(TIME))
#define TICK_NEVER_THINK constexpr (-1)