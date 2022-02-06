#pragma once

struct Colour
{
	typedef unsigned char Channel;

	Colour()
		: R( 255 )
		, G( 255 )
		, B( 255 )
		, A( 255 )
	{ }

	Colour( Colour a_Colour, Channel a_Alpha )
		: R( a_Colour.R )
		, G( a_Colour.G )
		, B( a_Colour.B )
		, A( a_Alpha )
	{ }

	Colour( Channel a_Red, Channel a_Green, Channel a_Blue, Channel a_Alpha = 255 )
		: R( a_Red )
		, G( a_Green )
		, B( a_Blue )
		, A( a_Alpha )
	{ }

	Colour operator +( Colour a_RHS ) const
	{
		static const float Denom = 1.0f / 255.0f;

		if ( a_RHS.A == 255 )
		{
			return a_RHS;
		}

		if ( a_RHS.A == 0 )
		{
			return *this;
		}

		Channel NewA = 255 - ( 255 - a_RHS.A ) * ( 1.0f - A * Denom );

		if ( A < 1.0e-6f )
		{
			return Colour( 255, 255, 255, 0 );
		}

		float PremultiplyLeft = float( a_RHS.A ) / NewA;
		float PremultiplyRight = A * ( 1.0f - a_RHS.A * Denom ) / NewA;

		a_RHS.R = a_RHS.R * PremultiplyLeft + R * PremultiplyRight;
		a_RHS.G = a_RHS.G * PremultiplyLeft + G * PremultiplyRight;
		a_RHS.B = a_RHS.B * PremultiplyLeft + B * PremultiplyRight;
		a_RHS.A = NewA;

		return a_RHS;
	}

	Colour& operator +=( Colour a_RHS )
	{
		static const float Denom = 1.0f / 255.0f;

		if ( a_RHS.A == 255 )
		{
			*this = a_RHS;
			return *this;
		}

		if ( a_RHS.A == 0 )
		{
			return *this;
		}

		Channel NewA = 255 - ( 255 - a_RHS.A ) * ( 1.0f - A * Denom );

		if ( A < 1.0e-6f )
		{
			*this = Colour( 255, 255, 255, 0 );
			return *this;
		}

		float PremultiplyLeft = static_cast< float >( a_RHS.A ) / NewA;
		float PremultiplyRight = A * ( 1.0f - a_RHS.A * Denom ) / NewA;

		a_RHS.R = a_RHS.R * PremultiplyLeft + R * PremultiplyRight;
		a_RHS.G = a_RHS.G * PremultiplyLeft + G * PremultiplyRight;
		a_RHS.B = a_RHS.B * PremultiplyLeft + B * PremultiplyRight;
		*this = a_RHS;
		return *this;
	}

	bool operator ==( Colour a_RHS ) const
	{
		return
			R == a_RHS.R &&
			G == a_RHS.G && 
			B == a_RHS.B &&
			A == a_RHS.A;
	}

	bool operator !=( Colour a_RHS ) const
	{
		return !operator==( a_RHS );
	}

	Channel R;
	Channel G;
	Channel B;
	Channel A;

	static const Colour WHITE;
	static const Colour BLACK;
	static const Colour GREY;
	static const Colour LIGHT_GREY;
	static const Colour DARK_GREY;
	static const Colour BROWN;
	static const Colour RED;
	static const Colour LIGHT_RED;
	static const Colour DARK_RED;
	static const Colour ORANGE;
	static const Colour LIGHT_ORANGE;
	static const Colour DARK_ORANGE;
	static const Colour YELLOW;
	static const Colour LIGHT_YELLOW;
	static const Colour DARK_YELLOW;
	static const Colour GREEN;
	static const Colour LIGHT_GREEN;
	static const Colour DARK_GREEN;
	static const Colour BLUE;
	static const Colour DARK_BLUE;
	static const Colour LIGHT_BLUE;
	static const Colour PURPLE;
	static const Colour DARK_PURPLE;
	static const Colour LIGHT_PURPLE;
	static const Colour PINK;
	static const Colour LIGHT_PINK;
	static const Colour DARK_PINK;
};

