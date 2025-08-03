#pragma once

class CTriggerBot
{
public:
	void Run( );
private:
	void UpdateValues( );

	int m_iTriggerDelay = 100;
	CTimer m_TriggerTimer;
};

inline CTriggerBot g_TriggerBot;