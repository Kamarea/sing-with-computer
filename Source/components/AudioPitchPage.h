#include "../component_headers.h"

//==============================================================================
/* This component scrolls a continuous waveform showing the audio that's currently
   coming into the audio input.
*/
class LiveAudioPitchDisplayComp  : public Component,
                                   public AudioIODeviceCallback,
                                   public Timer
{
public:
    //==============================================================================
	static LiveAudioPitchDisplayComp* getInstance(std::string folderName)
	{
		if (instance == NULL)
		{
			instance = new LiveAudioPitchDisplayComp(folderName);
		}
		return instance;
	};
    ~LiveAudioPitchDisplayComp();

    void paint (Graphics& g);
    void timerCallback();

    void audioDeviceAboutToStart (AudioIODevice* device);
    void audioDeviceStopped();
    void audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                                float** outputChannelData, int numOutputChannels, int numSamples);
	Array<float> pitches;

private:
	static LiveAudioPitchDisplayComp* instance;
	int numSamplesX;
    float samples [2048];
	float pitch[2048];
	float pitchMIDI[2048];
	float frequency;
    int nextSample, subSample;
    float accumulator;
	int test;

	float computePitch(float *my_samples);
	float abs(float a);
	int power(int a, int b);
	float mean(float a[]);
	float max(float a[]);
	float min(float a[]);

	
    LiveAudioPitchDisplayComp(std::string folderName);
    LiveAudioPitchDisplayComp (const LiveAudioPitchDisplayComp&);
    LiveAudioPitchDisplayComp& operator= (const LiveAudioPitchDisplayComp&);
};


class AudioPitchPage  : public Component
{
public:
    //==============================================================================
    AudioPitchPage (AudioDeviceManager& deviceManager_, std::string folderName);
    ~AudioPitchPage();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();



    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    AudioDeviceManager& deviceManager;
    //[/UserVariables]

    //==============================================================================
    LiveAudioPitchDisplayComp* liveAudioPDisplayComp;


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    AudioPitchPage (const AudioPitchPage&);
    const AudioPitchPage& operator= (const AudioPitchPage&);
};
