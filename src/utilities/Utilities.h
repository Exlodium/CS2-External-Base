#pragma once

class CUtilities
{
public:
	void Sleep(float flMilliseconds);
	bool IsVisible(C_CSPlayerPawn* pPlayer, Vector vecStart, Vector vecPosition);
	bool IsChangingLevel();
	bool IsInGame();
	bool IsConnected();
};

inline CUtilities g_Utilities;