#pragma once

#define CLIENT_DLL X("client.dll")
#define ENGINE2_DLL X("engine2.dll")
#define NAVSYSTEM_DLL X("navsystem.dll")
#define SCHEMASYSTEM_DLL X( "schemasystem.dll" )

#define TICK_INTERVAL (Interfaces::m_GlobalVariables.m_flIntervalPerTick)
#define TIME_TO_TICKS(TIME) (static_cast<int>(0.5f + static_cast<float>(TIME) / TICK_INTERVAL))
#define TICKS_TO_TIME(TICKS) (TICK_INTERVAL * static_cast<float>(TICKS))
#define ROUND_TO_TICKS(TIME) (TICK_INTERVAL * TIME_TO_TICKS(TIME))
#define TICK_NEVER_THINK (-1)

#ifndef CS_DEBUG_BREAK
#define CS_DEBUG_BREAK() __debugbreak()
#endif

#ifndef CS_ASSERT
#ifdef _DEBUG
#define CS_ASSERT(EXPRESSION) static_cast<void>(!!(EXPRESSION) || (CS_DEBUG_BREAK(), 0))
#else
// disable assertion for release builds
#define CS_ASSERT(EXPRESSION) static_cast<void>(0)
#endif
#endif