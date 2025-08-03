#pragma once

enum EVisualsInfoFlags : unsigned int
{
	FLAG_MONEY = 0,
	FLAG_ARMOR,
	FLAG_KIT,
	FLAG_ZOOM,
	FLAG_DEFUSING,
	INFO_FLAG_MAX
};

class CVariables
{
public:
	// Visuals start
	C_ADD_VARIABLE(int, m_iMenuKey, VK_INSERT);
	C_ADD_VARIABLE(int, m_iUnloadKey, VK_DELETE);

	C_ADD_VARIABLE(bool, m_bEnableVisuals, false);
	C_ADD_VARIABLE(bool, m_bVisualsIgnoreTeammates, true);
	C_ADD_VARIABLE(bool, m_bVisualsOnlyWhenSpotted, false);

	C_ADD_VARIABLE(bool, m_bBox, false);
	C_ADD_VARIABLE(Color, m_colBox, Color(255, 255, 255, 255));
	C_ADD_VARIABLE(Color, m_colOutline, Color(0, 0, 0, 255));

	C_ADD_VARIABLE(bool, m_bHealthBar, false);

	C_ADD_VARIABLE(bool, m_bArmorBar, false);
	C_ADD_VARIABLE(Color, m_colArmorBar, Color(0, 120, 255, 255));

	C_ADD_VARIABLE(bool, m_bName, false);
	C_ADD_VARIABLE(bool, m_bDistance, false);

	C_ADD_VARIABLE(bool, m_bSnapLines, false);
	C_ADD_VARIABLE(Color, m_colSnapLines, Color(255, 255, 255, 255));

	C_ADD_VARIABLE_VECTOR(bool, INFO_FLAG_MAX, m_vecFlags, false)
		// Visuals end

		// Aimbot start
		C_ADD_VARIABLE(bool, m_bEnableAimBot, false);
	C_ADD_VARIABLE(KeyBind_t, m_AimBotKey, KeyBind_t(VK_XBUTTON2, 0));

	C_ADD_VARIABLE(float, m_flAimbotFOV, 0.0f);
	C_ADD_VARIABLE(float, m_flAimbotSmooth, 50.0f);
	// Aimbot end

	// Triggerbot start
	C_ADD_VARIABLE(bool, m_bEnableTriggerbot, false);
	C_ADD_VARIABLE(KeyBind_t, m_TriggerBotKey, KeyBind_t(VK_XBUTTON2, 0));
	C_ADD_VARIABLE(int, m_iTriggerBotDelay, 200);
	// Triggerbot end
};

inline CVariables g_Variables;