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
    static SoLoud::handle Play3d(SoLoud::AudioSource& source, const Vector3 position);
    static bool IsPlaying(const SoLoud::handle& handle);
    static void SetLooping(const SoLoud::handle& handle, const bool looping);
    static void UpdateAudioSourcePosition(const SoLoud::handle& handle, const Transform& transform);

private:
    static SoLoud::Soloud s_SoLoud;
};

