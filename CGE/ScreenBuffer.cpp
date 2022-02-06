#include "ScreenBuffer.hpp"
#include "Pixel.hpp"
#include "Colour.h"
#include "Math.h"
#include "PixelColourMap.hpp"

Pixel*            ScreenBuffer::s_PixelBuffer = nullptr;
Colour*           ScreenBuffer::s_ColourBuffer = nullptr;
tVector2< short > ScreenBuffer::s_Size;
PixelColourMap    ScreenBuffer::s_PixelColourMap;
bool              ScreenBuffer::BlendingEnabled = false;