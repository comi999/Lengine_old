#pragma once
#include "Colour.hpp"
#include "Math.hpp"
#include "CGE.hpp"
#include "Line.hpp"

struct Point
{
	Colour Colour;
	Vector2 Position;
};

struct Triangle
{
	union
	{
		Point P[ 3 ];
		struct { Point p0, p1, p2; };
	};
};

class Primitive
{
public:

	static inline void DrawPoint( Vector2 a_Position, Colour a_Colour )
	{
		ScreenBuffer::SetColour( Vector< short, 2 >( static_cast< short >( ScreenBuffer::GetBufferHeight() - a_Position.y - 1 ), static_cast< short >( a_Position.x ) ), a_Colour );
	}

	static inline void DrawPoint( const Point& a_Point )
	{
		ScreenBuffer::SetColour( Vector< short, 2 >( static_cast< short >( ScreenBuffer::GetBufferHeight() - a_Point.Position.y - 1 ), static_cast< short >( a_Point.Position.x ) ), a_Point.Colour );
	}

	static void DrawLine( Vector2 a_Start, Vector2 a_End, Colour a_Colour )
	{
		Line Segment = { a_Start, a_End };
		Segment.RectClamp( ScreenBuffer::GetBufferRect() );
		Vector< short, 2 > Start = { static_cast< short >( Segment.Start.x ), static_cast< short >( Segment.Start.y ) };
		Vector< short, 2 > End = { static_cast< short >( Segment.End.x ), static_cast< short >( Segment.End.y ) };

		// Invert Y.
		short MaxY = ScreenBuffer::GetBufferHeight() - 1;
		Start.y = MaxY - Start.y;
		End.y = MaxY - End.y;

		short DX = abs( End.x - Start.x );
		short DY = -abs( End.y - Start.y );
		short SX = Start.x < End.x ? 1 : -1;
		short SY = Start.y < End.y ? 1 : -1;
		short ER = DX + DY;

		if ( ScreenBuffer::BlendingEnabled )
		{
			while ( true )
			{
				ScreenBuffer::SetColour( Start, a_Colour );

				if ( Start.x == End.x && Start.y == End.y )
				{
					break;
				}

				short ER2 = 2 * ER;

				if ( ER2 >= DY )
				{
					if ( Start.x == End.x )
					{
						break;
					}

					ER += DY;
					Start.x += SX;
				}

				if ( ER2 <= DX )
				{
					if ( Start.y == End.y )
					{
						break;
					}

					ER += DX;
					Start.y += SY;
				}
			}
		}
		else
		{
			Pixel PixelToSet = ScreenBuffer::GetPixelColourMap().ConvertColour( a_Colour );

			while ( true )
			{
				ScreenBuffer::SetPixel( Start, PixelToSet );

				if ( Start.x == End.x && Start.y == End.y )
				{
					break;
				}

				short ER2 = 2 * ER;

				if ( ER2 >= DY )
				{
					if ( Start.x == End.x )
					{
						break;
					}

					ER += DY;
					Start.x += SX;
				}

				if ( ER2 <= DX )
				{
					if ( Start.y == End.y )
					{
						break;
					}

					ER += DX;
					Start.y += SY;
				}
			}
		}
	}

	static void DrawLine( Line a_Line, Colour a_Colour )
	{
		a_Line.RectClamp( ScreenBuffer::GetBufferRect() );
		Vector< short, 2 > Start( static_cast< short >( a_Line.Start.x ), static_cast< short >( a_Line.Start.y ) );
		Vector< short, 2 > End( static_cast< short >( a_Line.End.x ), static_cast< short >( a_Line.End.y ) );

		// Invert Y.
		short MaxY = ScreenBuffer::GetBufferHeight() - 1;
		Start.y = MaxY - Start.y;
		End.y   = MaxY - End.y;

		short DX =  abs( End.x - Start.x );
		short DY = -abs( End.y - Start.y );
		short SX = Start.x < End.x ? 1 : -1;
		short SY = Start.y < End.y ? 1 : -1;
		short ER = DX + DY;

		if ( ScreenBuffer::BlendingEnabled )
		{
			while ( true )
			{
				ScreenBuffer::SetColour( Start, a_Colour );

				if ( Start.x == End.x && Start.y == End.y )
				{
					break;
				}

				short ER2 = 2 * ER;

				if ( ER2 >= DY )
				{
					if ( Start.x == End.x )
					{
						break;
					}

					ER += DY;
					Start.x += SX;
				}

				if ( ER2 <= DX )
				{
					if ( Start.y == End.y )
					{
						break;
					}

					ER += DX;
					Start.y += SY;
				}
			}
		}
		else
		{
			Pixel PixelToSet = ScreenBuffer::GetPixelColourMap().ConvertColour( a_Colour );

			while ( true )
			{
				ScreenBuffer::SetPixel( Start, PixelToSet );

				if ( Start.x == End.x && Start.y == End.y )
				{
					break;
				}

				short ER2 = 2 * ER;

				if ( ER2 >= DY )
				{
					if ( Start.x == End.x )
					{
						break;
					}

					ER += DY;
					Start.x += SX;
				}

				if ( ER2 <= DX )
				{
					if ( Start.y == End.y )
					{
						break;
					}

					ER += DX;
					Start.y += SY;
				}
			}
		}
	}

	static void DrawTriangle( Vector2 a_P0, Vector2 a_P1, Vector2 a_P2, Colour a_Colour )
	{
		// Correct Y
		a_P0.y = -a_P0.y - 1.0f + ScreenBuffer::GetBufferHeight();
		a_P1.y = -a_P1.y - 1.0f + ScreenBuffer::GetBufferHeight();
		a_P2.y = -a_P2.y - 1.0f + ScreenBuffer::GetBufferHeight();

		// Sort corners.
		if ( a_P0.y < a_P1.y )
		{
			std::swap( a_P0, a_P1 );
		}

		if ( a_P0.y < a_P2.y )
		{
			std::swap( a_P0, a_P2 );
		}

		if ( a_P1.y < a_P2.y )
		{
			std::swap( a_P1, a_P2 );
		}

		// If points are colinear, reject.
		if ( a_P0.y == a_P2.y || a_P0.x == a_P2.x )
		{
			return;
		}

		// Align y.
		a_P0.y = static_cast< int >( a_P0.y );
		a_P1.y = static_cast< int >( a_P1.y );
		a_P2.y = static_cast< int >( a_P2.y );

		// Find M
		Vector2 M;
		
		if ( a_P0.y == a_P2.y )
		{
			M.y = a_P1.y;
			M.x = a_P0.x;
		}
		else
		{
			M.y = a_P1.y;
			M.x = a_P0.x - ( a_P0.y - a_P1.y ) * ( a_P0.x - a_P2.x ) / ( a_P0.y - a_P2.y );
		}

		float InvGrad = ( a_P0.x - a_P2.x ) / ( a_P0.y - a_P2.y );

		// Long side is on left
		if ( M.x <= a_P1.x )
		{
			float InvM;
			int XLeft, XRight, YMin, YMax;

			// Top
			if ( a_P0.y != a_P1.y )
			{
				InvM = ( a_P0.x - a_P1.x ) / ( a_P0.y - a_P1.y );
				YMin = Math::Max( a_P1.y, 0.0f );
				YMax = Math::Min( a_P0.y, -1.0f + ScreenBuffer::GetBufferHeight() );

				for ( int y = YMin; y < YMax; ++y )
				{
					XLeft  = Math::Max( 0.0f, ( y - a_P0.y ) * InvGrad + a_P0.x );
					XRight = Math::Min( -1.0f + ScreenBuffer::GetBufferWidth(), ( y - a_P0.y ) * InvM    + a_P0.x );
					ScreenBuffer::SetColours( Vector< short, 2 >( XLeft, y ), a_Colour, XRight - XLeft );
				}
			}

			// Bottom
			if ( a_P1.y != a_P2.y )
			{
				InvM = ( a_P1.x - a_P2.x ) / ( a_P1.y - a_P2.y );
				YMin = Math::Max( a_P2.y, 0.0f );
				YMax = Math::Min( a_P1.y, -1.0f + ScreenBuffer::GetBufferHeight() );

				for ( int y = YMin; y < YMax; ++y )
				{
					XLeft  = Math::Max( 0.0f, ( y - a_P2.y ) * InvGrad + a_P2.x );
					XRight = Math::Min( -1.0f + ScreenBuffer::GetBufferWidth(), ( y - a_P2.y ) * InvM    + a_P2.x );
					ScreenBuffer::SetColours( Vector< short, 2 >( XLeft, y ), a_Colour, XRight - XLeft );
				}
			}
		}

		// Long side is on right
		if ( M.x > a_P1.x )
		{
			float InvM;
			int XLeft, XRight, YMin, YMax;

			// Top
			if ( a_P0.y != a_P1.y )
			{
				InvM = ( a_P0.x - a_P1.x ) / ( a_P0.y - a_P1.y );
				YMin = Math::Max( a_P1.y, 0.0f );
				YMax = Math::Min( a_P0.y, -1.0f + ScreenBuffer::GetBufferHeight() );

				for ( int y = YMin; y < YMax; ++y )
				{
					XLeft  = Math::Max( 0.0f, ( y - a_P0.y ) * InvM    + a_P0.x );
					XRight = Math::Min( -1.0f + ScreenBuffer::GetBufferWidth(), ( y - a_P0.y ) * InvGrad + a_P0.x );
					ScreenBuffer::SetColours( Vector< short, 2 >( XLeft, y ), a_Colour, XRight - XLeft );
				}
			}

			// Bottom
			if ( a_P1.y != a_P2.y )
			{
				InvM = ( a_P1.x - a_P2.x ) / ( a_P1.y - a_P2.y );
				YMin = Math::Max( a_P2.y, 0.0f );
				YMax = Math::Min( a_P1.y, -1.0f + ScreenBuffer::GetBufferHeight() );

				for ( int y = YMin; y < YMax; ++y )
				{
					XLeft  = Math::Max( 0.0f, ( y - a_P2.y ) * InvM    + a_P2.x );
					XRight = Math::Min( -1.0f + ScreenBuffer::GetBufferWidth(), ( y - a_P2.y ) * InvGrad + a_P2.x );
					ScreenBuffer::SetColours( Vector< short, 2 >( XLeft, y ), a_Colour, XRight - XLeft );
				}
			}
		}
	}
};