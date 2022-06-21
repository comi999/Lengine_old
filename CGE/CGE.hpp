#pragma once
#include <Windows.h>
#include <string>
#include "ConsoleWindow.hpp"
#include "Delegate.hpp"
#include "Input.hpp"
#include "Time.hpp"
#include "Resource.hpp"
#include "ECS.hpp"
#include "MeshRenderer.hpp"

class CGE
{
public:

    // Begin ticking.
    static void Run( const Action<>& a_Action )
    {
        s_Running = true;
        Input::Initialize();

        while ( s_Running )
        {
            a_Action.Invoke();

            // Update calls.
            Input::Tick();
            Time::Tick();
            
            // Draw calls.
            Draw();
            ConsoleWindow::SwapBuffers( ConsoleWindow::GetCurrentContext() );
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

    inline static bool IsRunning()
    {
        return s_Running;
    }

private:

    static void Draw()
    {
        // Change this to deal with all generic renderers in the future.
        //auto Renderers = ECS::GetComponents< MeshRenderer >();

        //// This system needs to be upgraded to do batch rendering. Right now, it's extremely inneficient to call draw on each renderer.
        //for ( auto& RendererType : Renderers )
        //{
        //    for ( size_t i = 0; i < RendererType.second; ++i )
        //    {
        //        RendererType.first[ i ].OnDraw();
        //    }
        //}
    }

    static bool  s_Running;
    static bool  s_ShowFPS;
    static float s_TargetFPS;
    static float s_TargetDeltaTime;
};