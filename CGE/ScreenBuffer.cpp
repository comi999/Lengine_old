#include "ScreenBuffer.hpp"
#include "Pixel.hpp"
#include "Colour.hpp"
#include "Math.hpp"
#include "PixelColourMap.hpp"

Pixel*             ScreenBuffer::s_PixelBuffer = nullptr;
Colour*            ScreenBuffer::s_ColourBuffer = nullptr;
Vector< short, 2 > ScreenBuffer::s_Size;
PixelColourMap     ScreenBuffer::s_PixelColourMap;
bool               ScreenBuffer::BlendingEnabled = false;