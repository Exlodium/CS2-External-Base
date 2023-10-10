#pragma once

enum
{
	COLOR_R = 0,
	COLOR_G = 1,
	COLOR_B = 2,
	COLOR_A = 3
};

struct ColorRGBExp32
{
	char r, g, b;
	signed char exponent;
};

class Color
{
public:
	Color( ) = default;

	/* default color constructor (in: 0 - 255) */
	constexpr Color( std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255 ) :
		arrColor( { r, g, b, a } ) { }

	constexpr Color( int r, int g, int b, int a = 255 ) :
		arrColor( { static_cast< std::uint8_t >( r ), static_cast< std::uint8_t >( g ), static_cast< std::uint8_t >( b ), static_cast< std::uint8_t >( a ) } ) { }

	/* float color constructor (in: 0.0 - 1.0) */
	constexpr Color( float r, float g, float b, float a = 1.0f ) :
		arrColor( { static_cast< std::uint8_t >( r * 255.f ), static_cast< std::uint8_t >( g * 255.f ), static_cast< std::uint8_t >( b * 255.f ), static_cast< std::uint8_t >( a * 255.f ) } ) { }

	/* alpha modifier coonstructor */
	constexpr Color( const Color& colSecond, int a ) :
		arrColor( { colSecond.arrColor.at( 0 ), colSecond.arrColor.at( 1 ), colSecond.arrColor.at( 2 ), static_cast< std::uint8_t >( a ) } ) { }

	constexpr Color( const Color& colSecond, float a ) :
		arrColor( { colSecond.arrColor.at( 0 ), colSecond.arrColor.at( 1 ), colSecond.arrColor.at( 2 ), static_cast< std::uint8_t >( a * 255.f ) } ) { }

	/* output color to given variables */
	void Get( std::uint8_t& r, std::uint8_t& g, std::uint8_t& b, std::uint8_t& a ) const
	{
		r = arrColor.at( COLOR_R );
		g = arrColor.at( COLOR_G );
		b = arrColor.at( COLOR_B );
		a = arrColor.at( COLOR_A );
	}

	void Set( std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a )
	{
		arrColor.at( 0 ) = r;
		arrColor.at( 1 ) = g;
		arrColor.at( 2 ) = b;
		arrColor.at( 3 ) = a;
	}

	void Set( float r, float g, float b, float a )
	{
		arrColor.at( 0 ) = ( std::uint8_t )( r * 255.f );
		arrColor.at( 1 ) = ( std::uint8_t )( g * 255.f );
		arrColor.at( 2 ) = ( std::uint8_t )( b * 255.f );
		arrColor.at( 3 ) = ( std::uint8_t )( a * 255.f );
	}

	inline std::uint8_t r( ) const { return arrColor.at( 0 ); }
	inline std::uint8_t g( ) const { return arrColor.at( 1 ); }
	inline std::uint8_t b( ) const { return arrColor.at( 2 ); }
	inline std::uint8_t a( ) const { return arrColor.at( 3 ); }

	inline float rBase( ) const { return arrColor.at( 0 ) / 255.f; }
	inline float gBase( ) const { return arrColor.at( 1 ) / 255.f; }
	inline float bBase( ) const { return arrColor.at( 2 ) / 255.f; }
	inline float aBase( ) const { return arrColor.at( 3 ) / 255.f; }

	const std::array<std::uint8_t, 4>& GetArray( ) const { return arrColor; }

	/* convert color to directx argb */
	/*D3DCOLOR GetD3D( ) const
	{
		return D3DCOLOR_ARGB( arrColor.at( COLOR_A ), arrColor.at( COLOR_R ), arrColor.at( COLOR_G ), arrColor.at( COLOR_B ) );
	}*/

	/* convert color to imgui rgba */
	ImU32 GetU32( const float flAlphaMultiplier = 1.0f ) const
	{
		return ImGui::GetColorU32( this->GetVec4( flAlphaMultiplier ) );
	}

	/* convert color to imgui vector */
	ImVec4 GetVec4( const float flAlphaMultiplier = 1.0f ) const
	{
		return ImVec4( this->Base<COLOR_R>( ), this->Base<COLOR_G>( ), this->Base<COLOR_B>( ), this->Base<COLOR_A>( ) * flAlphaMultiplier );
	}

	std::uint8_t& operator[]( const std::size_t i )
	{
		return this->arrColor.data( )[ i ];
	}

	const std::uint8_t& operator[]( const std::size_t i ) const
	{
		return this->arrColor.data( )[ i ];
	}

	bool operator==( const Color& colSecond ) const
	{
		return *const_cast< Color* >( this ) == *const_cast< Color* >( &colSecond );
	}

	bool operator!=( const Color& colSecond ) const
	{
		return !( operator==( colSecond ) );
	}

	Color& operator=( const Color& colFrom )
	{
		arrColor.at( 0 ) = colFrom.arrColor.at( COLOR_R );
		arrColor.at( 1 ) = colFrom.arrColor.at( COLOR_G );
		arrColor.at( 2 ) = colFrom.arrColor.at( COLOR_B );
		arrColor.at( 3 ) = colFrom.arrColor.at( COLOR_A );
		return *this;
	}

	Color Gradient( ) const
	{
		Color copy = *this;

		copy.arrColor.at( 0 ) /= 3;
		copy.arrColor.at( 1 ) /= 3;
		copy.arrColor.at( 2 ) /= 3;

		return copy;
	}

	/* returns certain R/G/B/A value */
	template <std::size_t N>
	std::uint8_t Get( ) const
	{
		static_assert( N >= COLOR_R && N <= COLOR_A, "given index is out of range" );
		return arrColor[ N ];
	}

	/* returns copy of color with changed certain R/G/B/A value to given value */
	template <std::size_t N>
	Color Set( const std::uint8_t nValue ) const
	{
		static_assert( N >= COLOR_R && N <= COLOR_A, "given index is out of range" );

		Color colCopy = *this;
		colCopy.arrColor[ N ] = nValue;
		return colCopy;
	}

	/* returns copy of color with multiplied certain R/G/B/A value by given value */
	template <std::size_t N>
	Color Multiplier( const float flValue ) const
	{
		static_assert( N >= COLOR_R && N <= COLOR_A, "given index is out of range" );

		Color colCopy = *this;
		colCopy.arrColor[ N ] = static_cast< std::uint8_t >( static_cast< float >( colCopy.arrColor[ N ] ) * flValue );
		return colCopy;
	}

	/* returns copy of color with divided certain R/G/B/A value by given value */
	template <std::size_t N>
	Color Divider( const int iValue ) const
	{
		static_assert( N >= COLOR_R && N <= COLOR_A, "given index is out of range" );

		Color colCopy = *this;
		colCopy.arrColor[ N ] /= iValue;
		return colCopy;
	}

	/* returns certain R/G/B/A float value (in: 0 - 255, out: 0.0 - 1.0) */
	template <std::size_t N>
	float Base( ) const
	{
		static_assert( N >= COLOR_R && N <= COLOR_A, "given index is out of range" );
		return arrColor[ N ] / 255.f;
	}

	/* convert color to float array (in: 0 - 255, out: 0.0 - 1.0) */
	std::array<float, 3U> Base( ) const
	{
		std::array<float, 3U> arrBaseColor = { };
		arrBaseColor.at( COLOR_R ) = this->Base<COLOR_R>( );
		arrBaseColor.at( COLOR_G ) = this->Base<COLOR_G>( );
		arrBaseColor.at( COLOR_B ) = this->Base<COLOR_B>( );
		return arrBaseColor;
	}

	/* set color from float array (in: 0.0 - 1.0, out: 0 - 255) */
	static Color FromBase3( float arrBase[ 3 ] )
	{
		return Color( arrBase[ 0 ], arrBase[ 1 ], arrBase[ 2 ] );
	}

	/* convert color to float array w/ alpha (in: 0 - 255, out: 0.0 - 1.0) */
	std::array<float, 4U> BaseAlpha( ) const
	{
		std::array<float, 4U> arrBaseColor = { };
		arrBaseColor.at( COLOR_R ) = this->Base<COLOR_R>( );
		arrBaseColor.at( COLOR_G ) = this->Base<COLOR_G>( );
		arrBaseColor.at( COLOR_B ) = this->Base<COLOR_B>( );
		arrBaseColor.at( COLOR_A ) = this->Base<COLOR_A>( );
		return arrBaseColor;
	}

	/* set color from float array w/ alpha (in: 0.0 - 1.0, out: 0 - 255) */
	static Color FromBase4( float arrBase[ 4 ] )
	{
		return Color( arrBase[ 0 ], arrBase[ 1 ], arrBase[ 2 ], arrBase[ 3 ] );
	}

	float Hue( ) const
	{
		if (arrColor.at( COLOR_R ) == arrColor.at( COLOR_G ) && arrColor.at( COLOR_G ) == arrColor.at( COLOR_B ))
			return 0.f;

		const float r = this->Base<COLOR_R>( );
		const float g = this->Base<COLOR_G>( );
		const float b = this->Base<COLOR_B>( );

		const float flMax = std::max( r, std::max( g, b ) ), flMin = std::min( r, std::min( g, b ) );

		if (flMax == flMin)
			return 0.f;

		const float flDelta = flMax - flMin;
		float flHue = 0.f;

		if (flMax == r)
			flHue = ( g - b ) / flDelta;
		else if (flMax == g)
			flHue = 2.f + ( b - r ) / flDelta;
		else if (flMax == b)
			flHue = 4.f + ( r - g ) / flDelta;

		flHue *= 60.f;

		if (flHue < 0.f)
			flHue += 360.f;

		return flHue / 360.f;
	}

	float Saturation( ) const
	{
		const float r = this->Base<COLOR_R>( );
		const float g = this->Base<COLOR_G>( );
		const float b = this->Base<COLOR_B>( );

		const float flMax = std::max( r, std::max( g, b ) ), flMin = std::min( r, std::min( g, b ) );
		const float flDelta = flMax - flMin;

		if (flMax == 0.f)
			return flDelta;

		return flDelta / flMax;
	}

	float Brightness( ) const
	{
		const float r = this->Base<COLOR_R>( );
		const float g = this->Base<COLOR_G>( );
		const float b = this->Base<COLOR_B>( );

		return std::max( r, std::max( g, b ) );
	}

	/* return RGB color converted from HSB/HSV color */
	static Color FromHSB( float flHue, float flSaturation, float flBrightness )
	{
		const float h = std::fmodf( flHue, 1.0f ) / ( 60.0f / 360.0f );
		const int i = static_cast< int >( h );
		const float f = h - static_cast< float >( i );
		const float p = flBrightness * ( 1.0f - flSaturation );
		const float q = flBrightness * ( 1.0f - flSaturation * f );
		const float t = flBrightness * ( 1.0f - flSaturation * ( 1.0f - f ) );

		float r = 0.0f, g = 0.0f, b = 0.0f;

		switch (i)
		{
		case 0:
			r = flBrightness, g = t, b = p;
			break;
		case 1:
			r = q, g = flBrightness, b = p;
			break;
		case 2:
			r = p, g = flBrightness, b = t;
			break;
		case 3:
			r = p, g = q, b = flBrightness;
			break;
		case 4:
			r = t, g = p, b = flBrightness;
			break;
		case 5:
		default:
			r = flBrightness, g = p, b = q;
			break;
		}

		return Color( r, g, b );
	}

	/* return RGB color converted from heximal input */
	static Color FromHex( unsigned uHexValue )
	{
		int iR = ( uHexValue >> 24 ) & 0xFF;
		int iG = ( uHexValue >> 16 ) & 0xFF;
		int iB = ( uHexValue >> 8 ) & 0xFF;
		int iA = ( uHexValue ) & 0xFF;

		return Color( iR, iG, iB, iA );
	}

	/* convert color to heximal value */
	[[nodiscard]] unsigned GetHex( ) const
	{
		return ( ( ( arrColor[ COLOR_R ] ) << 24 ) | ( ( arrColor[ COLOR_G ] ) << 16 ) | ( ( arrColor[ COLOR_B ] ) << 8 ) | ( ( arrColor[ COLOR_A ] ) ) );
	}

	Color OverrideAlpha( int alpha )
	{
		return Color( static_cast<int>( arrColor.at( COLOR_R ) ) , static_cast< int >( arrColor.at( COLOR_G ) ), static_cast< int >( arrColor.at( COLOR_B ) ), alpha );
	}

private:
	std::array<std::uint8_t, 4U> arrColor;
};
