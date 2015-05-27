#include "../component_headers.h"
#include <fstream>

//==============================================================================
/* This component scrolls a continuous waveform showing the audio that's currently
   coming into the audio input.
*/

class AudioRecorder  : public AudioIODeviceCallback
{
public:
    AudioRecorder();
    ~AudioRecorder();

    //==============================================================================
    void startRecording (const File& file);
    void stop();
    bool isRecording() const;

    //==============================================================================
    void audioDeviceAboutToStart (AudioIODevice* device);
    void audioDeviceStopped();
    void audioDeviceIOCallback (const float** inputChannelData, int /*numInputChannels*/,
                                float** outputChannelData, int numOutputChannels,
                                int numSamples);

private:
    TimeSliceThread backgroundThread; // the thread that will write our audio data to disk
    ScopedPointer<AudioFormatWriter::ThreadedWriter> threadedWriter; // the FIFO used to buffer the incoming data
    double sampleRate;

    CriticalSection writerLock;
    AudioFormatWriter::ThreadedWriter* volatile activeWriter;
};

class AudioAmplitudePage  : public Component,
                                   public Timer
{
public:
    //==============================================================================
	static AudioAmplitudePage* getInstance()
	{
		if (instance == NULL)
		{
			instance = new AudioAmplitudePage();
		}
		return instance;
	};
    ~AudioAmplitudePage();

    void paint (Graphics& g);
    void timerCallback();

    void updateSamples(int number, std::vector<float>* samples);

private:
	static AudioAmplitudePage* instance;
	std::vector<float> allSamples;
	std::vector<float> amplitudes;
    float accumulator;
	CriticalSection lock;
	float meanNoise;
	
	int numberOfSamplesRead;
	int numberOfSamplesRecalculated;

    AudioAmplitudePage();
    AudioAmplitudePage (const AudioAmplitudePage&);
    AudioAmplitudePage& operator= (const AudioAmplitudePage&);
};

/*
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
	bool isRecording;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    AudioAmplitudePage (const AudioAmplitudePage&);
    const AudioAmplitudePage& operator= (const AudioAmplitudePage&);
};
*/

