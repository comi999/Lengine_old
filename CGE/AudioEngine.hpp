#pragma once
#include <memory>
#include "soloud.h"
#include "Transform.hpp"

class AudioEngine
{
public:
    static void Init();
    static void Tick();
    static void Deinitialize();

    static SoLoud::handle Play(SoLoud::AudioSource& source);
    static void SetLooping(SoLoud::handle handle, bool looping);
    static void UpdateAudioSourcePosition(SoLoud::handle handle, Transform& transform);

private:
    static SoLoud::Soloud s_SoLoud;
};

