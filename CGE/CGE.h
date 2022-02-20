#pragma once
#include <Windows.h>
#include <string>
#include "ConsoleWindow.hpp"
#include "Delegate.hpp"
#include "Input.hpp"
#include "Time.hpp"

class CGE
{
public:

    // Create a console window.
    static bool Initialize( const char* a_Title, Vector< short, 2 > a_WindowSize, Vector< short, 2 > a_PixelSize )
    {
        return ConsoleWindow::Initialize( a_Title, a_WindowSize, a_PixelSize );
    }

    // Create a console window with largest possible size.
    static bool Initialize( const char* a_Title, Vector< short, 2 > a_PixelSize )
    {
        return ConsoleWindow::Initialize( a_Title, a_PixelSize );
    }

    // Begin ticking.
    static void Run( const Action<>& a_Action )
    {
        s_Running = true;
        Input::Initialize();

        while ( s_Running )
        {
            a_Action.Invoke();

            Input::Tick();
            float deltaTime = 1000.0f / Time::GetDeltaTime();
            std::string Delta = std::to_string((int)deltaTime);
            Time::Tick();
            ConsoleWindow::SetTitle( Delta.c_str() );
            ConsoleWindow::WriteBuffer();
        }

        Input::Deinitialize();
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