#pragma once

enum EPaddingDirection : unsigned int
{
	DIR_LEFT = 0,
	DIR_TOP,
	DIR_RIGHT,
	DIR_BOTTOM,
	DIR_MAX
};

class CPlayerESP
{
public:
	void Run( CCSPlayerController* pEntity, CCSPlayerPawn* pPawn, int nIndex );
private:
	// functions
	void DrawBox( CCSPlayerController* pEntity, int nIndex, ImVec2 vecMin, ImVec2 vecMax, Color colColor, Color colOutline );
	void DrawHealthBar( CCSPlayerPawn* pPawn, int nIndex, ImVec2 vecMin, ImVec2 vecMax, Color colColor, Color colOutline );
	void DrawArmorBar( CCSPlayerPawn* pPawn, int nIndex, ImVec2 vecMin, ImVec2 vecMax, Color colColor, Color colOutline );
	void DrawName( CCSPlayerController* pEntity, int nIndex, ImVec2 vecPosition, Color colColor, Color colOutline );
	void DrawDistance( CCSPlayerPawn* pPawn, int nIndex, ImVec2 vecPosition, Color colColor, Color colOutline );
	void DrawSnapLine( ImVec2 vecPosition, int nIndex, Color colColor );

	std::array<float, DIR_MAX> m_arrPadding = { 0, 0, 0, 0 };
	
	float m_flSideBarInfoHeightLeft = 0.0f;
	bool m_bFlashed = false;
	bool m_bArmored = false;
	bool m_bSpotted = false;
};

inline CPlayerESP g_PlayerESP;