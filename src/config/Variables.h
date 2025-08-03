#pragma once

class CVariables
{
public:
	struct GuiVariables_t
	{
		C_ADD_VARIABLE(int, m_iMenuKey, VK_INSERT);
		C_ADD_VARIABLE(int, m_iUnloadKey, VK_DELETE);

	}; GuiVariables_t m_Gui;
};

inline CVariables g_Variables;