#include "CGE.h"
#include "Primitive.hpp"
#include "entt/entt.hpp"

using namespace std;

int main()
{
	CGE::Initialize( "Some window!", { 128, 128 }, { 1, 1 } );
	
	/*for ( int x = 0; x < 256; x += 1 )
	{
		Sleep( 25 );
		for ( int y = 0; y < 128; ++y )
		{
			for ( int z = 0; z < 128; ++z )
			{
				ScreenBuffer::SetPixel( { short(y), short(z) }, ScreenBuffer::GetPixelColourMap().ConvertColour( Colour( x, y * 2, z * 2 ) ) );
			}
		}
		ConsoleWindow::WriteBuffer();

		if ( x == 252 )
		{
			x = 0;
		}
	}*/

	/*Rect rect1 = { { 1, 1 }, { 45, 20 } };
	Rect rect2 = { { 5, 15 }, { 45, 100 } };
	ScreenBuffer::BlendingEnabled = true;
	float f = 0.0f;
	while ( true )
	{
		f += 0.1f;
		ScreenBuffer::SetBuffer( Colour::WHITE );
		ScreenBuffer::SetRect( rect1, Colour::YELLOW );
		ScreenBuffer::SetRect( rect2, Colour( Colour::DARK_BLUE, 32 ) );
		Primitive::DrawLine( { 64, 64 }, { sin( f ) * 90 + 64, cos( f ) * 90 + 64 }, Colour( Colour::DARK_RED, 128 ) );
		ConsoleWindow::WriteBuffer();
	}*/

	
	
	
	return 0;
}