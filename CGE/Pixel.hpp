#pragma once
#include <Windows.h>
#include "ConsoleColour.hpp"

struct Pixel : protected CHAR_INFO
{
	Pixel() : CHAR_INFO()
	{ }

	inline Pixel( const CHAR_INFO& a_CharInfo )
	{
		*reinterpret_cast< CHAR_INFO* >( this ) = a_CharInfo;
	}

	inline CHAR& Ascii()
	{
		return Char.AsciiChar;
	}

	inline WCHAR& Unicode()
	{
		return Char.UnicodeChar;
	}

	inline WORD& Attributes()
	{
		return CHAR_INFO::Attributes;
	}

	inline ConsoleColour GetForegroundColour()
	{
		return static_cast< ConsoleColour >( 0xF & CHAR_INFO::Attributes );
	}

	inline ConsoleColour GetBackgroundColour()
	{
		return static_cast< ConsoleColour >( ( 0xF0 & CHAR_INFO::Attributes ) >> 4 );
	}

	inline void SetForegroundColour( ConsoleColour a_ConsoleColour )
	{
		CHAR_INFO::Attributes &= 0xFFF0;
		CHAR_INFO::Attributes |= static_cast< WORD >( a_ConsoleColour );
	}

	inline void SetBackgroundColour( ConsoleColour a_ConsoleColour )
	{
		CHAR_INFO::Attributes &= 0xFF0F;
		CHAR_INFO::Attributes |= static_cast< WORD >( a_ConsoleColour ) << 4;
	}

	inline operator CHAR_INFO& ( )
	{
		return *reinterpret_cast< CHAR_INFO* >( this );
	}

private:

	friend class ConsoleWindow;
};