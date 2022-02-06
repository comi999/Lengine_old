#pragma once
#include <Windows.h>
#include <string>
#include "ConsoleWindow.hpp"

using namespace std;

class CGE
{
public:

    // Create a console window.
    static bool Initialize( const char* a_Title, tVector2< short > a_WindowSize, tVector2< short > a_PixelSize )
    {
        return ConsoleWindow::Initialize( a_Title, a_WindowSize, a_PixelSize );
    }

    // Create a console window with largest possible size.
    static bool Initialize( const char* a_Title, tVector2< short > a_PixelSize )
    {
        return ConsoleWindow::Initialize( a_Title, a_PixelSize );
    }

    // Begin ticking.
    static bool Run()
    {
        s_Running = true;

        while ( s_Running )
        {

        }
    }

    static bool Quit()
    {
        if ( !s_Running )
        {
            return false;
        }

        s_Running = false;
        return true;
    }

    static bool IsRunning()
    {
        return s_Running;
    }

    static bool s_Running;
};