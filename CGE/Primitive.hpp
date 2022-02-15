#pragma once
#include "Colour.h"
#include "Math.h"
#include "CGE.h"
#include "Line.hpp"

struct Point
{
	Colour Colour;
	Vector2 Position;
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
};