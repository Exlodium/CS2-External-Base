#pragma once

enum EVisualsModifiers : std::uint8_t
{
	VISUALS_IGNORE_TEAMMATES = 0U,
	VISUALS_ONLY_WHEN_VISIBLE,
	VISUALS_MAX
};

class CVariables
{
public:
	struct GuiVariables_t
	{
		CONFIG_ADD_VARIABLE(int, m_iMenuKey, VK_INSERT);
		CONFIG_ADD_VARIABLE(int, m_iUnloadKey, VK_DELETE);
		
		CONFIG_ADD_VARIABLE(bool, m_bExcludeFromDesktopCapture, false);
	}; GuiVariables_t m_Gui;

	struct AimBotVariables_t
	{
		CONFIG_ADD_VARIABLE(bool, m_bEnableAimbot, false);
		
	}; AimBotVariables_t m_AimBot;
	
	struct TriggerBotVariables_t
	{
		CONFIG_ADD_VARIABLE(bool, m_bEnableTriggerbot, false);

	}; TriggerBotVariables_t m_TriggerBot;

	struct PlayerVisualsVariables_t
	{
		CONFIG_ADD_VARIABLE(bool, m_bEnableVisuals, false);
		CONFIG_ADD_VARIABLE_VECTOR(bool, EVisualsModifiers::VISUALS_MAX, m_vecVisualsModifiers, false);
		
	}; PlayerVisualsVariables_t m_PlayerVisuals;

};
inline CVariables g_Variables;