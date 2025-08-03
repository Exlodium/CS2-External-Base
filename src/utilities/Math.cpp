#include "../Precompiled.h"

void Math::NormalizeAngles( QAngle& angAngles )
{
	for (auto i = 0; i < 3; i++) {
		while (angAngles[ i ] < -180.0f) angAngles[ i ] += 360.0f;
		while (angAngles[ i ] > 180.0f) angAngles[ i ] -= 360.0f;
	}
}

void Math::VectorAngles( const Vector& vecForward, QAngle& angView )
{
	float flPitch, flYaw;

	if (vecForward.x == 0.f && vecForward.y == 0.f)
	{
		flPitch = ( vecForward.z > 0.f ) ? 270.f : 90.f;
		flYaw = 0.f;
	}
	else
	{
		flPitch = std::atan2f( -vecForward.z, vecForward.Length2D( ) ) * 180.f / M_PI;

		if (flPitch < 0.f)
			flPitch += 360.f;

		flYaw = std::atan2f( vecForward.y, vecForward.x ) * 180.f / M_PI;

		if (flYaw < 0.f)
			flYaw += 360.f;
	}

	angView.x = flPitch;
	angView.y = flYaw;
	angView.z = 0.f;
}

float Math::GetFOV( const QAngle& angViewAngle, const QAngle& angAimAngle )
{
	auto AngleVector = []( QAngle ang ) -> Vector
		{
			return Vector(
				cos( M_DEG2RAD( ang.x ) ) * cos( M_DEG2RAD( ang.y ) ),
				cos( M_DEG2RAD( ang.x ) ) * sin( M_DEG2RAD( ang.y ) ),
				-sin( M_DEG2RAD( ang.x ) ) );
		};

	Vector vAim = AngleVector( angViewAngle ),
		vAng = AngleVector( angAimAngle );

	return M_RAD2DEG( acos( vAim.DotProduct( vAng ) / vAim.LengthSqr( ) ) );
}

QAngle Math::CalcAngle( const Vector& vecStart, const Vector& vecEnd )
{
	QAngle angView;
	const Vector vecDelta = vecEnd - vecStart;
	VectorAngles( vecDelta, angView );
	angView.Normalize( );

	return angView;
}
