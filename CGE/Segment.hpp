#pragma once
#include "Math.hpp"
#include "Rect.hpp"

struct Segment
{
	Vector2 Start;
	Vector2 End;

	// This is just a test. remove later
	inline static bool FindClipParams( Vector4* a_P0, Vector4* a_P1, float* o_T, uint32_t* o_C )
	{
		static constexpr uint8_t _Inside = 0b000000;
		static constexpr uint8_t _Left = 0b000001;
		static constexpr uint8_t _Right = 0b000010;
		static constexpr uint8_t _Bottom = 0b000100;
		static constexpr uint8_t _Top = 0b001000;
		static constexpr uint8_t _Front = 0b010000;
		static constexpr uint8_t _Back = 0b100000;
		static constexpr int8_t  _XMin = -1;
		static constexpr int8_t  _YMin = -1;
		static constexpr int8_t  _ZMin = -0;
		static constexpr int8_t  _XMax = +1;
		static constexpr int8_t  _YMax = +1;
		static constexpr int8_t  _ZMax = +1;

		static constexpr auto ComputeCode = []( float* a_P )
		{
			uint8_t Code = _Inside;
			if ( a_P[ 0 ] < _XMin ) Code |= _Left;
			else if ( a_P[ 0 ] > _XMax ) Code |= _Right;
			else if ( a_P[ 1 ] < _YMin ) Code |= _Bottom;
			else if ( a_P[ 1 ] > _YMax ) Code |= _Top;
			else if ( a_P[ 2 ] < _ZMin ) Code |= _Back;
			else if ( a_P[ 2 ] > _ZMax ) Code |= _Front;
			return Code;
		};

		uint8_t CodeA = ComputeCode( &a_P0->x );
		uint8_t CodeB = ComputeCode( &a_P1->x );
		Vector3 Temp;
		*o_C = 0;
		float T = 1.0f;

		while ( true )
		{
			// Entire segment is inside bounding volume. Trivial. Accept.
			if ( !( CodeA | CodeB ) ) return *o_C;

			// Entire segment exists in one outside zone. Reject completely.
			if ( CodeA & CodeB ) return *o_C;

			// Pick one of the outsides.
			uint8_t CodeC = CodeA > CodeB ? CodeA : CodeB;

			// Needs clipping
			if ( CodeC & _Top )
			{
				o_T[ ( *o_C )++ ] = T = T * ( _YMax - a_P0->y ) / ( a_P1->y - a_P0->y );
				Temp.x = a_P0->x + ( a_P1->x - a_P0->x ) * T;
				Temp.y = _YMax;
				Temp.z = a_P0->z + ( a_P1->z - a_P0->z ) * T;
			}
			else if ( CodeC & _Bottom )
			{
				o_T[ ( *o_C )++ ] = T = T * ( _YMin - a_P0->y ) / ( a_P1->y - a_P0->y );
				Temp.x = a_P0->x + ( a_P1->x - a_P0->x ) * T;
				Temp.y = _YMin;
				Temp.z = a_P0->z + ( a_P1->z - a_P0->z ) * T;
			}
			else if ( CodeC & _Right )
			{
				o_T[ ( *o_C )++ ] = T = T * ( _XMax - a_P0->x ) / ( a_P1->x - a_P0->x );
				Temp.x = _XMax;
				Temp.y = a_P0->y + ( a_P1->y - a_P0->y ) * T;
				Temp.z = a_P0->z + ( a_P1->z - a_P0->z ) * T;
			}
			else if ( CodeC & _Left )
			{
				o_T[ ( *o_C )++ ] = T = T * ( _XMin - a_P0->x ) / ( a_P1->x - a_P0->x );
				Temp.x = _XMin;
				Temp.y = a_P0->y + ( a_P1->y - a_P0->y ) * T;
				Temp.z = a_P0->z + ( a_P1->z - a_P0->z ) * T;
			}
			else if ( CodeC & _Front )
			{
				o_T[ ( *o_C )++ ] = T = T * ( _ZMax - a_P0->z ) / ( a_P1->z - a_P0->z );
				Temp.x = a_P0->x + ( a_P1->x - a_P0->x ) * T;
				Temp.y = a_P0->y + ( a_P1->y - a_P0->y ) * T;
				Temp.z = _ZMax;
			}
			else if ( CodeC & _Front )
			{
				o_T[ ( *o_C )++ ] = T = T * ( _ZMin - a_P0->z ) / ( a_P1->z - a_P0->z );
				Temp.x = a_P0->x + ( a_P1->x - a_P0->x ) * T;
				Temp.y = a_P0->y + ( a_P1->y - a_P0->y ) * T;
				Temp.z = _ZMin;
			}

			if ( CodeC == CodeA )
			{
				a_P0->x = Temp.x;
				a_P0->y = Temp.y;
				a_P0->z = Temp.z;
				CodeA = ComputeCode( &a_P0->x );
			}
			else
			{
				a_P1->x = Temp.x;
				a_P1->y = Temp.y;
				a_P1->z = Temp.z;
				CodeB = ComputeCode( &a_P1->x );
			}
		}
	}

	bool RectClamp( Rect a_Rect )
	{
		const int INSIDE = 0; // 0000
		const int LEFT = 1;   // 0001
		const int RIGHT = 2;  // 0010
		const int BOTTOM = 4; // 0100
		const int TOP = 8;    // 1000
		int xmin = a_Rect.GetLeft();
		int xmax = a_Rect.GetRight();
		int ymax = a_Rect.GetBottom();
		int ymin = a_Rect.GetTop();

		// Cohen–Sutherland clipping algorithm clips a line from
		// P0 = (x0, y0) to P1 = (x1, y1) against a rectangle with 
		// diagonal from (xmin, ymin) to (xmax, ymax).
		auto CohenSutherlandLineClipAndDraw = [&]( float& x0, float& y0, float& x1, float& y1 )
		{
			auto ComputeOutCode = [&]( double x, double y )
			{
				int code;

				code = INSIDE;          // initialised as being inside of [[clip window]]

				if ( x < xmin )           // to the left of clip window
					code |= LEFT;
				else if ( x > xmax )      // to the right of clip window
					code |= RIGHT;
				if ( y < ymin )           // below the clip window
					code |= BOTTOM;
				else if ( y > ymax )      // above the clip window
					code |= TOP;

				return code;
			};

			// compute outcodes for P0, P1, and whatever point lies outside the clip rectangle
			int outcode0 = ComputeOutCode( x0, y0 );
			int outcode1 = ComputeOutCode( x1, y1 );
			bool accept = false;

			while ( true )
			{
				if ( !( outcode0 | outcode1 ) )
				{
					// bitwise OR is 0: both points inside window; trivially accept and exit loop
					accept = true;
					break;
				}
				else if ( outcode0 & outcode1 )
				{
					// bitwise AND is not 0: both points share an outside zone (LEFT, RIGHT, TOP,
					// or BOTTOM), so both must be outside window; exit loop (accept is false)
					break;
				}
				else
				{
					// failed both tests, so calculate the line segment to clip
					// from an outside point to an intersection with clip edge
					float x, y;

					// At least one endpoint is outside the clip rectangle; pick it.
					int outcodeOut = outcode1 > outcode0 ? outcode1 : outcode0;

					// Now find the intersection point;
					// use formulas:
					//   slope = (y1 - y0) / (x1 - x0)
					//   x = x0 + (1 / slope) * (ym - y0), where ym is ymin or ymax
					//   y = y0 + slope * (xm - x0), where xm is xmin or xmax
					// No need to worry about divide-by-zero because, in each case, the
					// outcode bit being tested guarantees the denominator is non-zero
					if ( outcodeOut & TOP )
					{           // point is above the clip window
						x = x0 + ( x1 - x0 ) * ( ymax - y0 ) / ( y1 - y0 );
						y = ymax;
					}
					else if ( outcodeOut & BOTTOM )
					{ // point is below the clip window
						x = x0 + ( x1 - x0 ) * ( ymin - y0 ) / ( y1 - y0 );
						y = ymin;
					}
					else if ( outcodeOut & RIGHT )
					{  // point is to the right of clip window
						y = y0 + ( y1 - y0 ) * ( xmax - x0 ) / ( x1 - x0 );
						x = xmax;
					}
					else if ( outcodeOut & LEFT )
					{   // point is to the left of clip window
						y = y0 + ( y1 - y0 ) * ( xmin - x0 ) / ( x1 - x0 );
						x = xmin;
					}

					// Now we move outside point to intersection point to clip
					// and get ready for next pass.
					if ( outcodeOut == outcode0 )
					{
						x0 = x;
						y0 = y;
						outcode0 = ComputeOutCode( x0, y0 );
					}
					else
					{
						x1 = x;
						y1 = y;
						outcode1 = ComputeOutCode( x1, y1 );
					}
				}
			}
		};

		float X0 = Start.x;
		float X1 = End.x;
		float Y0 = Start.y;
		float Y1 = End.y;

		CohenSutherlandLineClipAndDraw( Start.x, Start.y, End.x, End.y );
		return true;
	}
};