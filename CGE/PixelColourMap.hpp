#pragma once
#include <fstream>
#include "Colour.h"
#include "Pixel.hpp"
#include "Math.h"

using namespace std;

class PixelColourMap
{
public:

	PixelColourMap()
		: m_PixelMap( new Pixel[ 16777216 ] )
	{ }

	~PixelColourMap()
	{
		delete[] m_PixelMap;
	}

	bool Initialize()
	{
		if ( Load() )
		{
			return true;
		}

		return BuildAndSave();
	}

	void Build()
	{
		auto Convert = []( Colour a_Colour )
		{
			return 
				static_cast< int >( a_Colour.R ) +
				static_cast< int >( a_Colour.G ) * 256 +
				static_cast< int >( a_Colour.B ) * 65536;
		};

		// Set initial colours.
		for ( int i = 0; i < 16; ++i )
		{
			Pixel NewPixel;
			NewPixel.SetForegroundColour( ConsoleColours[ i ] );
			NewPixel.Unicode() = L'\x2588'; // Block
			m_PixelMap[ Convert( SeedColours[ i ] ) ] = NewPixel;
		}

		size_t Index = 16;

		// Set remaining colours.
		for ( int i = 0; i < 16; ++i )
		{
			Colour Background = SeedColours[ i ];
			Pixel NewPixel;

			for ( int j = i + 1; j < 16; ++j )
			{
				Colour Foreground = SeedColours[ j ];

				for ( int k = 1; k < 4; ++k )
				{
					// Set alpha.
					Foreground.A = ( k - 1 ) * 64 + 63;
					
					// Create and set Colour Seed.
					Colour& SeedColour = SeedColours[ Index++ ];
					SeedColour = Background + Foreground;

					// Set Pixel.
					NewPixel.SetBackgroundColour( ConsoleColours[ i ] );
					NewPixel.SetForegroundColour( ConsoleColours[ j ] );
					NewPixel.Unicode() = L'\x2590' + k; // Dithering characters.
					m_PixelMap[ Convert( SeedColour ) ] = NewPixel;
				}
			}
		}

		// Extrapolate to empty regions in cube.
		for ( int B = 0; B < 256; ++B )
		{
			for ( int G = 0; G < 256; ++G )
			{
				for ( int R = 0; R < 256; ++R )
				{
					tVector3< int > DiffVector;
					int MinDistSqrd = 16777216;
					Pixel& Current = m_PixelMap[ Convert( Colour( R, G, B ) ) ];
					Pixel Closest;

					for ( Colour Seed : SeedColours )
					{
						DiffVector.i = R - Seed.R;
						DiffVector.j = G - Seed.G;
						DiffVector.k = B - Seed.B;
						int DistSqrd = DiffVector * DiffVector;

						if ( DistSqrd == 0 )
						{
							Closest = Current;
							break;
						}

						if ( DistSqrd < MinDistSqrd )
						{
							MinDistSqrd = DistSqrd;
							Closest = m_PixelMap[ Convert( Seed ) ];
						}
					}

					Current = Closest;
				}
			}
		}
	}

	bool BuildAndSave()
	{
		Build();
		return Save();
	}

	bool Load()
	{
		fstream File;
		File.open( "colours.map", ios::binary | ios::in );

		if ( !File.is_open() )
		{
			return false;
		}

		File.read( reinterpret_cast< char* >( m_PixelMap ), 16777216 * sizeof( Pixel ) );
		File.close();
		return true;
	}

	bool Save()
	{
		fstream File;
		File.open( "colours.map", ios::binary | ios::out );

		if ( !File.is_open() )
		{
			return false;
		}

		File.write( reinterpret_cast< char* >( m_PixelMap ), 16777216 * sizeof( Pixel ) );
		File.close();
		return true;
	}

	Pixel ConvertColour( Colour a_Colour )
	{
		return m_PixelMap[
			static_cast< int >( a_Colour.R ) +
			static_cast< int >( a_Colour.G ) * 256 +
			static_cast< int >( a_Colour.B ) * 65536 ];
	}

	static Colour SeedColours[ 376 ];

private:

	Pixel* m_PixelMap;
};