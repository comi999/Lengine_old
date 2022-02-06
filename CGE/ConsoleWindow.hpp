#pragma once
#include <Windows.h>
#include "Math.h"
#include "Colour.h"
#include "ScreenBuffer.hpp"
#include "PixelColourMap.hpp"

class ConsoleWindow
{
public:

    typedef HANDLE ConsoleHandle;
    typedef HWND   WindowHandle;
    typedef SMALL_RECT WindowRegion;

    static void SetTitle( const char* a_Title )
    {
        size_t Length = strlen( a_Title ) + 1;
        Length = Length > 64 ? 64 : Length;
        mbstowcs_s( nullptr, s_Title, Length, a_Title, Length );
        SetConsoleTitle( s_Title );
    }

    static inline tVector2< int > GetWindowSize()
    {
        return ScreenBuffer::GetBufferSize();
    }

    static inline short GetWindowArea()
    {
        return ScreenBuffer::GetBufferArea();
    }

    static inline short GetWindowWidth()
    {
        return ScreenBuffer::GetBufferWidth();
    }

    static inline short GetWindowHeight()
    {
        return ScreenBuffer::GetBufferHeight();
    }

    static inline tVector2< short > GetPixelSize()
    {
        return s_PixelSize;
    }

    static inline short GetPixelWidth()
    {
        return s_PixelSize.i;
    }

    static inline short GetPixelHeight()
    {
        return s_PixelSize.j;
    }

    static inline ConsoleHandle GetConsoleHandle()
    {
        return s_ConsoleHandle;
    }

    static inline WindowHandle GetWindowHandle()
    {
        return s_WindowHandle;
    }

private:

    static bool Initialize( const char* a_Title, tVector2< short > a_WindowSize, tVector2< short > a_PixelSize )
    {
        // If cannot create console window, return false.
        if ( !AllocConsole() )
        {
            return false;
        }

        // Retrieve handles for console window.
        s_ConsoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );
        s_WindowHandle = GetConsoleWindow();

        // Set console font.
        a_PixelSize.i = a_PixelSize.i > 8 ? a_PixelSize.i : 8;
        a_PixelSize.j = a_PixelSize.j > 8 ? a_PixelSize.j : 8;
        CONSOLE_FONT_INFOEX FontInfo;
        FontInfo.cbSize = sizeof( FontInfo );
        FontInfo.nFont = 0;
        FontInfo.dwFontSize = { a_PixelSize.i, a_PixelSize.j };
        FontInfo.FontFamily = FF_DONTCARE;
        FontInfo.FontWeight = FW_NORMAL;
        wcscpy_s( FontInfo.FaceName, L"Terminal" );
        SetCurrentConsoleFontEx( s_ConsoleHandle, false, &FontInfo );
        s_PixelSize = a_PixelSize;

        // Get screen buffer info object.
        CONSOLE_SCREEN_BUFFER_INFOEX ScreenBufferInfo;
        ScreenBufferInfo.cbSize = sizeof( ScreenBufferInfo );
        GetConsoleScreenBufferInfoEx( s_ConsoleHandle, &ScreenBufferInfo );

        for ( int i = 0; i < 16; ++i )
        {
            COLORREF& ColourRef = ScreenBufferInfo.ColorTable[ i ];
            Colour SeedColour = PixelColourMap::SeedColours[ i ];
            ColourRef =
                SeedColour.B << 16 |
                SeedColour.G << 8 |
                SeedColour.R;
        }

        SetConsoleScreenBufferInfoEx( s_ConsoleHandle, &ScreenBufferInfo );

        // Get largest possible window size that can fit on screen.
        COORD LargestWindow = GetLargestConsoleWindowSize( s_ConsoleHandle );

        // If smaller than requested size, exit.
        if ( LargestWindow.X < a_WindowSize.i ||
            LargestWindow.Y < a_WindowSize.j )
        {
            return false;
        }

        // Set screen buffer.
        ScreenBuffer::Initialize( a_WindowSize );
        COORD WindowSize = { GetWindowWidth(), GetWindowHeight() };

        // Set window region rect.
        s_WindowRegion.Left = 0;
        s_WindowRegion.Top = 0;
        s_WindowRegion.Right = WindowSize.X - 1;
        s_WindowRegion.Bottom = WindowSize.Y - 1;

        // Set console attributes.
        SetConsoleScreenBufferSize( s_ConsoleHandle, { a_WindowSize.i, a_WindowSize.j } );
        SetConsoleWindowInfo( s_ConsoleHandle, true, &s_WindowRegion );
        GetConsoleScreenBufferInfoEx( s_ConsoleHandle, &ScreenBufferInfo );
        SetConsoleScreenBufferSize( s_ConsoleHandle, { a_WindowSize.i, a_WindowSize.j } );

        // Set cursor attributes.
        CONSOLE_CURSOR_INFO CursorInfo;
        GetConsoleCursorInfo( s_ConsoleHandle, &CursorInfo );
        CursorInfo.bVisible = false;
        SetConsoleCursorInfo( s_ConsoleHandle, &CursorInfo );

        // Set window attributes.
        SetWindowLong( s_WindowHandle, GWL_STYLE, WS_CAPTION | DS_MODALFRAME | WS_MINIMIZEBOX | WS_SYSMENU );
        SetWindowPos( s_WindowHandle, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW );
        SetTitle( a_Title );
    }

    static bool Initialize( const char* a_Title, tVector2< short > a_PixelSize )
    {
        // If cannot create console window, return false.
        if ( !AllocConsole() )
        {
            return false;
        }

        // Retrieve handles for console window.
        s_ConsoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );
        s_WindowHandle = GetConsoleWindow();

        // Set console font.
        a_PixelSize.i = a_PixelSize.i > 8 ? a_PixelSize.i : 8;
        a_PixelSize.j = a_PixelSize.j > 8 ? a_PixelSize.j : 8;
        CONSOLE_FONT_INFOEX FontInfo;
        FontInfo.cbSize = sizeof( FontInfo );
        FontInfo.nFont = 0;
        FontInfo.dwFontSize = { a_PixelSize.i, a_PixelSize.j };
        FontInfo.FontFamily = FF_DONTCARE;
        FontInfo.FontWeight = FW_NORMAL;
        wcscpy_s( FontInfo.FaceName, L"Terminal" );
        SetCurrentConsoleFontEx( s_ConsoleHandle, false, &FontInfo );

        // Get screen buffer info object.
        CONSOLE_SCREEN_BUFFER_INFOEX ScreenBufferInfo;
        ScreenBufferInfo.cbSize = sizeof( ScreenBufferInfo );
        GetConsoleScreenBufferInfoEx( s_ConsoleHandle, &ScreenBufferInfo );
        SetConsoleScreenBufferInfoEx( s_ConsoleHandle, &ScreenBufferInfo );

        // Get largest possible window size that can fit on screen.
        COORD LargestWindow = GetLargestConsoleWindowSize( s_ConsoleHandle );

        // Set Screen Size.
        ScreenBuffer::Initialize( { LargestWindow.X, LargestWindow.Y } );
        s_WindowRegion.Left = 0;
        s_WindowRegion.Top = 0;
        s_WindowRegion.Right = LargestWindow.X - 1;
        s_WindowRegion.Bottom = LargestWindow.Y - 1;

        // Create a window area rect.
        SMALL_RECT WindowArea =
        {
            0,
            0,
            LargestWindow.X - 1,
            LargestWindow.Y - 1
        };

        // Set console attributes.
        SetConsoleScreenBufferSize( s_ConsoleHandle, LargestWindow );
        SetConsoleWindowInfo( s_ConsoleHandle, true, &WindowArea );
        GetConsoleScreenBufferInfoEx( s_ConsoleHandle, &ScreenBufferInfo );
        SetConsoleScreenBufferSize( s_ConsoleHandle, LargestWindow );

        // Set cursor attributes.
        CONSOLE_CURSOR_INFO CursorInfo;
        GetConsoleCursorInfo( s_ConsoleHandle, &CursorInfo );
        CursorInfo.bVisible = false;
        SetConsoleCursorInfo( s_ConsoleHandle, &CursorInfo );

        // Set window attributes.
        SetWindowLong( s_WindowHandle, GWL_STYLE, WS_CAPTION | DS_MODALFRAME | WS_MINIMIZEBOX | WS_SYSMENU );
        SetWindowPos( s_WindowHandle, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW );
        SetTitle( a_Title );
    }
    public:
    static void WriteBuffer()
    {
        WriteConsoleOutput(
            s_ConsoleHandle,
            ScreenBuffer::GetPixelBuffer(),
            { ScreenBuffer::GetBufferWidth(), ScreenBuffer::GetBufferHeight() },
            { 0, 0 },
            &s_WindowRegion );
    }
    private:
    friend class CGE;

    static ConsoleHandle     s_ConsoleHandle;
    static WindowHandle      s_WindowHandle;
    static WindowRegion      s_WindowRegion;
    static tVector2< short > s_PixelSize;
    static wchar_t           s_Title[ 64 ];
};