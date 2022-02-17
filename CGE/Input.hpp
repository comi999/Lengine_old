
#pragma once
#include <Windows.h>

enum class KeyCode : unsigned char
{
	Backspace     = 0x08,
	Tab           = 0x09,
	Enter         = 0x0D,
	Shift         = 0x10,
	Ctrl          = 0x11,
	Alt           = 0x12,
	Caps          = 0x14,
	Space         = 0x20,
	PageUp        = 0x21,
	PageDown      = 0x22,
	End           = 0x23,
	Home          = 0x24,
	Left          = 0x25,
	Up            = 0x26,
	Right         = 0x27,
	Down          = 0x28,
	PrintScrn     = 0x2C,
	Insert        = 0x2D,
	Delete        = 0x2E,
	Zero          = 0x30,
	One           = 0x31,
	Two           = 0x32,
	Three         = 0x33,
	Four          = 0x34,
	Five          = 0x35,
	Six           = 0x36,
	Seven         = 0x37,
	Eight         = 0x38,
	Nine          = 0x39,
	A             = 0x41,
	B             = 0x42,
	C             = 0x43,
	D             = 0x44,
	E             = 0x45,
	F             = 0x46,
	G             = 0x47,
	H             = 0x48,
	I             = 0x49,
	J             = 0x4A,
	K             = 0x4B,
	L             = 0x4C,
	M             = 0x4D,
	N             = 0x4E,
	O             = 0x4F,
	P             = 0x50,
	Q             = 0x51,
	R             = 0x52,
	S             = 0x53,
	T             = 0x54,
	U             = 0x55,
	V             = 0x56,
	W             = 0x57,
	X             = 0x58,
	Y             = 0x59,
	Z             = 0x5A,
	F1            = 0x70,
	F2            = 0x71,
	F3            = 0x72,
	F4            = 0x73,
	F5            = 0x74,
	F6            = 0x75,
	F7            = 0x76,
	F8            = 0x77,
	F9            = 0x78,
	F10           = 0x79,
	F11           = 0x7A,
	F12           = 0x7B,
	F13           = 0x7C,
	F14           = 0x7D,
	F15           = 0x7E,
	F16           = 0x7F,
	F17           = 0x80,
	F18           = 0x81,
	F19           = 0x82,
	F20           = 0x83,
	F21           = 0x84,
	F22           = 0x85,
	F23           = 0x86,
	F24           = 0x87,
	NumLock       = 0x90,
	ScrollLock    = 0x91,
	LeftShift     = 0xA0,
	RightShift    = 0xA1,
	LeftCtrl      = 0xA2,
	RightCtrl     = 0xA3,
	LeftMenu      = 0xA4,
	RightMenu     = 0xA5,
	Semicolon     = 0xBA,
	Plus          = 0xBB,
	Comma         = 0xBC,
	Minus         = 0xBD,
	Period        = 0xBE,
	Forwardslash  = 0xBF,
	Tilda         = 0xC0,
	LeftBracket   = 0xDB,
	Backslash     = 0xDC,
	RightBracket  = 0xDD,
	InvertedComma = 0xDE
	
};

enum class MouseButton
{
	LeftMouse = 0x01,
	RightMouse = 0x02,
	MiddleMouse = 0x04,
};

class Input
{
public:

	static bool IsKeyDown( KeyCode a_KeyCode )
	{
		return GetKeyState( static_cast< int >( a_KeyCode ) ) & 0x8000;
	}

	static bool IsKeyUp( KeyCode a_KeyCode )
	{
		return GetKeyState( static_cast< int >( a_KeyCode ) ) & 0x8000;
	}

	static bool IsMouseButtonDown( MouseButton a_MouseButton )
	{
		return GetKeyState( static_cast< int >( a_MouseButton ) ) & 0x8000;
	}

	static bool IsMouseButtonUp( MouseButton a_MouseButton )
	{
		return GetKeyState( static_cast< int >( a_MouseButton ) ) & 0x8000;
	}
};