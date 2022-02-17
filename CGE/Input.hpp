// Needed for these functions
#define _WIN32_WINNT 0x0500
#include "windows.h"
#include "winuser.h"
#include "wincon.h"

//int getkey()
//{
//
//	while ( true )
//	{
//		// This checks if the window is focused
//		if ( GetForegroundWindow() != GetConsoleWindow() )
//			continue;
//
//		for ( int i = 1; i < 255; ++i )
//		{
//			// The bitwise and selects the function behavior (look at doc)
//			if ( GetAsyncKeyState( i ) & 0x07 )
//				return i;
//
//		}
//
//		Sleep( 250 );
//	}
//}

bool IsKeyDown( int a_Key )
{
	if ( GetForegroundWindow() != GetConsoleWindow() )
		false;

	return GetAsyncKeyState( a_Key ) & 0x07;
}