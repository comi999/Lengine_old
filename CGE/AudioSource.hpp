#pragma once

#include <memory>
#include "Component.hpp"
#include "soloud.h"
#include "soloud_audiosource.h"
#include "soloud_wav.h"
#include "soloud_sfxr.h"
#include "AudioEngine.hpp"
#include "AudioClip.hpp"
#include "AudioEngine.hpp"
#include "SfxrClip.hpp"
#include "Transform.hpp"
#include "Resource.hpp"

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
        if (IsPlaying())
        {
            return;
        }

        if (is3d)
        {
            auto position = GetTransform()->GetGlobalPosition();
            handle = AudioEngine::Play3d(*audioSource.get(), position);
        }
        else
        {
            handle = AudioEngine::Play(*audioSource.get());
        }
        AudioEngine::SetLooping(handle, looping);
        AudioEngine::SetLoopPoint(handle, loopPoint);
        AudioEngine::SetVolume(handle, volume);
    }

    void Stop()
    {
        AudioEngine::Stop(handle);
    }

    bool IsPlaying()
    {
        return AudioEngine::IsPlaying(handle);
    }

    void SetLooping(bool looping)
    {
        AudioEngine::SetLooping(handle, looping);
        this->looping = looping;
    }

    bool IsLooping()
    {
        return looping;
    }

    SoLoud::handle GetHandle()
    {
        return handle;
    }

    void LoadWav(ResourceHandle<AudioClip>& resource)
    {
        auto* clip = resource.Assure();

        auto wav = std::make_unique<SoLoud::Wav>();
        wav->loadMem(clip->data, clip->length, false, false);

        audioSource = std::move(wav);

        Update3dParams();
    }

    void LoadSfx(ResourceHandle<SfxrClip>& resource)
    {
        auto* clip = resource.Assure();

        auto sfxr = std::make_unique<SoLoud::Sfxr>();
        sfxr->loadParamsMem(clip->data, clip->length, false, false);

        audioSource = std::move(sfxr);

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

    void Set3d(bool value)
    {
        is3d = value;
    }

    bool Get3d()
    {
        return is3d;
    }

    void SetVolume(float newVolume)
    {
        AudioEngine::SetVolume(handle, newVolume);
        volume = newVolume;
    }

    float GetVolume()
    {
        return volume;
    }

    void SetLoopPoint(float newLoopPoint)
    {
        AudioEngine::SetLoopPoint(handle, newLoopPoint);
        loopPoint = newLoopPoint;
    }

    float GetLoopPoint()
    {
        return loopPoint;
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
    bool is3d = true;
    bool looping = false;
    double loopPoint = 0.0f;
    float volume = 1.0f;
};
