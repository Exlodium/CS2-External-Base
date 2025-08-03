#include "../Precompiled.h"

void Draw::RenderDrawData( ImDrawList* pDrawList )
{
	std::unique_lock<std::shared_mutex> lock( drawMutex );

	if (vecSafeDrawData.empty( ))
		return;

	for (const auto& data : vecSafeDrawData)
	{
		if (!data.pObject.has_value( ))
			continue;

		switch (data.nType)
		{
			case EDrawType::LINE:
			{
				const auto& pObject = std::any_cast< LineObject_t >( data.pObject );
				pDrawList->AddLine( pObject.vecStart, pObject.vecEnd, pObject.colLine, pObject.flThickness );
				break;
			}
			case EDrawType::RECT:
			{
				const auto& pObject = std::any_cast< RectObject_t >( data.pObject );

				if (pObject.uFlags & DRAW_RECT_ALIGNED)
					AddDrawListRect( pDrawList,
						{ floor( pObject.vecMin.x ), floor( pObject.vecMin.y ) },
						{ floor( pObject.vecMax.x ), floor( pObject.vecMax.y ) },
						pObject.colRect, pObject.uFlags, pObject.colOutline, pObject.flRounding, pObject.flThickness );
				else
					AddDrawListRect( pDrawList, pObject.vecMin, pObject.vecMax, pObject.colRect, pObject.uFlags, pObject.colOutline, pObject.flRounding, pObject.flThickness );

				break;
			}
			case EDrawType::RECT_MULTICOLOR:
			{
				const auto& pObject = std::any_cast< RectMultiColorObject_t >( data.pObject );

				if (pObject.uFlags & DRAW_RECT_ALIGNED)
					AddDrawListRectMultiColor( pDrawList,
						{ floor( pObject.vecMin.x ), floor( pObject.vecMin.y ) },
						{ floor( pObject.vecMax.x ), floor( pObject.vecMax.y ) },
						pObject.colUpperLeft, pObject.colUpperRight,
						pObject.colBottomLeft, pObject.colBottomRight,
						pObject.uFlags, pObject.colOutline );
				else
					AddDrawListRectMultiColor( pDrawList,
						pObject.vecMin,
						pObject.vecMax,
						pObject.colUpperLeft, pObject.colUpperRight,
						pObject.colBottomLeft, pObject.colBottomRight,
						pObject.uFlags, pObject.colOutline );

				break;
			}
			case EDrawType::CIRCLE:
			{
				const auto& pObject = std::any_cast< CircleObject_t >( data.pObject );

				if (pObject.uFlags & DRAW_CIRCLE_FILLED)
					pDrawList->AddCircleFilled( pObject.vecCenter, pObject.flRadius, pObject.colCircle, pObject.nSegments );
				else
					pDrawList->AddCircle( pObject.vecCenter, pObject.flRadius, pObject.colCircle, pObject.nSegments, pObject.flThickness );

				if (pObject.uFlags & DRAW_CIRCLE_OUTLINE)
					pDrawList->AddCircle( pObject.vecCenter, pObject.flRadius + 1.0f, pObject.colOutline, pObject.nSegments, pObject.flThickness + 1.0f );

				break;
			}
			case EDrawType::TRIANGLE:
			{
				const auto& pObject = std::any_cast< TriangleObject_t >( data.pObject );

				if (pObject.uFlags & DRAW_TRIANGLE_FILLED)
					pDrawList->AddTriangleFilled( pObject.vecFirst, pObject.vecSecond, pObject.vecThird, pObject.colTriangle );
				else
					pDrawList->AddTriangle( pObject.vecFirst, pObject.vecSecond, pObject.vecThird, pObject.colTriangle, pObject.flThickness );

				if (pObject.uFlags & DRAW_TRIANGLE_OUTLINE)
					pDrawList->AddTriangle( pObject.vecFirst, pObject.vecSecond, pObject.vecThird, pObject.colOutline, pObject.flThickness + 1.0f );

				break;
			}
			case EDrawType::POLYGON:
			{
				const auto& pObject = std::any_cast< PolygonObject_t >( data.pObject );

				if (pObject.uFlags & DRAW_POLYGON_FILLED)
					pDrawList->AddConvexPolyFilled( pObject.vecPoints.data( ), static_cast< int >( pObject.vecPoints.size( ) ), pObject.colPolygon );
				else
					pDrawList->AddPolyline( pObject.vecPoints.data( ), static_cast<int>( pObject.vecPoints.size( ) ), pObject.colPolygon, pObject.bClosed, pObject.flThickness );

				if (pObject.uFlags & DRAW_POLYGON_OUTLINE)
					pDrawList->AddPolyline( pObject.vecPoints.data( ), static_cast< int >( pObject.vecPoints.size( ) ), pObject.colOutline, pObject.bClosed, pObject.flThickness + 1.0f );

				break;
			}
			case EDrawType::TEXT:
			{
				const auto& pObject = std::any_cast< TextObject_t >( data.pObject );
				AddDrawListText( pDrawList, pObject.pFont, pObject.flFontSize, pObject.vecPosition, pObject.szText, pObject.colText, pObject.uFlags, pObject.colOutline );
				break;
			}
			case EDrawType::IMAGE:
			{
				const auto& pObject = std::any_cast< ImageObject_t >( data.pObject );
				pDrawList->AddImageRounded( pObject.pTexture, pObject.vecMin, pObject.vecMax, ImVec2( 0, 0 ), ImVec2( 1, 1 ), pObject.colImage, pObject.flRounding );
				break;
			}
			case EDrawType::ARC:
			{
				const auto& pObject = std::any_cast< ArcObject_t >( data.pObject );
				pDrawList->PathArcTo( pObject.vecPosition, pObject.flRadius, M_DEG2RAD( pObject.flMinAngle ), M_DEG2RAD( pObject.flMaxAngle ), 32 );
				pDrawList->PathStroke( pObject.colFill, false, pObject.flThickness );
				break;
			}
			case EDrawType::RING:
			{
				Vector prevPos;
				ImVec2 pervScreenPos, screenPos;
				const auto& pObject = std::any_cast< RingObject_t >( data.pObject );
				float step = ( float )M_PI * 2.0f / pObject.nSegments;

				for (float a = 0; a < ( M_PI * 2.0f ); a += step)
				{
					Vector start( pObject.flRadius * cosf( a ) + pObject.vecCenter.x, pObject.flRadius * sinf( a ) + pObject.vecCenter.y, pObject.vecCenter.z );
					Vector end( pObject.flRadius * cosf( a + step ) + pObject.vecCenter.x, pObject.flRadius * sinf( a + step ) + pObject.vecCenter.y, pObject.vecCenter.z );

					ImVec2 start2d;
					ImVec2 end2d;

					ImVec2 start22d( start2d.x, start2d.y );
					ImVec2 end22d( end2d.x, end2d.y );
					if (WorldToScreen( start, start2d ) &&
						WorldToScreen( end, end2d ))
					{
						start22d.x = start2d.x;
						start22d.y = start2d.y;

						end22d.x = end2d.x;
						end22d.y = end2d.y;

						pDrawList->AddLine( ImVec2( start22d.x, start22d.y ), ImVec2( end22d.x, end22d.y ), pObject.colCircle, pObject.flThickness );
					}
				}
				break;
		}
		default:
			break;
		}
	}
}

void Draw::ClearDrawData( )
{
	if (!vecDrawData.empty( ))
		vecDrawData.clear( );
}

void Draw::SwapDrawData( )
{
	std::unique_lock<std::shared_mutex> lock( drawMutex );
	vecDrawData.swap( vecSafeDrawData );
}

#pragma region draw_render

void Draw::AddLine( const ImVec2& vecStart, const ImVec2& vecEnd, const Color& colLine, float flThickness )
{
	vecDrawData.emplace_back( EDrawType::LINE, std::make_any<LineObject_t>( LineObject_t{ vecStart, vecEnd, colLine.GetU32( ), flThickness } ) );
}

void Draw::AddRect( const ImVec2& vecMin, const ImVec2& vecMax, const Color& colRect, unsigned int uFlags, const Color& colOutline, float flRounding, float flThickness )
{
	vecDrawData.emplace_back( EDrawType::RECT, std::make_any<RectObject_t>( RectObject_t{ vecMin, vecMax, colRect.GetU32( ), uFlags, colOutline.GetU32( ), flRounding, flThickness } ) );
}

void Draw::AddRectMultiColor( const ImVec2& vecMin, const ImVec2& vecMax, const Color& colUpperLeft, const Color& colUpperRight, const Color& colBottomLeft, const Color& colBottomRight, unsigned int uFlags, const Color& colOutline )
{
	vecDrawData.emplace_back( EDrawType::RECT_MULTICOLOR, std::make_any<RectMultiColorObject_t>( RectMultiColorObject_t{ vecMin, vecMax, colUpperLeft.GetU32( ), colUpperRight.GetU32( ), colBottomLeft.GetU32( ), colBottomRight.GetU32( ), uFlags, colOutline.GetU32( ) } ) );
}

void Draw::AddCircle( const ImVec2& vecCenter, float flRadius, const Color& colCircle, int nSegments, unsigned int uFlags, const Color& colOutline, float flThickness )
{
	vecDrawData.emplace_back( EDrawType::CIRCLE, std::make_any<CircleObject_t>( CircleObject_t{ vecCenter, flRadius, colCircle.GetU32( ), nSegments, uFlags, colOutline.GetU32( ), flThickness } ) );
}

void Draw::AddRing( const Vector& vecCenter, float flRadius, const Color& colCircle, int nSegments, unsigned int uFlags, float flThickness )
{
	vecDrawData.emplace_back( EDrawType::RING, std::make_any<RingObject_t>( RingObject_t{ vecCenter, flRadius, colCircle.GetU32( ), nSegments, uFlags, flThickness } ) );
}

void Draw::AddTriangle( const ImVec2& vecFirst, const ImVec2& vecSecond, const ImVec2& vecThird, const Color& colTriangle, unsigned int uFlags, const Color& colOutline, float flThickness )
{
	vecDrawData.emplace_back( EDrawType::TRIANGLE, std::make_any<TriangleObject_t>( TriangleObject_t{ vecFirst, vecSecond, vecThird, colTriangle.GetU32( ), uFlags, colOutline.GetU32( ), flThickness } ) );
}

void Draw::AddPolygon( std::vector<ImVec2>& vecPoints, const Color& colPolygon, unsigned int uFlags, const Color& colOutline, bool bClosed, float flThickness )
{
	vecDrawData.emplace_back( EDrawType::POLYGON, std::make_any<PolygonObject_t>( PolygonObject_t{ std::move( vecPoints ), colPolygon.GetU32( ), uFlags, colOutline.GetU32( ), bClosed, flThickness } ) );
}

void Draw::AddText( const ImFont* pFont, float flFontSize, const ImVec2& vecPosition, const std::string& szText, const Color& colText, unsigned int uFlags, const Color& colOutline )
{
	if (pFont->ContainerAtlas == nullptr)
		return;

	// check is only one flag selected
	IM_ASSERT( ImIsPowerOfTwo( uFlags == DRAW_TEXT_NONE || uFlags & ( DRAW_TEXT_DROPSHADOW | DRAW_TEXT_OUTLINE ) ) );

	vecDrawData.emplace_back( EDrawType::TEXT, std::make_any<TextObject_t>( TextObject_t{ pFont, flFontSize, vecPosition, szText, colText.GetU32( ), uFlags, colOutline.GetU32( ) } ) );
}

void Draw::AddText( const ImVec2& vecPosition, const std::string& szText, const Color& colText, int iFlags, const Color& colOutline )
{
	AddText( nullptr, 0.f, vecPosition, szText, colText, iFlags, colOutline );
}

void Draw::AddImage( ImTextureID pTexture, const ImVec2& vecMin, const ImVec2& vecMax, const Color& colImage, float flRounding )
{
	vecDrawData.emplace_back( EDrawType::IMAGE, std::make_any<ImageObject_t>( ImageObject_t{ pTexture, vecMin, vecMax, colImage.GetU32( ), flRounding } ) );
}

void Draw::AddArcFilled( const ImVec2& vecPosition, float flRadius, float flMinAngle, float flMaxAngle, const Color& colFill, float flThickness ) {
	vecDrawData.emplace_back( EDrawType::ARC, std::make_any<ArcObject_t>( ArcObject_t{ vecPosition,flRadius,flMinAngle,flMaxAngle,colFill.GetU32( ),flThickness } ) );
}
#pragma endregion

#pragma region draw_bindings
void Draw::AddDrawListRect( ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, ImU32 colRect, unsigned int uFlags, ImU32 colOutline, float flRounding, float flThickness )
{
	if (uFlags & DRAW_RECT_FILLED)
		pDrawList->AddRectFilled( vecMin, vecMax, colRect, flRounding );
	else
		pDrawList->AddRect( vecMin, vecMax, colRect, flRounding, 0, flThickness );

	if (uFlags & DRAW_RECT_BORDER)
		pDrawList->AddRect( vecMin + ImVec2( 1.0f, 1.0f ), vecMax - ImVec2( 1.0f, 1.0f ), colOutline, flRounding, 0, 1.0f );

	if (uFlags & DRAW_RECT_OUTLINE)
		pDrawList->AddRect( vecMin - ImVec2( 1.0f, 1.0f ), vecMax + ImVec2( 1.0f, 1.0f ), colOutline, flRounding,0, 1.0f );
}

void Draw::AddDrawListRectMultiColor( ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, ImU32 colUpperLeft, ImU32 colUpperRight, ImU32 colBottomLeft, ImU32 colBottomRight, unsigned int uFlags, ImU32 colOutline )
{
	// @todo:
	// implement thickness

	if (uFlags & DRAW_RECT_FILLED)
		pDrawList->AddRectFilledMultiColor( vecMin, vecMax, colUpperLeft, colUpperRight, colBottomRight, colBottomLeft );
	else
	{
		pDrawList->AddRectFilledMultiColor( vecMin, ImVec2( vecMax.x, vecMin.y + 1.f ), colUpperLeft, colUpperRight, colUpperRight, colUpperLeft ); // top
		pDrawList->AddRectFilledMultiColor( ImVec2( vecMin.x, vecMax.y ), vecMax - ImVec2( 0, 1.f ), colBottomLeft, colBottomRight, colBottomRight, colBottomLeft ); // bottom

		pDrawList->AddRectFilledMultiColor( vecMin, ImVec2( vecMin.x + 1.f, vecMax.y ), colUpperLeft, colUpperLeft, colBottomLeft, colBottomLeft ); // left
		pDrawList->AddRectFilledMultiColor( ImVec2( vecMax.x, vecMin.y ), vecMax - ImVec2( 1.f, 0 ), colUpperRight, colUpperRight, colBottomRight, colBottomRight ); // right
	}

	if (uFlags & DRAW_RECT_BORDER)
		pDrawList->AddRect( vecMin + ImVec2( 1.0f, 1.0f ), vecMax - ImVec2( 1.0f, 1.0f ), colOutline );

	if (uFlags & DRAW_RECT_OUTLINE)
		pDrawList->AddRect( vecMin - ImVec2( 1.0f, 1.0f ), vecMax + ImVec2( 1.0f, 1.0f ), colOutline );
}

void Draw::AddDrawListText( ImDrawList* pDrawList, const ImFont* pFont, float flFontSize, const ImVec2& vecPosition, const std::string& szText, ImU32 colText, unsigned int uFlags, ImU32 colOutline )
{
	// set font texture
	pDrawList->PushTextureID( pFont->ContainerAtlas->TexID );

	if (uFlags & DRAW_TEXT_DROPSHADOW)
		pDrawList->AddText( pFont, flFontSize, vecPosition + ImVec2( 1.0f, 1.0f ), colOutline, szText.c_str( ) );
	else if (uFlags & DRAW_TEXT_OUTLINE)
	{
		pDrawList->AddText( pFont, flFontSize, vecPosition + ImVec2( 0.0f, -1.0f ), colOutline, szText.c_str( ) );
		pDrawList->AddText( pFont, flFontSize, vecPosition + ImVec2( 0.0f, 1.0f ), colOutline, szText.c_str( ) );
		pDrawList->AddText( pFont, flFontSize, vecPosition + ImVec2( -1.0f, 0.0f ), colOutline, szText.c_str( ) );
		pDrawList->AddText( pFont, flFontSize, vecPosition + ImVec2( 1.0f, 0.0f ), colOutline, szText.c_str( ) );
	}

	pDrawList->AddText( pFont, flFontSize, vecPosition, colText, szText.data( ) );
	pDrawList->PopTextureID( );
}
#pragma endregion

#pragma region draw_extra
bool Draw::WorldToScreen( const Vector& vecOrigin, ImVec2& vecScreen )
{
	const ViewMatrix_t& matWorldToScreen = g_Memory.Read<ViewMatrix_t>(Offsets::Client::dwViewMatrix);
	const float flWidth = matWorldToScreen[ 3 ][ 0 ] * vecOrigin.x + matWorldToScreen[ 3 ][ 1 ] * vecOrigin.y + matWorldToScreen[ 3 ][ 2 ] * vecOrigin.z + matWorldToScreen[ 3 ][ 3 ];

	if (flWidth < 0.001f)
		return false;

	// compute the scene coordinates of a point in 3d
	const float flInverse = 1.0f / flWidth;
	vecScreen.x = ( matWorldToScreen[ 0 ][ 0 ] * vecOrigin.x + matWorldToScreen[ 0 ][ 1 ] * vecOrigin.y + matWorldToScreen[ 0 ][ 2 ] * vecOrigin.z + matWorldToScreen[ 0 ][ 3 ] ) * flInverse;
	vecScreen.y = ( matWorldToScreen[ 1 ][ 0 ] * vecOrigin.x + matWorldToScreen[ 1 ][ 1 ] * vecOrigin.y + matWorldToScreen[ 1 ][ 2 ] * vecOrigin.z + matWorldToScreen[ 1 ][ 3 ] ) * flInverse;

	// screen transform
	// get the screen position in pixels of given point
	const ImVec2 vecDisplaySize = ImGui::GetIO( ).DisplaySize;
	vecScreen.x = ( vecDisplaySize.x * 0.5f ) + ( vecScreen.x * vecDisplaySize.x ) * 0.5f;
	vecScreen.y = ( vecDisplaySize.y * 0.5f ) - ( vecScreen.y * vecDisplaySize.y ) * 0.5f;
	return true;
}
#pragma endregion