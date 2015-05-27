#include "../component_headers.h"
//#include "AudioTabComponent.h"
//class AudioRecorder;

//==============================================================================
/* This component scrolls a continuous waveform showing the audio that's currently
   coming into the audio input.
*/
class AudioSpectrogramPage  : public Component,
							  public Timer
{
public:
    //==============================================================================
	static AudioSpectrogramPage* getInstance()
	{
		if (instance == NULL)
		{
			instance = new AudioSpectrogramPage();
		}
		return instance;
	};
    AudioSpectrogramPage();
    ~AudioSpectrogramPage();

    void paint (Graphics& g);
    void timerCallback();
	void playClicked(File directory);
	void stopClicked();
	void updateSamples(int number, std::vector<float>* samples);

private:
	static AudioSpectrogramPage* instance;
	int numberOfSamplesRead;
	int numberOfSamplesRecalculated;
    //float samples[1024];
	float transformSamples[2048];
	float spectroSamples[1024];
	std::vector<float> allSamples;
	std::vector<std::vector<float>> allSpectroSamples;
	//float paintSamples[2048][1024];
    //int nextSample, subSample;
    //float accumulator;
	int counter;
	Image image;
	FileOutputStream* fileOutput;
	int shiftNumber;
	int begin, end;
	CriticalSection lock;
	bool isRecording;
	std::vector<float> spectroNoise;
	std::vector<int> pitchFreq;

	void calculatePitch(std::vector<float>);

    AudioSpectrogramPage (const AudioSpectrogramPage&);
    AudioSpectrogramPage& operator= (const AudioSpectrogramPage&);
};

/*
class AudioSpectrogramPage  : public Component
{
public:
    //==============================================================================
    AudioSpectrogramPage (AudioDeviceManager& deviceManager_);
    ~AudioSpectrogramPage();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();

	void playClicked(File directory);
	void stopClicked();

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    AudioDeviceManager& deviceManager;
    //ScopedPointer<AudioRecorder> recorder;
    //[/UserVariables]

    //==============================================================================
    LiveAudioSpectrogramDisplayComp* liveAudioSpectrogramDisplayComp;
    //Label* explanationLabel;
    //TextButton* recordButton;


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    AudioSpectrogramPage (const AudioSpectrogramPage&);
    const AudioSpectrogramPage& operator= (const AudioSpectrogramPage&);
};
*/
