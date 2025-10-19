#pragma once

enum EKeyInputType : int
{
	HOLD,
	TOGGLE,
	ALWAYS_ON
};

class CInputHandler
{
public:
	bool HandleInput(KeyBind_t& pKeyBind)
	{
		if (pKeyBind.m_uKey == 0U)
			return false;

		if (pKeyBind.m_uMode == HOLD)
		{
			return GetAsyncKeyState(pKeyBind.m_uKey);
		}
		else if (pKeyBind.m_uMode == TOGGLE)
		{
			return GetKeyState(pKeyBind.m_uKey);
		}
		else if (pKeyBind.m_uMode == ALWAYS_ON)
		{
			return true;
		}

		return false;
	}
};

inline CInputHandler g_InputHandler;