#include "../../Precompiled.h"

bool GetEntityBoundingBox(C_BaseEntity* pEntity, Box_t* pBox)
{
	CCollisionProperty* pCollision = pEntity->m_pCollision();
	if (pCollision == nullptr)
		return false;

	CGameSceneNode* pGameSceneNode = pEntity->m_pGameSceneNode();
	if (pGameSceneNode == nullptr)
		return false;

	CTransform nodeToWorldTransform = pGameSceneNode->m_nodeToWorld();
	const Matrix3x4_t matTransform = nodeToWorldTransform.m_quatOrientation.ToMatrix(nodeToWorldTransform.m_vecPosition);

	const Vector vecMins = pCollision->m_vecMins();
	const Vector vecMaxs = pCollision->m_vecMaxs();

	float flLeft = std::numeric_limits<float>::max();
	float flTop = std::numeric_limits<float>::max();
	float flRight = std::numeric_limits<float>::lowest();
	float flBottom = std::numeric_limits<float>::lowest();

	for (int i = 0; i < 8; ++i)
	{
		const Vector vecPoint{
			i & 1 ? vecMaxs.x : vecMins.x,
			i & 2 ? vecMaxs.y : vecMins.y,
			i & 4 ? vecMaxs.z : vecMins.z
		};
		ImVec2 vecScreen;
		if (!Draw::WorldToScreen(vecPoint.Transform(matTransform), vecScreen))
			return false;

		flLeft = std::min(flLeft, vecScreen.x);
		flTop = std::min(flTop, vecScreen.y);
		flRight = std::max(flRight, vecScreen.x);
		flBottom = std::max(flBottom, vecScreen.y);
	}

	pBox->m_flLeft = flLeft;
	pBox->m_flTop = flTop;
	pBox->m_flRight = flRight;
	pBox->m_flBottom = flBottom;
	pBox->m_flWidth = flRight - flLeft;
	pBox->m_flHeight = flBottom - flTop;
	return true;
}

void CPlayerESP::Run( CCSPlayerController* pEntity, C_CSPlayerPawn* pPawn, int nIndex )
{
	this->m_bSpotted = Config::Get<bool>(g_Variables.m_bVisualsOnlyWhenSpotted) ? pEntity->m_iTeamNum() != Globals::m_pLocalPlayerController->m_iTeamNum() && pPawn->m_entitySpottedState().m_bSpotted : true;
	if (Config::Get<bool>(g_Variables.m_bVisualsIgnoreTeammates) && pEntity->m_iTeamNum( ) == Globals::m_pLocalPlayerController->m_iTeamNum( ) || !this->m_bSpotted)
		return;

	m_Context = { };
	if (!GetEntityBoundingBox(pPawn, &m_Context.m_Box))
		return;

	this->m_bFlashed = pPawn->m_flFlashAlpha( ) > 0.0f;
	this->m_bArmored = pPawn->m_ArmorValue( ) > 0;
	this->m_Context.m_arrPadding = { 0.0f, 0.0f, 0.0f, 2.0f };
	this->m_flSideBarInfoHeightLeft = 0.0f;

	if (Config::Get<bool>(g_Variables.m_bBox))
		DrawBox( 
			pEntity, 
			ImVec2(this->m_Context.m_Box.m_flLeft, this->m_Context.m_Box.m_flTop),
			ImVec2(this->m_Context.m_Box.m_flRight, this->m_Context.m_Box.m_flBottom),
			Config::Get<Color>(g_Variables.m_colBox),
			Config::Get<Color>(g_Variables.m_colOutline)
		);

	if (Config::Get<bool>(g_Variables.m_bHealthBar))
		DrawHealthBar(
			pPawn,
			ImVec2(this->m_Context.m_Box.m_flLeft - 6.0f, this->m_Context.m_Box.m_flTop - 1.0f ),
			ImVec2(this->m_Context.m_Box.m_flLeft - 2.0f, this->m_Context.m_Box.m_flBottom + 1.0f ),
			Color( 255, 255, 255, 255 ),
			Config::Get<Color>(g_Variables.m_colOutline)
		);
		
	if (Config::Get<bool>(g_Variables.m_bArmorBar))
		DrawArmorBar(
			pPawn,
			ImVec2(this->m_Context.m_Box.m_flLeft - ( this->m_Context.m_arrPadding.at( DIR_LEFT ) + 4.0f ), this->m_Context.m_Box.m_flTop - 1.0f ),
			ImVec2(this->m_Context.m_Box.m_flLeft - this->m_Context.m_arrPadding.at( DIR_LEFT ), this->m_Context.m_Box.m_flBottom + 1.0f ),
			Config::Get<Color>(g_Variables.m_colArmorBar),
			Config::Get<Color>(g_Variables.m_colOutline)
		);
			
	if (Config::Get<bool>(g_Variables.m_bName))
		DrawName( 
			pEntity, 
			ImVec2(this->m_Context.m_Box.m_flLeft + (this->m_Context.m_Box.m_flWidth * 0.5f), this->m_Context.m_Box.m_flTop - 2.0f),
			Color( 255, 255, 255, 255 ),
			Config::Get<Color>(g_Variables.m_colOutline)
		);

	if (Config::Get<bool>(g_Variables.m_bDistance))
		DrawDistance(
			pPawn, 
			ImVec2(this->m_Context.m_Box.m_flLeft + (this->m_Context.m_Box.m_flWidth * 0.5f), this->m_Context.m_Box.m_flBottom + this->m_Context.m_arrPadding.at( DIR_BOTTOM ) ),
			Color( 255, 255, 255, 255 ),
			Config::Get<Color>(g_Variables.m_colOutline)
		);

	if (Config::Get<bool>(g_Variables.m_bSnapLines))
		DrawSnapLine( 
			ImVec2(this->m_Context.m_Box.m_flLeft + (this->m_Context.m_Box.m_flWidth * 0.5f), this->m_Context.m_Box.m_flBottom ),
			Config::Get<Color>(g_Variables.m_colSnapLines)
		);

	DrawFlags(
		pEntity,
		pPawn,
		ImVec2(this->m_Context.m_Box.m_flRight + 2.0f, this->m_Context.m_Box.m_flTop),
		Color( 255, 255, 255, 255 ),
		Config::Get<Color>(g_Variables.m_colOutline)
	);
}

void CPlayerESP::DrawBox( CCSPlayerController* pEntity, ImVec2 vecMin, ImVec2 vecMax, Color colColor, Color colOutline )
{
	Draw::AddRect( vecMin, vecMax, colColor, DRAW_RECT_OUTLINE | DRAW_RECT_BORDER | DRAW_RECT_ALIGNED, colOutline );
}

void CPlayerESP::DrawHealthBar( C_CSPlayerPawn* pPawn, ImVec2 vecMin, ImVec2 vecMax, Color colColor, Color colOutline )
{
	const int iHealth = pPawn->m_iHealth( );
	const float flFactor = static_cast< float >( iHealth ) * 0.01f;
	const float flHue = ( flFactor * 120.f ) / 360.f;
	const float flHeight = vecMax.y - vecMin.y;

	Draw::AddRect( vecMin, vecMax, Color( 40, 40, 40, 100 ), DRAW_RECT_FILLED | DRAW_RECT_ALIGNED, colOutline );
	Draw::AddRect( ImVec2( vecMin.x + 1.0f, ( vecMax.y + 1.0f ) - flHeight * flFactor ), ImVec2( vecMax.x - 1.0f, vecMax.y - 1.0f ), Color::FromHSB( flHue, 1.0f, 1.0f ), DRAW_RECT_FILLED | DRAW_RECT_ALIGNED, colOutline );

	const std::string strHealth = std::to_string( iHealth ).append( X( "HP" ) );
	if (iHealth < 100)
	{
		Draw::AddText( Fonts::ESP, 10.0f, ImVec2( ( this->m_bArmored ? vecMin.x - 6.0f : vecMin.x - 1.0f ) - Fonts::ESP->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, strHealth.c_str( ) ).x, vecMin.y ), strHealth.c_str( ), colColor, DRAW_TEXT_OUTLINE, colOutline );
		this->m_flSideBarInfoHeightLeft += Fonts::ESP->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, strHealth.c_str( ) ).y;
	}		

	this->m_Context.m_arrPadding.at( DIR_LEFT ) += 7.0f;
}

void CPlayerESP::DrawArmorBar( C_CSPlayerPawn* pPawn, ImVec2 vecMin, ImVec2 vecMax, Color colColor, Color colOutline )
{
 	if ( !this->m_bArmored )
		return;

	const int iArmor = pPawn->m_ArmorValue( );
	const float flFactor = static_cast< float >( iArmor ) * 0.01f;
	const float flHeight = vecMax.y - vecMin.y;

	Draw::AddRect( vecMin, vecMax, Color( 40, 40, 40, 100 ), DRAW_RECT_FILLED | DRAW_RECT_ALIGNED, colOutline );
	Draw::AddRect( ImVec2( vecMin.x + 1.0f, ( vecMax.y + 1.0f ) - flHeight * flFactor ), ImVec2( vecMax.x - 1.0f, vecMax.y - 1.0f ), colColor, DRAW_RECT_FILLED | DRAW_RECT_ALIGNED, colOutline );

	const std::string strArmor = std::to_string( iArmor ).append( X( "%" ) );
	if (iArmor < 100)
	{
		Draw::AddText( Fonts::ESP, 10.0f, ImVec2( ( vecMin.x - 2.0f ) - Fonts::ESP->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, strArmor.c_str( ) ).x, vecMin.y + this->m_flSideBarInfoHeightLeft ), strArmor.c_str( ), colColor, DRAW_TEXT_OUTLINE, colOutline );
		this->m_flSideBarInfoHeightLeft += Fonts::ESP->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, strArmor.c_str( ) ).y;
	}	

	this->m_Context.m_arrPadding.at( DIR_LEFT ) += 7.0f;
}

void CPlayerESP::DrawName( CCSPlayerController* pEntity, ImVec2 vecPosition, Color colColor, Color colOutline )
{
	const std::string strName = pEntity->m_strSanitizedPlayerName( );
	const ImVec2 vecNameSize = Fonts::ESP->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, strName.c_str( ) );
	Draw::AddText( Fonts::ESP, 10.0f, ImVec2( vecPosition.x - vecNameSize.x * 0.5f, vecPosition.y - vecNameSize.y ), strName.c_str( ), colColor, DRAW_TEXT_OUTLINE, colOutline );
	this->m_Context.m_arrPadding.at( DIR_TOP ) += vecNameSize.y;
}

void CPlayerESP::DrawDistance( C_CSPlayerPawn* pPawn, ImVec2 vecPosition, Color colColor, Color colOutline )
{
	const Vector vecLocalOrigin = Globals::m_pLocalPlayerPawn->m_vOldOrigin( );
	const Vector vecEntityOrigin = pPawn->m_vOldOrigin( );

	if (vecEntityOrigin.IsZero( ) || vecLocalOrigin.IsZero( ))
		return;

	const float flDistance = vecEntityOrigin.DistTo( vecLocalOrigin );
	const int iDistance = static_cast< int >( M_INCH2METRE( flDistance ) );

	const std::string strText = std::to_string( iDistance ).append( X( "M" ) );
	const ImVec2 vecDistanceSize = Fonts::ESP->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, strText.c_str( ) );
	Draw::AddText( Fonts::ESP, 10.0f, ImVec2( vecPosition.x - vecDistanceSize.x * 0.5f, vecPosition.y ), strText.c_str( ), colColor, DRAW_TEXT_OUTLINE, colOutline );

	this->m_Context.m_arrPadding.at( DIR_BOTTOM ) += vecDistanceSize.y;
}

void CPlayerESP::DrawSnapLine( ImVec2 vecPosition, Color colColor )
{
	Draw::AddLine( ImVec2( static_cast<float>( Window::m_iWidth ) * 0.5f, static_cast< float >( Window::m_iHeight ) * 0.5f ), vecPosition, colColor, 1.0f );
}

void CPlayerESP::DrawFlags( CCSPlayerController* pEntity, C_CSPlayerPawn* pPawn, ImVec2 vecPosition, Color colColor, Color colOutline )
{
	CCSPlayer_ItemServices* pItemServices = pPawn->m_pItemServices( );
	CCSPlayerController_InGameMoneyServices* pInGameMoneyServices = pEntity->m_pInGameMoneyServices( );
	if (!pItemServices || !pInGameMoneyServices)
		return;

	int iFlagsHeight = 0;
	// create our flagObject
	FlagObjects_t flagObjects{ pItemServices->m_bHasHelmet( ), pItemServices->m_bHasHeavyArmor( ), pItemServices->m_bHasDefuser( ), pPawn->m_bIsScoped( ), pPawn->m_bIsDefusing( ), this->m_bFlashed, pInGameMoneyServices->m_iAccount( ) };

	if (Config::Get<std::vector<bool>>(g_Variables.m_vecFlags).at(FLAG_MONEY))
	{
		const std::string strText = std::to_string( flagObjects.m_iMoney ).append( X( "$" ) );
		const ImVec2 vecTextSize = Fonts::ESP->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, strText.c_str( ) );
		Draw::AddText( Fonts::ESP, 10.0f, ImVec2( vecPosition.x, vecPosition.y + iFlagsHeight ), strText.c_str( ), Color( 141, 227, 66, 255 ), DRAW_TEXT_OUTLINE, colOutline );
		iFlagsHeight += vecTextSize.y - 2.0f;
	}

	if (Config::Get<std::vector<bool>>(g_Variables.m_vecFlags).at(FLAG_ARMOR) && this->m_bArmored)
	{
		if (flagObjects.m_bHasHeavyArmor)
		{
			const std::string strText = X( "HEAVY" );
			const ImVec2 vecTextSize = Fonts::ESP->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, strText.c_str( ) );
			Draw::AddText( Fonts::ESP, 10.0f, ImVec2( vecPosition.x, vecPosition.y + iFlagsHeight ), strText.c_str( ), colColor, DRAW_TEXT_OUTLINE, colOutline );
			iFlagsHeight += vecTextSize.y - 2.0f;
		}
		else
		{
			const std::string strText = flagObjects.m_bHasHelmet ? X( "H+K" ) : X( "K" );
			const ImVec2 vecTextSize = Fonts::ESP->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, strText.c_str( ) );
			Draw::AddText( Fonts::ESP, 10.0f, ImVec2( vecPosition.x, vecPosition.y + iFlagsHeight ), strText.c_str( ), colColor, DRAW_TEXT_OUTLINE, colOutline );
			iFlagsHeight += vecTextSize.y - 2.0f;
		}
	}

	if (Config::Get<std::vector<bool>>(g_Variables.m_vecFlags).at(FLAG_KIT) && flagObjects.m_bHasDefuser)
	{
		const std::string strText = X( "KIT" );
		const ImVec2 vecTextSize = Fonts::ESP->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, strText.c_str( ) );
		Draw::AddText( Fonts::ESP, 10.0f, ImVec2( vecPosition.x, vecPosition.y + iFlagsHeight ), strText.c_str( ), Color( 63, 151, 224, 255 ), DRAW_TEXT_OUTLINE, colOutline );
		iFlagsHeight += vecTextSize.y - 2.0f;
	}

	if (Config::Get<std::vector<bool>>(g_Variables.m_vecFlags).at(FLAG_DEFUSING) && flagObjects.m_bIsDefusing)
	{
		const std::string strText = X( "DEFUSING" );
		const ImVec2 vecTextSize = Fonts::ESP->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, strText.c_str( ) );
		Draw::AddText( Fonts::ESP, 10.0f, ImVec2( vecPosition.x, vecPosition.y + iFlagsHeight ), strText.c_str( ), Color( 63, 151, 224, 255 ), DRAW_TEXT_OUTLINE, colOutline );
		iFlagsHeight += vecTextSize.y - 2.0f;
	}

	if (Config::Get<std::vector<bool>>(g_Variables.m_vecFlags).at(FLAG_ZOOM) && flagObjects.m_bIsScoping)
	{
		const std::string strText = X( "ZOOM" );
		const ImVec2 vecTextSize = Fonts::ESP->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, strText.c_str( ) );
		Draw::AddText( Fonts::ESP, 10.0f, ImVec2( vecPosition.x, vecPosition.y + iFlagsHeight ), strText.c_str( ), colColor, DRAW_TEXT_OUTLINE, colOutline );
		iFlagsHeight += vecTextSize.y - 2.0f;
	}
}