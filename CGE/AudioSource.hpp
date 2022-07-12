#pragma once

#include <memory>
#include "Component.hpp"
#include "soloud.h"
#include "soloud_wav.h"
#include "soloud_audiosource.h"
#include "Transform.hpp"

DefineComponent(AudioSource, Component)
{
public:
    enum ATTENUATION_MODELS
    {
        // No attenuation
        NO_ATTENUATION = 0,
        // Inverse distance attenuation model
        INVERSE_DISTANCE = 1,
        // Linear distance attenuation model
        LINEAR_DISTANCE = 2,
        // Exponential distance attenuation model
        EXPONENTIAL_DISTANCE = 3
    };

    void Play()
    {
        auto position = GetTransform()->GetGlobalPosition();
        handle = AudioEngine::Play3d(*audioSource.get(), position);
    }

    bool IsPlaying()
    {
        return AudioEngine::
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

        Update3dParams();
    }

    void SetAttenuationModel(ATTENUATION_MODELS newAttenuation)
    {
        attenuation = newAttenuation;
        Update3dParams();
    }

    ATTENUATION_MODELS GetAttenuationModel()
    {
        return attenuation;
    }

    void SetMinDistance(float newMinDistance)
    {
        minDistance = newMinDistance;
        Update3dParams();
    }

    float GetMinDistance()
    {
        return minDistance;
    }

    void SetMaxDistance(float newMaxDistance)
    {
        maxDistance = newMaxDistance;
        Update3dParams();
    }

    float GetMaxDistance()
    {
        return maxDistance;
    }

    void SetRolloffFactor(float newRolloffFactor)
    {
        rolloffFactor = newRolloffFactor;
        Update3dParams();
    }

    float GetRolloffFactor()
    {
        return rolloffFactor;
    }

private: 
    void Update3dParams()
    {
        if (!audioSource)
        {
            return;
        }

        audioSource->set3dAttenuation(attenuation, rolloffFactor);
        audioSource->set3dMinMaxDistance(minDistance, maxDistance);
    }

    std::unique_ptr<SoLoud::AudioSource> audioSource;
    SoLoud::handle handle = 0;
    ATTENUATION_MODELS attenuation = EXPONENTIAL_DISTANCE;
    float rolloffFactor = 0.5f;
    float minDistance = 1.0f;
    float maxDistance = 100.0f;
};
