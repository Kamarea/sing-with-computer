#include "../component_headers.h"
#include <fstream>

//==============================================================================
/* This component scrolls a continuous waveform showing the audio that's currently
   coming into the audio input.
*/
//class AudioRecorder;
class AudioRecorder;

class LiveAudioAmplitudeDisplayComp  : public Component,
                                   public AudioIODeviceCallback,
                                   public Timer
{
public:
    //==============================================================================
	static LiveAudioAmplitudeDisplayComp* getInstance()
	{
		if (instance == NULL)
		{
			instance = new LiveAudioAmplitudeDisplayComp();
		}
		return instance;
	};
    ~LiveAudioAmplitudeDisplayComp();

    void paint (Graphics& g);
    void timerCallback();

    void audioDeviceAboutToStart (AudioIODevice* device);
    void audioDeviceStopped();
    void audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                                float** outputChannelData, int numOutputChannels, int numSamples);
private:
	static LiveAudioAmplitudeDisplayComp* instance;
    float samples [2048];
    int nextSample, subSample;
    float accumulator;

    LiveAudioAmplitudeDisplayComp();
    LiveAudioAmplitudeDisplayComp (const LiveAudioAmplitudeDisplayComp&);
    LiveAudioAmplitudeDisplayComp& operator= (const LiveAudioAmplitudeDisplayComp&);
};


class AudioAmplitudePage  : public Component
{
public:
    //==============================================================================
    AudioAmplitudePage (AudioDeviceManager& deviceManager_);
    ~AudioAmplitudePage();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
	void playClicked(File folderName);
	void stopClicked();

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    AudioDeviceManager& deviceManager;
    //ScopedPointer<AudioRecorder> recorder;
    //[/UserVariables]

    //==============================================================================
	ScopedPointer<AudioRecorder> recorder;
	LiveAudioAmplitudeDisplayComp* liveAudioAmplitudeDisplayComp;
    //Label* explanationLabel;
    //TextButton* recordButton;
	File file;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    AudioAmplitudePage (const AudioAmplitudePage&);
    const AudioAmplitudePage& operator= (const AudioAmplitudePage&);
};


