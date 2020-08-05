#pragma once

#include <xaudio2.h>
#include <mmsystem.h>

#pragma comment (lib,"xaudio2.lib")
#pragma comment (lib,"winmm.lib")

class AudioManager
{
private:
	IXAudio2* audio;
	IXAudio2MasteringVoice* master;
	IXAudio2SourceVoice* voices[64] = {};
	XAUDIO2_BUFFER audioBuffers[64] = {};
	BYTE* soundSources[64] = {};
	DWORD playPos[64] = {};
public:
	AudioManager();
	~AudioManager();
	void Init();
	void Uninit();
	unsigned int Load(const char* filename);
	void Play(unsigned int num, bool loop = false);
	void Stop(unsigned int num);
};

