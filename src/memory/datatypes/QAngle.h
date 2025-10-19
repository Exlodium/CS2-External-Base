#pragma once

class QAngle
{
public:
	QAngle( )
	{
		Init( );
	}

	QAngle( float x, float y, float z )
	{
		Init( x, y, z );
	}

	QAngle( const float* angles )
	{
		Init( angles[ 0 ], angles[ 1 ], angles[ 2 ] );
	}

	const void Init( float x = 0.f, float y = 0.f, float z = 0.f )
	{
		this->x = x; this->y = y; this->z = z;
	}

	float operator[]( std::size_t nIndex ) const
	{
		return ( ( float* )this )[ nIndex ];
	}

	float& operator[]( std::size_t nIndex )
	{
		return ( ( float* )this )[ nIndex ];
	}

	bool operator==( const QAngle& v ) const
	{
		return ( this->x == v[ 0 ] &&
			this->y == v[ 1 ] &&
			this->z == v[ 2 ] );
	}

	bool operator!=( const QAngle& v ) const
	{
		return !( this->x == v[ 0 ] &&
			this->y == v[ 1 ] &&
			this->z == v[ 2 ] );
	}

	QAngle& operator=( const QAngle& v )
	{
		this->x = v.x; this->y = v.y; this->z = v.z; return *this;
	}

	QAngle& operator+=( const QAngle& v )
	{
		this->x += v.x; this->y += v.y; this->z += v.z; return *this;
	}

	QAngle& operator-=( const QAngle& v )
	{
		this->x -= v.x; this->y -= v.y; this->z -= v.z; return *this;
	}

	QAngle& operator*=( const QAngle& v )
	{
		this->x *= v.x; this->y *= v.y; this->z *= v.z; return *this;
	}

	QAngle& operator/=( const QAngle& v )
	{
		this->x /= v.x; this->y /= v.y; this->z /= v.z; return *this;
	}

	QAngle& operator+=( float v )
	{
		this->x += v; this->y += v; this->z += v; return *this;
	}

	QAngle& operator-=( float v )
	{
		this->x -= v; this->y -= v; this->z -= v; return *this;
	}

	QAngle& operator*=( float v )
	{
		this->x *= v; this->y *= v; this->z *= v; return *this;
	}

	QAngle& operator/=( float v )
	{
		this->x /= v; this->y /= v; this->z /= v; return *this;
	}

	QAngle operator+( float v ) const
	{
		return QAngle( this->x + v, this->y + v, this->z + v );
	}

	QAngle operator-( float v ) const
	{
		return QAngle( this->x - v, this->y - v, this->z - v );
	}

	QAngle operator*( float v ) const
	{
		return QAngle( this->x * v, this->y * v, this->z * v );
	}

	QAngle operator/( float v ) const
	{
		return QAngle( this->x / v, this->y / v, this->z / v );
	}

	QAngle operator+( const QAngle& v ) const
	{
		return QAngle( this->x + v.x, this->y + v.y, this->z + v.z );
	}

	QAngle operator-( const QAngle& v ) const
	{
		return QAngle( this->x - v.x, this->y - v.y, this->z - v.z );
	}

	QAngle operator*( const QAngle& v ) const
	{
		return QAngle( this->x * v.x, this->y * v.y, this->z * v.z );
	}

	QAngle operator/( const QAngle& v ) const
	{
		return QAngle( this->x / v.x, this->y / v.y, this->z / v.z );
	}

	float Length( ) const
	{
		return std::sqrtf( this->x * this->x + this->y * this->y + this->z * this->z );
	}

	float LengthSqr( ) const
	{
		return this->x * this->x + this->y * this->y + this->z * this->z;
	}

	void Clamp( )
	{
		this->x =std::clamp( this->x, -89.f, 89.f );
		this->x =std::clamp( this->y, -180.f, 180.f );
		this->z = 0.f; // @note: i think doesnt need clamp it at -50, 50 cuz it stay useless long ago
	}

	bool IsValid( )
	{
		return !std::isnan( x ) || !std::isnan( y ) || !std::isnan( z );
	}

	bool IsZero( ) const
	{
		return ( std::fpclassify( this->x ) == FP_ZERO &&
			std::fpclassify( this->y ) == FP_ZERO &&
			std::fpclassify( this->z ) == FP_ZERO );
	}

	QAngle Normalize( )
	{
		this->x = std::isfinite( this->x ) ? std::remainder( this->x, 360.f ) : 0.f;
		this->y = std::isfinite( this->y ) ? std::remainder( this->y, 360.f ) : 0.f;
		this->z = 0.f;
		return *this;
	}

	QAngle Mod( float flValue )
	{
		this->x = std::fmodf( this->x, flValue );
		this->y = std::fmodf( this->y, flValue );
		this->z = std::fmodf( this->z, flValue );
		return *this;
	}

	Vector ToVec( )
	{
		#ifndef DEG2RAD
		#define DEG2RAD( x ) ( ( float )( x ) * ( float )( ( float )( 3.14159265358979323846f ) / 180.0f ) )
		#endif

		float	sp, sy, cp, cy;
		sy = sin( DEG2RAD( ( ( float* )this )[ 1 ] ) );
		cy = cos( DEG2RAD( ( ( float* )this )[ 1 ] ) );

		sp = sin( DEG2RAD( ( ( float* )this )[ 0 ] ) );
		cp = cos( DEG2RAD( ( ( float* )this )[ 0 ] ) );

		#undef DEG2RAD

		return { cp * cy, cp * sy, -sp };
	}

	void ToDirections(Vector* pvecForward, Vector* pvecRight, Vector* pvecUp) const
	{
		#ifndef DEG2RAD
		#define DEG2RAD( x ) ( ( float )( x ) * ( float )( ( float )( 3.14159265358979323846f ) / 180.0f ) )
		#endif

		float flPitchSin, flPitchCos, flYawSin, flYawCos, flRollSin, flRollCos;
		
		flPitchSin = sin(DEG2RAD(this->x));
		flPitchCos = cos(DEG2RAD(this->x));

		flYawSin = sin(DEG2RAD(this->y));
		flYawCos = cos(DEG2RAD(this->y));

		flRollSin = sin(DEG2RAD(this->z));
		flRollCos = cos(DEG2RAD(this->z));

		if (pvecForward != nullptr)
		{
			pvecForward->x = flPitchCos * flYawCos;
			pvecForward->y = flPitchCos * flYawSin;
			pvecForward->z = -flPitchSin;
		}

		if (pvecRight != nullptr)
		{
			pvecRight->x = (-flRollSin * flPitchSin * flYawCos) + (-flRollCos * -flYawSin);
			pvecRight->y = (-flRollSin * flPitchSin * flYawSin) + (-flRollCos * flYawCos);
			pvecRight->z = (-flRollSin * flPitchCos);
		}

		if (pvecUp != nullptr)
		{
			pvecUp->x = (flRollCos * flPitchSin * flYawCos) + (-flRollSin * -flYawSin);
			pvecUp->y = (flRollCos * flPitchSin * flYawSin) + (-flRollSin * flYawCos);
			pvecUp->z = (flRollCos * flPitchCos);
		}

		#undef DEG2RAD
	}

public:
	float x, y, z;
};