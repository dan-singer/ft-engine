#pragma once
#include "Component.h"
#include <fmod/fmod.hpp>
// --------------------------------------------------------
// Component which allows sounds to be played.
// Abstraction of FMOD
// --------------------------------------------------------
class SoundComponent : public Component
{
private:
	FMOD::Sound* m_sound = nullptr;
	FMOD::ChannelGroup* m_channelGroup = nullptr;
	float m_volume = 1.0f;
public:

	SoundComponent(Entity* entity);

	virtual void Start() override;

	float GetVolume();
	void SetVolume(float volume);

	// --------------------------------------------------------
	// Sets the sound to be used for this component and 
	// stops currently playing sound, if it is playing.
	// --------------------------------------------------------
	void SetSound(FMOD::Sound* sound);

	// --------------------------------------------------------
	// Plays the set sound.
	// --------------------------------------------------------
	void Play();
	void Pause();
	// --------------------------------------------------------
	// Resumes playing audio that was previously paused
	// --------------------------------------------------------
	void Resume();
	void Stop();

	virtual void Tick(float deltaTime) override;

	~SoundComponent();
};

