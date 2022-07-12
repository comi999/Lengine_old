#include "AudioEngine.hpp"
#include "AudioSource.hpp"
#include "AudioListener.hpp"
#include "Component.hpp"
#include "Transform.hpp"

SoLoud::Soloud AudioEngine::s_SoLoud;

void AudioEngine::Init()
{
    s_SoLoud.init(SoLoud::Soloud::FLAGS::LEFT_HANDED_3D);
}

void AudioEngine::Tick()
{
    auto audioSources = Component::GetExactComponents<AudioSource>();
    for (auto it = audioSources.begin(); it != audioSources.end(); ++it)
    {
        UpdateAudioSourcePosition((*it)->GetHandle(), *(*it)->GetTransform());
    }

    auto audioListeners = Component::GetExactComponents<AudioListener>();
    _STL_ASSERT(audioListeners.size() <= 1, "Only one audio listener allowed!");
    if (audioListeners.size() > 0) 
    {
        auto* audioListener = *audioListeners.begin();
        auto* transform = audioListener->GetTransform();
        auto position = transform->GetGlobalPosition();
        auto forward = transform->GetGlobalForward();
        auto up = transform->GetGlobalUp();

        s_SoLoud.set3dListenerPosition(position.x, position.y, position.z);
        s_SoLoud.set3dListenerAt(forward.x, forward.y, forward.z);
        s_SoLoud.set3dListenerUp(up.x, up.y, up.z);
        s_SoLoud.update3dAudio();
    }
}

void AudioEngine::UpdateAudioSourcePosition(const SoLoud::handle& handle, const Transform& transform)
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

SoLoud::handle AudioEngine::Play3d(SoLoud::AudioSource& source, const Vector3 position)
{
    return s_SoLoud.play3d(source, position.x, position.y, position.z);
}

bool AudioEngine::IsPlaying(const SoLoud::handle& handle)
{
    return s_SoLoud.isValidVoiceHandle(handle);
}

void AudioEngine::SetLooping(const SoLoud::handle& handle, const bool looping)
{
    s_SoLoud.setLooping(handle, looping);
}