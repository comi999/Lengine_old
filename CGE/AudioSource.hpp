#pragma once

#include <memory>
#include "Component.hpp"
#include "soloud.h"
#include "soloud_wav.h"
#include "Transform.hpp"

DefineComponent(AudioSource, Component)
{
public:
    void Play()
    {
        handle = AudioEngine::Play(*audioSource.get());
        AudioEngine::UpdateAudioSourcePosition(handle, *GetTransform());
    }

    void SetLooping(bool looping)
    {
        AudioEngine::SetLooping(handle, looping);
    }

    bool IsLooping()
    {
        return AudioEngine->GetLooping(handle);
    }

    SoLoud::handle GetHandle()
    {
        return handle;
    }

    void LoadWav(const char* path)
    {
        auto wav = std::make_unique<SoLoud::Wav>();
        wav->load(path);

        audioSource = std::move(wav);
    }

private: 
    std::unique_ptr<SoLoud::AudioSource> audioSource;
    SoLoud::handle handle = 0;
};
