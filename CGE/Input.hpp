
#pragma once
//#define _WIN32_WINNT 0x0500
#include "windows.h"
#include "winuser.h"
#include "wincon.h"


class Input
{

public:

	static void Tick()
	{
		Result = GetRawInputBuffer( InputBuffer, &RawInputSize, sizeof( RAWINPUTHEADER ) );
	}

	static void Initialize()
	{
		Result = GetRawInputBuffer( nullptr, &RawInputSize, sizeof( RAWINPUTHEADER ) );
		InputBuffer = new RAWINPUT[ RawInputSize ];
		

		//Result = GetRawInputBuffer( NULL, &( RawInputSize ), sizeof( RAWINPUTHEADER ) );

		//if ( Result == -1 )
		//{
		//}
		//else if ( Result == 0 && RawInputSize != 0 )
		//{
		//	UINT AllocatedBufferByteCount = RawInputSize * 16;
		//	RAWINPUT* RawInputBuffer = reinterpret_cast< RAWINPUT* >( malloc( AllocatedBufferByteCount ) );

		//	UINT AllocatedBufferByteCountTwo = AllocatedBufferByteCount;
		//	Result = GetRawInputBuffer( RawInputBuffer, &( AllocatedBufferByteCountTwo ), sizeof( RAWINPUTHEADER ) );
		//	if ( Result == -1 )
		//	{
		//	}
		//	else if ( Result != 0 )
		//	{
		//		UINT RawInputCount = Result;

		//		DWORD MouseDeltaX = 0;
		//		DWORD MouseDeltaY = 0;

		//		bool ButtonsPressed[ 2 ] = { false, false };

		//		RAWINPUT* RawInput = RawInputBuffer;
		//		for ( unsigned int i = 0; i < RawInputCount; ++i )
		//		{
		//			switch ( RawInput->header.dwType )
		//			{
		//				// Raw mouse movement data for high-resolution mice. 
		//			case RIM_TYPEMOUSE:
		//			{
		//				MouseDeltaX += RawInput->data.mouse.lLastX;
		//				MouseDeltaY += RawInput->data.mouse.lLastY;

		//				ButtonsPressed[ 0 ] = ( ( RawInput->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN ) == RI_MOUSE_LEFT_BUTTON_DOWN );
		//				ButtonsPressed[ 1 ] = ( ( RawInput->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN ) == RI_MOUSE_RIGHT_BUTTON_DOWN );
		//			}
		//			break;
		//			}

		//			RawInput += 1;
		//		}
		//		DefRawInputProc( &( RawInputBuffer ), RawInputCount, sizeof( RAWINPUTHEADER ) );

		//		if ( ButtonsPressed[ 0 ] )
		//		{
		//		}
		//		if ( ButtonsPressed[ 1 ] )
		//		{
		//		}
		//	}

		//	free( RawInputBuffer );
		//}



	}

private:

	static UINT RawInputSize;
	static UINT Result;
	static PRAWINPUT InputBuffer;
};