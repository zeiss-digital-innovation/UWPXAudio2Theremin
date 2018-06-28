#pragma once
#include <xaudio2.h>

#define PI 3.14159265358979323846f
#define SAMPLERATE 44100
#define BUFFERSIZE 2048 //jb: buffersize in BYTES. It was 1024

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

			union byteint  //jb: used to convert a 16 bit integer to 2 bytes. (union lets us interpret the same memory both ways)
			{
				BYTE b[sizeof int16_t];
				int16_t iValue;
			};

			void FillBuffer(byteint* buffer); //zz

			XAUDIO2_BUFFER xaudio_buffer;
			IXAudio2* xaudio_engine;
			IXAudio2MasteringVoice* xaudio_master;
			IXAudio2SourceVoice* xaudio_source;

			double currentFrequency = 440;
			double masterVolume = 0;
			double currentVolume = 1;
			double inUseVolume =  masterVolume*currentVolume; //jb: volume being played now (only changes at crossing points where sound output is near zero) 
			double pos = 0; //jb moved from static


			byteint* data1; //jb: converted from BYTE to allow us to work in 16 bit integer but then convert to BYTEs
			byteint* data2;

    };
}
