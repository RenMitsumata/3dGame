#include "AudioManager.h"


AudioManager::AudioManager()
{
}


AudioManager::~AudioManager()
{
}

void AudioManager::Init()
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) {
		MessageBox(nullptr, "COM初期化に失敗しました", "XAudio2", MB_OK | MB_ICONHAND);
		exit(1);
	}

	if (FAILED(XAudio2Create(&audio, 0, XAUDIO2_DEFAULT_PROCESSOR))) {
		MessageBox(nullptr, "XAudio2の生成に失敗しました", "XAudio2", MB_OK | MB_ICONHAND);
		exit(1);
	}

	if (FAILED(audio->CreateMasteringVoice(&master))) {
		MessageBox(nullptr, "マスタリングボイスの生成に失敗しました", "XAudio2", MB_OK | MB_ICONHAND);
		exit(1);
	}

}

void AudioManager::Uninit()
{
	for (int i = 0; i < 64; i++) {
		if (voices[i] != NULL) {
			voices[i]->DestroyVoice();
			voices[i] = NULL;
		}
		if (soundSources[i] != NULL) {
			delete soundSources[i];
		}
	}
	if (master) { master->DestroyVoice(); }
	if (audio) { audio->Release(); }
	CoUninitialize();
}

unsigned int AudioManager::Load(const char * filename)
{
	// サウンドデータ読込
	WAVEFORMATEX wfx = { 0 };
	XAUDIO2_BUFFER buffer;
	BYTE* soundData;

	{
		HMMIO hmmio = NULL;
		MMIOINFO mmioinfo = { 0 };
		MMCKINFO riffchunkinfo = { 0 };
		MMCKINFO datachunkinfo = { 0 };
		MMCKINFO mmckinfo = { 0 };
		UINT32 buflen;
		LONG readlen;


		hmmio = mmioOpen((LPSTR)filename, &mmioinfo, MMIO_READ);
		if (hmmio == NULL) {
			char filestring[256];
			lstrcpy(filestring, filename);
			char exp[32] = { "が読み込めません" };
			lstrcat(filestring, exp);
			MessageBox(NULL, filestring, "XAudio2", MB_OK | MB_ICONHAND);
			exit(1);
		}
		

		riffchunkinfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		mmioDescend(hmmio, &riffchunkinfo, NULL, MMIO_FINDRIFF);

		mmckinfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
		mmioDescend(hmmio, &mmckinfo, &riffchunkinfo, MMIO_FINDCHUNK);

		if (mmckinfo.cksize >= sizeof(WAVEFORMATEX))
		{
			mmioRead(hmmio, (HPSTR)&wfx, sizeof(wfx));
		}
		else
		{
			PCMWAVEFORMAT pcmwf = { 0 };
			mmioRead(hmmio, (HPSTR)&pcmwf, sizeof(pcmwf));
			memset(&wfx, 0x00, sizeof(wfx));
			memcpy(&wfx, &pcmwf, sizeof(pcmwf));
			wfx.cbSize = 0;
		}
		mmioAscend(hmmio, &mmckinfo, 0);

		datachunkinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
		mmioDescend(hmmio, &datachunkinfo, &riffchunkinfo, MMIO_FINDCHUNK);



		buflen = datachunkinfo.cksize;
		soundData = new unsigned char[buflen];
		readlen = mmioRead(hmmio, (HPSTR)soundData, buflen);


		buffer.AudioBytes = readlen;
		buffer.pAudioData = soundData;
		buffer.PlayBegin = 0;
		buffer.PlayLength = readlen / wfx.nBlockAlign;
		buffer.LoopBegin = 0;
		buffer.LoopLength = readlen;
		buffer.LoopCount = 0;

		mmioClose(hmmio, 0);
	}


	// サウンドソース生成
	unsigned int num = 0;
	for (int j = 0; j < 64; j++,num++)
	{
		if (voices[j] == NULL) {
			audio->CreateSourceVoice(&voices[j], &wfx);

			
			audioBuffers[j] = buffer;
			if (soundSources[j]) {
				delete soundSources[j];
			}
			soundSources[j] = new unsigned char[sizeof(soundData)];
			memcpy(soundSources[j], soundData, sizeof(soundData));
			voices[j]->SubmitSourceBuffer(&buffer);
			break;
		}
	}
	
	return num;
}

void AudioManager::Play(unsigned int num, bool loop)
{
	XAUDIO2_VOICE_STATE state;
	voices[num]->GetState(&state);
	if (state.BuffersQueued == 0) {
		// 停止中
		voices[num]->SubmitSourceBuffer(&audioBuffers[num], NULL);
	}
	voices[num]->Start();

}

void AudioManager::Stop(unsigned int num)
{
	voices[num]->Stop();
}
