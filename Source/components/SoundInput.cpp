#include "SoundInput.h"
//#include "logs.h"

SoundInput* SoundInput::instance;


//[MiscUserDefs] You can add your own user definitions and misc code here...
SoundInput::SoundInput()
{
	nextSample = subSample = 0;
    accumulator = 0;
    zeromem (samples, sizeof (samples));
    setOpaque (true);
	samplesNumber=0;
}

SoundInput::~SoundInput()
{
}
void SoundInput::audioDeviceAboutToStart (AudioIODevice*)
{
    zeromem (samples, sizeof (samples));
}

void SoundInput::audioDeviceStopped()
{
    zeromem (samples, sizeof (samples));
}

void SoundInput::audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                                                       float** outputChannelData, int numOutputChannels, int numSamples)
{
    for(int i = 0; i < numSamples; ++i)//++i)
    {
        for (int chan = 0; chan < numInputChannels; ++chan)
        {
            if (inputChannelData[chan] != 0)
                accumulator += fabsf (inputChannelData[chan][i]);
        }

        const int numSubSamples = 1; // how many input samples go onto one pixel.
        const float boost = 1.0f;     // how much to boost the levels to make it more visible.

        if (subSample == 0)
        {
            samples[nextSample] = accumulator * boost / numSubSamples;
            nextSample = (nextSample + 1) % numElementsInArray (samples);
            subSample = numSubSamples;
            accumulator = 0;
        }
        else
        {
            --subSample;
        }
    }

	lock.enter();
	for(int i=0;i<numSamples / numInputChannels;i++)
		allSamples.push_back(samples[i]);
	lock.exit();
}

