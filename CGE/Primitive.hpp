#pragma once
#include "Colour.h"
#include "Math.h"
#include "CGE.h"

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

	//static void DrawLine( Vector2 a_Start, Vector2 a_End, Colour a_Colour )
	//{
	//	float MaxX = ScreenBuffer::GetBufferWidth() - 1;
	//	float MaxY = ScreenBuffer::GetBufferHeight() - 1;
	//	Clamp( a_Start.i, 0.0f, MaxX );
	//	Clamp( a_Start.j, 0.0f, MaxY );
	//	Clamp( a_End.i, 0.0f, MaxX );
	//	Clamp( a_End.j, 0.0f, MaxY );

	//	// Invert Y.
	//	a_Start.j = MaxY - a_Start.j;
	//	a_End.j = MaxY - a_End.j;

	//	tVector2< short > Start = { static_cast< short >( a_Start.i ), static_cast< short >( a_Start.j ) };
	//	tVector2< short > End = { static_cast< short >( a_End.i ), static_cast< short >( a_End.j ) };

	//	if ( Start.i == End.i )
	//	{
	//		return;
	//	}

	//	short DX = abs( a_Start.i - a_End.i );
	//	short DY = -abs( a_Start.j - a_End.j );
	//	short SX = a_Start.i < a_End.i ? 1 : -1;
	//	short SY = a_Start.j < a_End.j ? 1 : -1;
	//	short ER = DX + DY;

	//	if ( ScreenBuffer::BlendingEnabled )
	//	{
	//		while ( true )
	//		{
	//			ScreenBuffer::SetColour( Start, a_Colour );

	//			if ( Start.i == End.i && Start.j == End.j )
	//			{
	//				break;
	//			}

	//			short E2 = 2 * ER;

	//			if ( E2 >= DY )
	//			{
	//				ER += DY;
	//				Start.i += SX;
	//			}

	//			if ( E2 <= DX )
	//			{
	//				ER += DX;
	//				Start.j += SY;
	//			}
	//		}
	//	}
	//	else
	//	{
	//		Pixel PixelToSet = ScreenBuffer::GetPixelColourMap().ConvertColour( a_Colour );

	//		while ( true )
	//		{
	//			ScreenBuffer::SetPixel( Start, PixelToSet );

	//			if ( ( Start.i > End.i && Start.j > End.j ) || ( Start.i < End.i && Start.j < End.j ) )
	//			{
	//				break;
	//			}

	//			short E2 = 2 * ER;

	//			if ( E2 >= DY )
	//			{
	//				ER += DY;
	//				Start.i += SX;
	//			}

	//			if ( E2 <= DX )
	//			{
	//				ER += DX;
	//				Start.j += SY;
	//			}
	//		}
	//	}
	//}

	static void DrawLine( Vector2 a_Start, Vector2 a_End, Colour a_Colour )
	{
		tVector2< short > Start = { static_cast< short >( a_Start.i ), static_cast< short >( a_Start.j ) };
		tVector2< short > End   = { static_cast< short >( a_End.i   ), static_cast< short >( a_End.j   ) };
		short MaxX = ScreenBuffer::GetBufferWidth()  - 1;
		short MaxY = ScreenBuffer::GetBufferHeight() - 1;

		//Clamp( Start.i, ( short )0, MaxX );
		//Clamp( Start.j, ( short )0, MaxY );
		//Clamp( End.i,   ( short )0, MaxX );
		//Clamp( End.j,   ( short )0, MaxY );



		// Invert Y.
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

	static void DrawLine( const Point& a_Start, const Point& a_End )
	{
		tVector2< short > Start = { static_cast< short >( a_Start.Position.i ), static_cast< short >( a_Start.Position.j ) };
		tVector2< short > End   = { static_cast< short >( a_End.Position.i   ), static_cast< short >( a_End.Position.j   ) };
		short MaxX = ScreenBuffer::GetBufferWidth()  - 1;
		short MaxY = ScreenBuffer::GetBufferHeight() - 1;

		//Clamp( Start.i, ( short )0, MaxX );
		//Clamp( Start.j, ( short )0, MaxY );
		//Clamp( End.i,   ( short )0, MaxX );
		//Clamp( End.j,   ( short )0, MaxY );

		if ( Start.i < 0 )
		{

		}
		else if ( Start.i > MaxX )
		{

		}

		if ( Start.j < 0 )
		{

		}
		else if ( Start.j > MaxY )
		{

		}

		if ( End.i < 0 )
		{

		}
		else if ( End.i > MaxX )
		{

		}

		if ( End.j < 0 )
		{

		}
		else if ( End.j > MaxY )
		{

		}

		// Invert Y.
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