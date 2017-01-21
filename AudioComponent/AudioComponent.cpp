#include "pch.h"
#include "AudioComponent.h"

using namespace AudioComponent;
using namespace Platform;

class VoiceCallback : public IXAudio2VoiceCallback
{
public:

	HANDLE hBufferEndEvent;
	VoiceCallback() { hBufferEndEvent = CreateEvent(NULL, FALSE, FALSE, NULL); }
	~VoiceCallback() { CloseHandle(hBufferEndEvent); }

	void STDMETHODCALLTYPE OnBufferEnd(void * pBufferContext)
	{
		SetEvent(hBufferEndEvent);
	}

	void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() { }
	void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 SamplesRequired) { }
	void STDMETHODCALLTYPE OnBufferStart(void * pBufferContext) { }
	void STDMETHODCALLTYPE OnLoopEnd(void * pBufferContext) { }
	void STDMETHODCALLTYPE OnStreamEnd() { }
	void STDMETHODCALLTYPE OnVoiceError(void * pBufferContext, HRESULT Error) { }
};

void AudioEngine::StartEngineLoop()
{
	//create the engine and voices
	XAudio2Create(&xaudio_engine);
	xaudio_engine->CreateMasteringVoice(&xaudio_master);

	WAVEFORMATEX waveformat;
	waveformat.nChannels = 1;
	waveformat.nSamplesPerSec = SAMPLERATE;
	waveformat.wBitsPerSample = 8;
	waveformat.nAvgBytesPerSec = SAMPLERATE;
	waveformat.wFormatTag = WAVE_FORMAT_PCM;
	waveformat.nBlockAlign = 1;// (waveformat.nChannels * waveformat.wBitsPerSample) / 8;
	waveformat.cbSize = 0;

	VoiceCallback* voiceCallback = new VoiceCallback();
	xaudio_engine->CreateSourceVoice(&xaudio_source, &waveformat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, voiceCallback, NULL, NULL);
	xaudio_source->Start();

	ZeroMemory(&xaudio_buffer, sizeof(xaudio_buffer));
	xaudio_buffer.AudioBytes = BUFFERSIZE;

	data1 = new BYTE[BUFFERSIZE];
	data2 = new BYTE[BUFFERSIZE];

	FillBuffer(data1);

	while (true)
	{
		FillBuffer(data2);
		WaitForSingleObject(voiceCallback->hBufferEndEvent, 1000);

		FillBuffer(data1);
		WaitForSingleObject(voiceCallback->hBufferEndEvent, 1000);
	}
}

void AudioEngine::FillBuffer(BYTE* buffer)
{
	static double pos = 0;

	double incr = currentFrequency / SAMPLERATE;
	for (int i = 0; i < BUFFERSIZE; i++)
	{
		buffer[i] = (BYTE)(sin((2) * PI * pos)* currentVolume*masterVolume * 128) + 128;
		pos += incr;
	}

	xaudio_buffer.pAudioData = buffer;
	xaudio_source->SubmitSourceBuffer(&xaudio_buffer);
}

void AudioEngine::Start()
{
	masterVolume = 1.0;
}

void AudioEngine::Stop()
{
	masterVolume = 0;
}

void AudioEngine::SetTone(double tone, double volume)
{
	currentFrequency = (float)(110 * pow(pow(2, 1.0 / 12), (int)(tone * 22)));
	currentVolume = volume;
}

AudioEngine::~AudioEngine()
{
	xaudio_engine->Release();
	delete data1;
	delete data2;
}