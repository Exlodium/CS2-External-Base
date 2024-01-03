#pragma once

namespace Variables
{
	inline int m_iUnloadKey = VK_DELETE;
	inline int m_iMenuKey = VK_INSERT;

	namespace Visuals
	{
		inline bool m_bEnabled = false;
		inline bool m_bIgnoreTeammates = true;
		inline bool m_bOnlyWhenSpotted = false;

		inline bool m_bBox = false;
		inline Color m_colBox = Color( 255, 255, 255, 255 );
		inline Color m_colOutline = Color( 0, 0, 0, 255 );
		
		inline bool m_bHealthBar = false;
		
		inline bool m_bArmorBar = false;
		inline Color m_colArmorBar = Color( 0, 120, 255, 255 );
		
		inline bool m_bName = false;
		inline bool m_bDistance = false;

		inline bool m_bSnapLines = false;
		inline Color m_colSnapLines = Color( 255, 255, 255, 255 );

		inline bool m_arrFlags[ 5 ] = { false, false, false, false, false };
	}

	namespace Aimbot
	{
		inline bool m_bEnabled = false;
		
		inline KeyBind_t m_iAimbotKey = { VK_XBUTTON2, 0 };
		inline float m_flAimbotFOV = 0.0f;
		inline float m_flAimbotSmooth = 50.0f;
	}

	namespace TriggerBot
	{
		inline bool m_bEnabled = false;

		inline KeyBind_t m_iTriggerBotkey = { VK_XBUTTON2, 0 };
		inline int m_iTriggerBotDelay = 200;
	}
}