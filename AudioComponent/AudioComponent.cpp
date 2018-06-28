#include "pch.h"
#include "AudioComponent.h"
#include "Windows.h"

//#include "Winuser.h"

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
	waveformat.wBitsPerSample = 16; //jb:changed from 8bits to improve sound when volume is low.
	waveformat.nAvgBytesPerSec = SAMPLERATE*2; // was just samplerate
	waveformat.wFormatTag = WAVE_FORMAT_PCM;
	waveformat.nBlockAlign = 2;//jb (waveformat.nChannels * waveformat.wBitsPerSample) / 8; Note this was 1 for 8 bit
	waveformat.cbSize = 0;

	VoiceCallback* voiceCallback = new VoiceCallback();
	xaudio_engine->CreateSourceVoice(&xaudio_source, &waveformat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, voiceCallback, NULL, NULL);
	xaudio_source->Start();

	ZeroMemory(&xaudio_buffer, sizeof(xaudio_buffer));
	xaudio_buffer.AudioBytes = BUFFERSIZE;

	data1 = new byteint[BUFFERSIZE / 2]; //jb: using 16 bit so the number of samples in a buffer is half the number of bytes
	data2 = new byteint[BUFFERSIZE / 2]; //

	FillBuffer(data1);

	while (true)
	{
		FillBuffer(data2);
		WaitForSingleObject(voiceCallback->hBufferEndEvent, 1000);

		FillBuffer(data1);
		WaitForSingleObject(voiceCallback->hBufferEndEvent, 1000);
	}
}

void AudioEngine::FillBuffer(byteint* buffer) //zz
{

	double incr = currentFrequency / SAMPLERATE;

	for (int i = 0; i < BUFFERSIZE; i=i+2)
	{
        if (pos >= 1)  //jb: When pos = 1, sin(2*PI*pos)=0  so now we can change the in-use volume without making a click
		    {
			pos = pos - 1; //jb: of course, sin(2*PI*(pos-1)) = sin(2*PI*pos), so this subtraction of 1 makes no difference
			inUseVolume = currentVolume*masterVolume;  //jb changes to currentVolume or to masterVolume (at start or stop) are only made now when the output level is zero 
		    }

		buffer[i / 2].iValue = sin((2) * PI * pos)*128*64* inUseVolume; //jb: replace currentVolume*masterVolume with inUseVolume, use 16 bit samples rather than 8 

		pos += incr;
	}

	xaudio_buffer.pAudioData = &buffer[0].b[0];  //jb: &buffer[0].b[0] is the BYTE address of the start of the buffer
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
	currentFrequency = (float)(110 * pow(pow(2, 1.0 / 12), (int)(tone * 44)));//jb: was 22
	currentVolume = volume;
}

AudioEngine::~AudioEngine()
{
	xaudio_engine->Release();
	delete data1;
	delete data2;
}