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
		ScreenBuffer::SetColour( { ScreenBuffer::GetBufferHeight() - static_cast< short >( a_Position.j ) - 1, static_cast< short >( a_Position.i ) }, a_Colour );
	}

	static inline void DrawPoint( const Point& a_Point )
	{
		ScreenBuffer::SetColour( { ScreenBuffer::GetBufferHeight() - static_cast< short >( a_Point.Position.j ) - 1, static_cast< short >( a_Point.Position.i ) }, a_Point.Colour );
	}

	static void DrawLine( Vector2 a_Start, Vector2 a_End, Colour a_Colour )
	{
		Line Segment = { a_Start, a_End };
		Segment.RectClamp( ScreenBuffer::GetBufferRect() );
		tVector2< short > Start = { static_cast< short >( Segment.Start.i ), static_cast< short >( Segment.Start.j ) };
		tVector2< short > End = { static_cast< short >( Segment.End.i ), static_cast< short >( Segment.End.j ) };

		// Invert Y.
		short MaxY = ScreenBuffer::GetBufferHeight() - 1;
		Start.j = MaxY - Start.j;
		End.j = MaxY - End.j;

		short DX = abs( End.i - Start.i );
		short DY = -abs( End.j - Start.j );
		short SX = Start.i < End.i ? 1 : -1;
		short SY = Start.j < End.j ? 1 : -1;
		short ER = DX + DY;

		if ( ScreenBuffer::BlendingEnabled )
		{
			while ( true )
			{
				ScreenBuffer::SetColour( Start, a_Colour );

				if ( Start.i == End.i && Start.j == End.j )
				{
					break;
				}

				short ER2 = 2 * ER;

				if ( ER2 >= DY )
				{
					if ( Start.i == End.i )
					{
						break;
					}

					ER += DY;
					Start.i += SX;
				}

				if ( ER2 <= DX )
				{
					if ( Start.j == End.j )
					{
						break;
					}

					ER += DX;
					Start.j += SY;
				}
			}
		}
		else
		{
			Pixel PixelToSet = ScreenBuffer::GetPixelColourMap().ConvertColour( a_Colour );

			while ( true )
			{
				ScreenBuffer::SetPixel( Start, PixelToSet );

				if ( Start.i == End.i && Start.j == End.j )
				{
					break;
				}

				short ER2 = 2 * ER;

				if ( ER2 >= DY )
				{
					if ( Start.i == End.i )
					{
						break;
					}

					ER += DY;
					Start.i += SX;
				}

				if ( ER2 <= DX )
				{
					if ( Start.j == End.j )
					{
						break;
					}

					ER += DX;
					Start.j += SY;
				}
			}
		}
	}

	static void DrawLine( Line a_Line, Colour a_Colour )
	{
		a_Line.RectClamp( ScreenBuffer::GetBufferRect() );
		tVector2< short > Start = { static_cast< short >( a_Line.Start.i ), static_cast< short >( a_Line.Start.j ) };
		tVector2< short > End = { static_cast< short >( a_Line.End.i ), static_cast< short >( a_Line.End.j ) };

		// Invert Y.
		short MaxY = ScreenBuffer::GetBufferHeight() - 1;
		Start.j = MaxY - Start.j;
		End.j   = MaxY - End.j;

		short DX =  abs( End.i - Start.i );
		short DY = -abs( End.j - Start.j );
		short SX = Start.i < End.i ? 1 : -1;
		short SY = Start.j < End.j ? 1 : -1;
		short ER = DX + DY;

		if ( ScreenBuffer::BlendingEnabled )
		{
			while ( true )
			{
				ScreenBuffer::SetColour( Start, a_Colour );

				if ( Start.i == End.i && Start.j == End.j )
				{
					break;
				}

				short ER2 = 2 * ER;

				if ( ER2 >= DY )
				{
					if ( Start.i == End.i )
					{
						break;
					}

					ER += DY;
					Start.i += SX;
				}

				if ( ER2 <= DX )
				{
					if ( Start.j == End.j )
					{
						break;
					}

					ER += DX;
					Start.j += SY;
				}
			}
		}
		else
		{
			Pixel PixelToSet = ScreenBuffer::GetPixelColourMap().ConvertColour( a_Colour );

			while ( true )
			{
				ScreenBuffer::SetPixel( Start, PixelToSet );

				if ( Start.i == End.i && Start.j == End.j )
				{
					break;
				}

				short ER2 = 2 * ER;

				if ( ER2 >= DY )
				{
					if ( Start.i == End.i )
					{
						break;
					}

					ER += DY;
					Start.i += SX;
				}

				if ( ER2 <= DX )
				{
					if ( Start.j == End.j )
					{
						break;
					}

					ER += DX;
					Start.j += SY;
				}
			}
		}
	}
};