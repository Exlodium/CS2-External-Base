#pragma once

enum EKeyInputType : int
{
	HOLD,
	TOGGLE,
	ALWAYS_ON
};

namespace Input
{
	inline bool HandleInput( KeyBind_t& pKeyBind )
	{
		if (pKeyBind.m_iKey == 0)
			return false;

		if (pKeyBind.m_iMode == HOLD)
		{
			return GetAsyncKeyState( pKeyBind.m_iKey );
		}
		else if (pKeyBind.m_iMode == TOGGLE)
		{
			return GetKeyState( pKeyBind.m_iKey );
		}
		else if (pKeyBind.m_iMode == ALWAYS_ON)
		{
			return true;
		}

		return false;
	}
}