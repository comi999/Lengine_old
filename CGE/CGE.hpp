#pragma once
#include <Windows.h>
#include <string>
#include "ConsoleWindow.hpp"
#include "Delegate.hpp"
#include "Input.hpp"
#include "Time.hpp"
#include "Scene.hpp"
#include "RenderPipeline.hpp"
#include "AudioEngine.hpp"

class CGE
{
public:

    static void Init()
    {
        PixelColourMap::Init();
        Resource::Init();
        Input::Init();
        Rendering::Init();
        AudioEngine::Init();
    }

    // Begin ticking.
    static void Run( const Action<>& a_Action )
    {
        s_Running = true;

        while ( s_Running )
        {
            Input::Tick();
            AudioEngine::Tick();

            a_Action.Invoke();

            // Update calls.
            ComponentBase::TickAllComponents();
            Scene::Tick();
            Time::Tick();
            RenderPipeline::Tick();
            
            ConsoleWindow::SwapBuffers( ConsoleWindow::GetCurrentContext() );
        }

        AudioEngine::Deinitialize();
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

    static bool  s_Running;
    static bool  s_ShowFPS;
    static float s_TargetFPS;
    static float s_TargetDeltaTime;
};