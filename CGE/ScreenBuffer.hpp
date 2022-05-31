#pragma once
#include "Rect.hpp"
#include "PixelColourMap.hpp"

class ScreenBuffer
{
public:

    void Initialize( Vector< short, 2 > a_BufferSize )
    {
        m_FrontBuffer = new Pixel[ static_cast< size_t >( a_BufferSize.x ) * a_BufferSize.y ];
        m_BackBuffer = new Pixel[ static_cast< size_t >( a_BufferSize.x ) * a_BufferSize.y ];;
        m_ColourBuffer = new Colour[ static_cast< size_t >( a_BufferSize.x ) * a_BufferSize.y ];
        m_Size = a_BufferSize;
    }

    inline Pixel* GetPixelBuffer()
    {
        return m_BackBuffer;
    }

    inline Colour* GetColourBuffer()
    {
        return m_ColourBuffer;
    }

    inline Vector< short, 2 > GetSize()
    {
        return m_Size;
    }

    inline short GetArea()
    {
        return m_Size.x * m_Size.y;
    }

    inline short GetWidth()
    {
        return m_Size.x;
    }

    inline short GetHeight()
    {
        return m_Size.y;
    }

    inline Rect GetBufferRect()
    {
        return { { 0, 0 }, { m_Size.x, m_Size.y } };
    }

    void SetPixel( Vector< short, 2 > a_Coord, Pixel a_Pixel )
    {//These early outs can be removed when rendering pipeline is done.
        if ( a_Coord.x < 0 || a_Coord.x >= GetWidth() )
        {
            return;
        }

        if ( a_Coord.y < 0 || a_Coord.y >= GetHeight() )
        {
            return;
        }

        m_BackBuffer[ a_Coord.y * m_Size.x + a_Coord.x ] = a_Pixel;
    }

    void SetPixels( int a_Index, Pixel a_Pixel, short a_Count )
    {
        Pixel* PixelBegin = m_BackBuffer + a_Index;
        for ( ; a_Count > 0; --a_Count )
        {
            *PixelBegin = a_Pixel;
            ++PixelBegin;
        }
    }

    void SetPixels( Vector< short, 2 > a_Coord, Pixel a_Pixel, short a_Count )
    {
        Pixel* PixelBegin = m_BackBuffer + GetIndex( a_Coord );
        for ( ; a_Count > 0; --a_Count )
        {
            *PixelBegin = a_Pixel;
            ++PixelBegin;
        }
    }

    void SetColour( Vector< short, 2 > a_Coord, Colour a_Colour )
    { // Remove these when rendering pipeline clipping is complete.
        if ( a_Coord.x < 0 || a_Coord.x >= GetWidth() || a_Coord.y < 0 || a_Coord.y >= GetHeight() ) return;
        int Index = GetIndex( a_Coord );


        // Remove this when blending is done through pipeline.
        if ( BlendingEnabled )
        {
            Colour& Background = m_ColourBuffer[ Index ];
            Background += a_Colour;
            m_BackBuffer[ Index ] = PixelColourMap::Get().ConvertColour( Background );
        }
        else
        {
            m_BackBuffer[ Index ] = PixelColourMap::Get().ConvertColour( a_Colour );
            m_ColourBuffer[ Index ] = a_Colour;
        }
    }

    void SetColours( Vector< short, 2 > a_Coord, Colour a_Colour, short a_Count )
    {
        int Index = GetIndex( a_Coord );
        Pixel PixelToSet = PixelColourMap::Get().ConvertColour( a_Colour );
        Pixel* PixelBegin = m_BackBuffer + Index;
        Colour* ColourBegin = m_ColourBuffer + Index;

        if ( BlendingEnabled )
        {
            for ( ; a_Count > 0; --a_Count )
            {
                *ColourBegin += a_Colour;
                *PixelBegin = PixelColourMap::Get().ConvertColour( *ColourBegin );
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

    void SetColours( int a_Index, Colour a_Colour, short a_Count )
    {
        Pixel PixelToSet = PixelColourMap::Get().ConvertColour( a_Colour );
        Pixel* PixelBegin = m_BackBuffer + a_Index;
        Colour* ColourBegin = m_ColourBuffer + a_Index;

        if ( BlendingEnabled )
        {
            for ( ; a_Count > 0; --a_Count )
            {
                *ColourBegin += a_Colour;
                *PixelBegin = PixelColourMap::Get().ConvertColour( *ColourBegin );
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

    inline void SetBuffer( Pixel a_Pixel )
    {
        SetPixels( { 0, 0 }, a_Pixel, GetArea() );
    }

    inline void SetBuffer( Colour a_Colour )
    {
        SetColours( { 0, 0 }, a_Colour, GetArea() );
    }

    void SetRect( const Rect& a_Rect, Pixel a_Pixel )
    {
        int Index = GetIndex( {
            static_cast< short >( a_Rect.Origin.x ),
            static_cast< short >( a_Rect.Origin.y ) } );

        for ( int y = 0; y < a_Rect.Size.y; ++y )
        {
            SetPixels( Index, a_Pixel, a_Rect.Size.x );
            Index += m_Size.x;
        }
    }

    void SetRect( const Rect& a_Rect, Colour a_Colour )
    {
        int Index = GetIndex( {
            static_cast< short >( a_Rect.Origin.x ),
            static_cast< short >( a_Rect.Origin.y ) } );

        for ( int y = 0; y < a_Rect.Size.y; ++y )
        {
            SetColours( Index, a_Colour, a_Rect.Size.x );
            Index += m_Size.x;
        }
    }

    void SwapPixelBuffer()
    {
        std::swap( m_BackBuffer, m_FrontBuffer );
    }

    inline Vector< short, 2 > GetCoordinate( int a_Index )
    {
        Vector< short, 2 > Coordinate = { 0, static_cast< short >( a_Index ) / m_Size.x };
        Coordinate.x = a_Index - Coordinate.y * m_Size.y;
        return Coordinate;
    }

    inline int GetIndex( Vector< short, 2 > a_Coord )
    {
        return a_Coord.y * m_Size.x + a_Coord.x;
    }

private:

    friend class ConsoleWindow;

    Pixel*             m_BackBuffer;
    Pixel*             m_FrontBuffer;
    Colour*            m_ColourBuffer;
    Vector< short, 2 > m_Size;

public:

    static bool BlendingEnabled;
};