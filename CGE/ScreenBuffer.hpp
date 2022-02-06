#pragma once
#include "Rect.hpp"
#include "PixelColourMap.hpp"

class ScreenBuffer
{
public:

    ScreenBuffer() = delete;

    static inline Pixel* GetPixelBuffer()
    {
        return s_PixelBuffer;
    }

    static inline Colour* GetColourBuffer()
    {
        return s_ColourBuffer;
    }

    static inline tVector2< short > GetBufferSize()
    {
        return s_Size;
    }

    static inline short GetBufferArea()
    {
        return s_Size.i * s_Size.j;
    }

    static inline short GetBufferWidth()
    {
        return s_Size.i;
    }

    static inline short GetBufferHeight()
    {
        return s_Size.j;
    }

    static inline Rect GetBufferRect()
    {
        return { { 0, 0 }, { s_Size.i, s_Size.j } };
    }

    static PixelColourMap& GetPixelColourMap()
    {
        return s_PixelColourMap;
    }

    static void SetPixel( tVector2< short > a_Coord, Pixel a_Pixel )
    {
        s_PixelBuffer[ a_Coord.j * s_Size.i + a_Coord.i ] = a_Pixel;
    }

    static void SetPixels( int a_Index, Pixel a_Pixel, short a_Count )
    {
        Pixel* PixelBegin = s_PixelBuffer + a_Index;
        for ( ; a_Count > 0; --a_Count )
        {
            *PixelBegin = a_Pixel;
            ++PixelBegin;
        }
    }

    static void SetPixels( tVector2< short > a_Coord, Pixel a_Pixel, short a_Count )
    {
        Pixel* PixelBegin = s_PixelBuffer + GetIndex( a_Coord );
        for ( ; a_Count > 0; --a_Count )
        {
            *PixelBegin = a_Pixel;
            ++PixelBegin;
        }
    }

    static void SetColour( tVector2< short > a_Coord, Colour a_Colour )
    {
        int Index = GetIndex( a_Coord );

        if ( BlendingEnabled )
        {
            Colour& Background = s_ColourBuffer[ Index ];
            Background += a_Colour;
            s_PixelBuffer[ Index ] = s_PixelColourMap.ConvertColour( Background );
        }
        else
        {
            s_PixelBuffer[ Index ] = s_PixelColourMap.ConvertColour( a_Colour );
            s_ColourBuffer[ Index ] = a_Colour;
        }
    }

    static void SetColours( tVector2< short > a_Coord, Colour a_Colour, short a_Count )
    {
        int Index = GetIndex( a_Coord );
        Pixel PixelToSet = s_PixelColourMap.ConvertColour( a_Colour );
        Pixel* PixelBegin = s_PixelBuffer + Index;
        Colour* ColourBegin = s_ColourBuffer + Index;

        if ( BlendingEnabled )
        {
            for ( ; a_Count > 0; --a_Count )
            {
                *ColourBegin += a_Colour;
                *PixelBegin = s_PixelColourMap.ConvertColour( *ColourBegin );
                ++PixelBegin;
                ++ColourBegin;
            }
        }
        else
        {
            for ( ; a_Count > 0; --a_Count )
            {
                *PixelBegin = PixelToSet;
                *ColourBegin = a_Colour;
                ++PixelBegin;
                ++ColourBegin;
            }
        }
    }

    static void SetColours( int a_Index, Colour a_Colour, short a_Count )
    {
        Pixel PixelToSet = s_PixelColourMap.ConvertColour( a_Colour );
        Pixel* PixelBegin = s_PixelBuffer + a_Index;
        Colour* ColourBegin = s_ColourBuffer + a_Index;

        if ( BlendingEnabled )
        {
            for ( ; a_Count > 0; --a_Count )
            {
                *ColourBegin += a_Colour;
                *PixelBegin = s_PixelColourMap.ConvertColour( *ColourBegin );
                ++PixelBegin;
                ++ColourBegin;
            }
        }
        else
        {
            for ( ; a_Count > 0; --a_Count )
            {
                *PixelBegin = PixelToSet;
                *ColourBegin = a_Colour;
                ++PixelBegin;
                ++ColourBegin;
            }
        }
    }

    static inline void SetBuffer( Pixel a_Pixel )
    {
        SetPixels( { 0, 0 }, a_Pixel, GetBufferArea() );
    }

    static inline void SetBuffer( Colour a_Colour )
    {
        SetColours( { 0, 0 }, a_Colour, GetBufferArea() );
    }

    static void SetRect( const Rect& a_Rect, Pixel a_Pixel )
    {
        int Index = GetIndex( {
            static_cast< short >( a_Rect.Origin.i ),
            static_cast< short >( a_Rect.Origin.j ) } );

        for ( int y = 0; y < a_Rect.Size.j; ++y )
        {
            SetPixels( Index, a_Pixel, a_Rect.Size.i );
            Index += s_Size.i;
        }
    }

    static void SetRect( const Rect& a_Rect, Colour a_Colour )
    {
        int Index = GetIndex( {
            static_cast< short >( a_Rect.Origin.i ),
            static_cast< short >( a_Rect.Origin.j ) } );

        for ( int y = 0; y < a_Rect.Size.j; ++y )
        {
            SetColours( Index, a_Colour, a_Rect.Size.i );
            Index += s_Size.i;
        }
    }

    static inline tVector2< short > GetCoordinate( int a_Index )
    {
        tVector2< short > Coordinate = { 0, static_cast< short >( a_Index ) / s_Size.i };
        Coordinate.i = a_Index - Coordinate.j * s_Size.i;
        return Coordinate;
    }

    static inline int GetIndex( tVector2< short > a_Coord )
    {
        return a_Coord.j * s_Size.i + a_Coord.i;
    }

private:

    static bool Initialize( tVector2< short > a_BufferSize )
    {
        if ( !s_PixelColourMap.Initialize() )
        {
            return false;
        }

        s_PixelBuffer = new Pixel[ static_cast< size_t >( a_BufferSize.i ) * a_BufferSize.j ];
        s_ColourBuffer = new Colour[ static_cast< size_t >( a_BufferSize.i ) * a_BufferSize.j ];
        s_Size = a_BufferSize;
    }

    friend class ConsoleWindow;

    static Pixel*            s_PixelBuffer;
    static Colour*           s_ColourBuffer;
    static tVector2< short > s_Size;
    static PixelColourMap    s_PixelColourMap;

public:

    static bool BlendingEnabled;
};