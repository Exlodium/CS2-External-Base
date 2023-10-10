#pragma once
#include <DirectXMath.h>

#define M_HPI			DirectX::XM_PIDIV2	// half pi
#define M_QPI			DirectX::XM_PIDIV4	// quarter pi
#define M_PI			DirectX::XM_PI		// pi const
#define M_2PI			DirectX::XM_2PI		// pi double
#define M_GPI			1.6180339887498f	// golden ratio
#define M_RADPI			57.295779513082f	// pi in radians

#define M_METRE2INCH( x )	( ( x ) / 0.0254f )
#define M_INCH2METRE( x )	( ( x ) * 0.0254f )
#define M_METRE2FOOT( x )	( ( x ) * 3.28f )
#define M_FOOT2METRE( x )	( ( x ) / 3.28f )
#define M_RAD2DEG( x )		DirectX::XMConvertToDegrees( x )
#define M_DEG2RAD( x )		DirectX::XMConvertToRadians( x )

namespace Math
{
	void NormalizeAngles( QAngle& angAngles );
	void VectorAngles( const Vector& vecForward, QAngle& angView );

	float GetFOV( const QAngle& angViewAngle, const QAngle& angAimAngle );
	QAngle CalcAngle( const Vector& vecStart, const Vector& vecEnd );
}