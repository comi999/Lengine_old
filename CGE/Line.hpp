#pragma once
#include "Math.h"
#include "Rect.hpp"

struct Line
{
	Vector2 Start;
	Vector2 End;

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