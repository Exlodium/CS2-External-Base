#pragma once

enum EPaddingDirection : unsigned int
{
	DIR_LEFT = 0,
	DIR_TOP,
	DIR_RIGHT,
	DIR_BOTTOM,
	DIR_MAX
};

struct FlagObjects_t
{
	FlagObjects_t( bool bHelmet, bool bHeavy, bool bDefuser, bool bScoping, bool bDefusing, bool bFlashed, int iMoney )
	{
		this->m_bHasHelmet = bHelmet;
		this->m_bHasHeavyArmor = bHeavy;
		this->m_bHasDefuser = bDefuser;
		this->m_bIsScoping = bScoping;
		this->m_bIsDefusing = bDefusing;
		this->m_bFlashed = bFlashed;

		this->m_iMoney = iMoney;
	}

	bool m_bHasHelmet = false;
	bool m_bHasHeavyArmor = false;
	bool m_bHasDefuser = false;
	bool m_bIsScoping = false;
	bool m_bIsDefusing = false;
	bool m_bFlashed = false;

	int m_iMoney = 0;
};


class CPlayerESP
{
public:
	void Run( CCSPlayerController* pEntity, CCSPlayerPawn* pPawn, int nIndex );
private:
	// functions
	void DrawBox( CCSPlayerController* pEntity, ImVec2 vecMin, ImVec2 vecMax, Color colColor, Color colOutline );
	void DrawHealthBar( CCSPlayerPawn* pPawn, ImVec2 vecMin, ImVec2 vecMax, Color colColor, Color colOutline );
	void DrawArmorBar( CCSPlayerPawn* pPawn, ImVec2 vecMin, ImVec2 vecMax, Color colColor, Color colOutline );
	void DrawName( CCSPlayerController* pEntity, ImVec2 vecPosition, Color colColor, Color colOutline );
	void DrawDistance( CCSPlayerPawn* pPawn, ImVec2 vecPosition, Color colColor, Color colOutline );
	void DrawSnapLine( ImVec2 vecPosition, Color colColor );
	void DrawFlags( CCSPlayerController* pEntity, CCSPlayerPawn* pPawn, ImVec2 vecPosition, Color colColor, Color colOutline );

	std::array<float, DIR_MAX> m_arrPadding = { 0, 0, 0, 0 };
	
	float m_flSideBarInfoHeightLeft = 0.0f;
	bool m_bFlashed = false;
	bool m_bArmored = false;
	bool m_bSpotted = false;
};

inline CPlayerESP g_PlayerESP;