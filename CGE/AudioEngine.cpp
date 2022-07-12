#include "AudioEngine.hpp"
#include "AudioSource.hpp"
#include "Component.hpp"
#include "Transform.hpp"

SoLoud::Soloud AudioEngine::s_SoLoud;

void AudioEngine::Init()
{
    s_SoLoud.init();
}

void AudioEngine::Tick()
{
    auto audioSources = Component::GetExactComponents<AudioSource>();
    for (auto it = audioSources.begin(); it != audioSources.end(); ++it)
    {
        UpdateAudioSourcePosition((*it)->GetHandle(), *(*it)->GetTransform());
    }
}

void AudioEngine::UpdateAudioSourcePosition(SoLoud::handle handle, Transform& transform)
{
    auto position = transform.GetGlobalPosition();
    s_SoLoud.set3dSourcePosition(handle, position.x, position.y, position.z);
}

void AudioEngine::Deinitialize()
{
    s_SoLoud.deinit();
}

SoLoud::handle AudioEngine::Play(SoLoud::AudioSource& source)
{
    return s_SoLoud.play(source);
}

void AudioEngine::SetLooping(SoLoud::handle handle, bool looping)
{
    s_SoLoud.setLooping(handle, looping);
}