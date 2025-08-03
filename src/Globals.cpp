#include "Includes.h"

//// Decided to push in RIP(PatternScan) into the function itself to avoid having to add more args for different instructions and RIP offsets..
//FindOffsetFromSignature(offsets::dwEntityList, memory::ResolveRelativeAddress(memory::PatternScan(hClientDLL, "48 8B 0D ?? ?? ?? ?? 8B F3"), 0x3, 0x7), reinterpret_cast<std::uintptr_t>(hClientDLL));
//FindOffsetFromSignature(offsets::dwLocalPlayerController, memory::ResolveRelativeAddress(memory::PatternScan(hClientDLL, "48 8B 05 ?? ?? ?? ?? 48 85 C0 74 53"), 0x3, 0x7), reinterpret_cast<std::uintptr_t>(hClientDLL));
//FindOffsetFromSignature(offsets::dwGlobalVars, memory::ResolveRelativeAddress(memory::PatternScan(hClientDLL, "48 89 15 ?? ?? ?? ?? 48 89 42" /*"48 8B 05 ?? ?? ?? ?? 44 8B B7 ?? ?? ?? ?? 8B 70 04 B8 ?? ?? ?? ??"*/ /*"48 89 0D ?? ?? ?? ?? 48 89 41"*/), 0x3, 0x7), reinterpret_cast<std::uintptr_t>(hClientDLL));
////48 8B 05 ?? ?? ?? ?? 44 8B B7 ?? ?? ?? ?? 8B 70 04 B8 ?? ?? ?? ?? fl0 says its this...		
//FindOffsetFromSignature(offsets::dwViewMatrix, memory::ResolveRelativeAddress(memory::PatternScan(hClientDLL, "48 8D 0D ?? ?? ?? ?? 48 C1 E0 06"), 0x3, 0x7), reinterpret_cast<std::uintptr_t>(hClientDLL));
//FindOffsetFromSignature(offsets::dwViewAngles, memory::ResolveRelativeAddress(memory::PatternScan(hClientDLL, "48 8B 0D ?? ?? ?? ?? 4C 8B C6 8B 10 E8"), 0x3, 0x7, 0x3D0 /*0x5418*/), reinterpret_cast<std::uintptr_t>(hClientDLL));
////FindOffsetFromSignature(offsets::dwCSGOInput, memory::ResolveRelativeAddress(memory::PatternScan(hClientDLL, "48 8B 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B BE 84 12 00 00"), 0x3, 0x7), reinterpret_cast<std::uintptr_t>(hClientDLL));	//"48 8B 0D ?? ?? ?? ?? E9 ?? ?? ?? ?? CC CC CC CC 40 55"
//FindOffsetFromSignature(offsets::dwGameRules, memory::ResolveRelativeAddress(memory::PatternScan(hClientDLL, "48 8B 0D ? ? ? ? 48 85 C9 74 ? 48 8B 01 48 8D 54 24 ? 45 33 C0"), 0x3, 0x7), reinterpret_cast<std::uintptr_t>(hClientDLL));	//"48 8B 0D ?? ?? ?? ?? E9 ?? ?? ?? ?? CC CC CC CC 40 55"
//FindOffsetFromSignature(offsets::dwPlantedC4, memory::ResolveRelativeAddress(memory::PatternScan(hClientDLL, "4C 8B 0D ?? ?? ?? ?? 8B C8 4A 39 3C 09"), 0x3, 0x7), reinterpret_cast<std::uintptr_t>(hClientDLL));
////FindOffsetFromSignature(offsets::dwSensitivity, memory::ResolveRelativeAddress(memory::PatternScan(hClientDLL, "48 8D 0D ?? ?? ?? ?? 85 ED BA ?? ?? ?? ?? 0F 44 E8"), 0x3, 0x7), reinterpret_cast<std::uintptr_t>(hClientDLL));
////offsets::dwSensitivity += 0x5A20;
//FindOffsetFromSignature(offsets::dwSensitivity, memory::ResolveRelativeAddress(memory::PatternScan(hClientDLL, "48 8D 0D ?? ?? ?? ?? 44 0F 28 C1 0F 28 F3 0F 28 FA E8 ?? ?? ?? ??"), 0x3, 0x7), reinterpret_cast<std::uintptr_t>(hClientDLL));
//offsets::dwSensitivity += 0x8;

bool CGlobals::UpdateGlobals()
{
	std::uintptr_t uClientAddress = g_Memory.GetModule(CLIENT_DLL).m_uBaseAddress;
	g_Globals.m_uEntityList = g_Memory.ReadMemory<std::uintptr_t>(uClientAddress + 0x1A1F730);

	return true;
}