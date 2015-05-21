
#include "../component_headers.h"


class SoundInput  : public Component,
                    public AudioIODeviceCallback
{
public:
    //==============================================================================
	static SoundInput* getInstance()
	{
		if (instance == NULL)
		{
			instance = new SoundInput();
		}
		return instance;
	};
    ~SoundInput();


    void audioDeviceAboutToStart (AudioIODevice* device);
    void audioDeviceStopped();
    void audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                                float** outputChannelData, int numOutputChannels, int numSamples);
	std::vector<float> allSamples;
	CriticalSection lock;

private:
	static SoundInput* instance;
    int nextSample, subSample;
    float samples [5 * 512 / 2];
    float accumulator;
	int samplesNumber;
	
    SoundInput();
    SoundInput (const SoundInput&);
    SoundInput& operator= (const SoundInput&);
};