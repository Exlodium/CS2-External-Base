#include "../Includes.h"

float CMath::GetFOV( QAngle angViewAngle, QAngle angAimAngle )
{
	Vector vAim = angViewAngle.ToVec();
	Vector vAng = angAimAngle.ToVec();

	return M_RAD2DEG( acos( vAim.DotProduct( vAng ) / vAim.LengthSqr( ) ) );
}

QAngle CMath::CalcAngle( const Vector& vecStart, const Vector& vecEnd )
{
	Vector vecDelta = vecEnd - vecStart;
	QAngle angView = vecDelta.ToAngles();
	angView.Normalize( );
	return angView;
}