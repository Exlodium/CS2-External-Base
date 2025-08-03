#include "../../Precompiled.h"

void CTriggerBot::UpdateValues( )
{
	this->m_iTriggerDelay = Config::Get<int>(g_Variables.m_iTriggerBotDelay);
}

void CTriggerBot::Run( )
{
	if (!Input::HandleInput( Config::Get<KeyBind_t>(g_Variables.m_TriggerBotKey) ))
	{
		this->m_TriggerTimer.Reset( );
		return;
	}

	this->UpdateValues( );

	if ( this->m_TriggerTimer.Elapsed( ) < this->m_iTriggerDelay )
		return;
	
	// do a trigger bot here
}