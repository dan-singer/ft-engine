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
	bool m_loop = false;
public:
	virtual void Start() override;

	float GetVolume();
	void SetVolume(float volume);

	bool GetLoop();
	void SetLoop(bool loop);

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

};

