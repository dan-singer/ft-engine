#include "SoundComponent.h"
#include "World.h"
#include "Entity.h"

SoundComponent::SoundComponent(Entity* entity) : Component(entity)
{
	FMOD::System* system = World::GetInstance()->GetSoundSystem();
	const char* channelName = GetOwner()->GetName().c_str();
	system->createChannelGroup(channelName, &m_channelGroup);
}

void SoundComponent::Start()
{

}

float SoundComponent::GetVolume()
{
	return m_volume;
}

void SoundComponent::SetVolume(float volume)
{
	m_volume = volume;
	m_channelGroup->setVolume(m_volume);
}

void SoundComponent::SetSound(FMOD::Sound* sound)
{
	m_channelGroup->stop();
	m_sound = sound;
}

void SoundComponent::Play()
{
	if (!m_sound) {
		throw "No sound has been set";
	}
	FMOD::System* system = World::GetInstance()->GetSoundSystem();
	m_channelGroup->setPaused(false);

	system->playSound(m_sound, m_channelGroup, false, nullptr);
}

void SoundComponent::Pause()
{
	if (!m_sound) {
		throw "No sound has been set";
	}
	m_channelGroup->setPaused(true);
}

void SoundComponent::Resume()
{
	if (!m_sound) {
		throw "No sound has been set";
	}
	m_channelGroup->setPaused(false);
}

void SoundComponent::Stop()
{
	if (!m_sound) {
		throw "No sound has been set";
	}
	m_channelGroup->stop();
}

void SoundComponent::Tick(float deltaTime)
{
}

SoundComponent::~SoundComponent()
{
	m_channelGroup->release();
}
