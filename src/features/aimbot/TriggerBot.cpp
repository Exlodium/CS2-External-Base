#include "../../Precompiled.h"

void CTriggerBot::UpdateValues( )
{
	this->m_iTriggerDelay = Variables::TriggerBot::m_iTriggerBotDelay;
}

void CTriggerBot::Run( )
{
	if (!Input::HandleInput( Variables::TriggerBot::m_iTriggerBotkey ))
	{
		this->m_TriggerTimer.Reset( );
		return;
	}

	this->UpdateValues( );

	if ( this->m_TriggerTimer.Elapsed( ) < this->m_iTriggerDelay )
		return;
	
	// do a trigger bot here
}