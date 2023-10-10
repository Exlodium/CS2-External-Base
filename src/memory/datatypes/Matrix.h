#pragma once

struct ViewMatrix_t
{
	ViewMatrix_t( ) = default;

	float* operator[]( const int nIndex )
	{
		return flData[ nIndex ];
	}

	const float* operator[]( const int nIndex ) const
	{
		return flData[ nIndex ];
	}

	float flData[ 4 ][ 4 ] = { };
};