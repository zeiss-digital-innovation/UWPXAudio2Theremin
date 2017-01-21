#pragma once
#include <xaudio2.h>

#define PI 3.14159265358979323846f
#define SAMPLERATE 44100
#define BUFFERSIZE 1024

namespace AudioComponent
{
    public ref class AudioEngine sealed
    {
		public:

			virtual ~AudioEngine();

			void StartEngineLoop();
			void Start();
			void Stop();
			void SetTone(double tone, double volume);

		private:

			void FillBuffer(BYTE * buffer);

			XAUDIO2_BUFFER xaudio_buffer;
			IXAudio2* xaudio_engine;
			IXAudio2MasteringVoice* xaudio_master;
			IXAudio2SourceVoice* xaudio_source;

			double currentFrequency = 440;
			double masterVolume = 0;
			double currentVolume = 1;
			BYTE* data1;
			BYTE* data2;
    };
}
