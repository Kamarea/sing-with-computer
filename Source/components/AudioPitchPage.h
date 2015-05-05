#ifndef AUDIO_PITCH_PAGE_H
#define AUDIO_PITCH_PAGE_H


//#include "../component_headers.h"
#include "ScoreParts.h"

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
	static LiveAudioPitchDisplayComp* getInstance()
	{
		if (instance == NULL)
		{
			instance = new LiveAudioPitchDisplayComp();
		}
		return instance;
	};
    ~LiveAudioPitchDisplayComp();

    void paint (Graphics& g);
    void timerCallback();
	void playClicked(File directory, int* pitchPosition, Array<Note>* scorePitches);
	void stopClicked();

    void audioDeviceAboutToStart (AudioIODevice* device);
    void audioDeviceStopped();
    void audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                                float** outputChannelData, int numOutputChannels, int numSamples);
	Array<float> pitches;
	int* pitchPosition;
	Array<Note>* scorePitches;

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
	FileOutputStream* fileOut;
	std::vector<float> allSamples;
	CriticalSection lock;
	int samplesNumber;
	int begin, end;
	bool isRecording;

	float computePitch(float *my_samples);
	float abs(float a);
	int power(int a, int b);
	float mean(float a[]);
	float max(float a[]);
	float min(float a[]);

	
    LiveAudioPitchDisplayComp();
    LiveAudioPitchDisplayComp (const LiveAudioPitchDisplayComp&);
    LiveAudioPitchDisplayComp& operator= (const LiveAudioPitchDisplayComp&);
};


class AudioPitchPage  : public Component
{
public:
    //==============================================================================
    AudioPitchPage (AudioDeviceManager& deviceManager_);
    ~AudioPitchPage();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();

	void playClicked(File directory, int* pitchPosition, Array<Note>* pitches);
	void stopClicked();

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

#endif // AUDIO_PITCH_PAGE_H