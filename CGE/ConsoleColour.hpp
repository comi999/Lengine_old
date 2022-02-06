#pragma once

enum class ConsoleColour : char
{
	BLACK        = 0x0000,
	DARK_BLUE    = 0x0001,
	DARK_GREEN   = 0x0002,
	DARK_RED     = 0x0004,
	DARK_GREY    = 0x0008,
	DARK_CYAN    = DARK_BLUE  | DARK_GREEN,
	DARK_MAGENTA = DARK_BLUE  | DARK_RED,
	DARK_YELLOW  = DARK_GREEN | DARK_RED,
	BLUE         = DARK_GREY  | DARK_BLUE,
	GREEN        = DARK_GREY  | DARK_GREEN,
	RED          = DARK_GREY  | DARK_RED,
	GREY         = DARK_CYAN  | DARK_RED,
	CYAN         = DARK_GREY  | DARK_CYAN,
	MAGENTA      = DARK_GREY  | DARK_MAGENTA,
	YELLOW       = DARK_GREY  | DARK_YELLOW,
	WHITE        = BLUE       | DARK_YELLOW
}; 

const ConsoleColour ConsoleColours[ 16 ]
{
	ConsoleColour::BLACK,
	ConsoleColour::DARK_BLUE,
	ConsoleColour::DARK_GREEN,
	ConsoleColour::DARK_CYAN,
	ConsoleColour::DARK_RED,
	ConsoleColour::DARK_MAGENTA,
	ConsoleColour::DARK_YELLOW,
	ConsoleColour::GREY,
	ConsoleColour::DARK_GREY,
	ConsoleColour::BLUE,
	ConsoleColour::GREEN,
	ConsoleColour::CYAN,
	ConsoleColour::RED,
	ConsoleColour::MAGENTA,
	ConsoleColour::YELLOW,
	ConsoleColour::WHITE
};