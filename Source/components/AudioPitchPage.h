#ifndef AUDIO_PITCH_PAGE_H
#define AUDIO_PITCH_PAGE_H


//#include "../component_headers.h"
#include "ScoreParts.h"

class ScoreTable;

//==============================================================================
/* This component scrolls a continuous waveform showing the audio that's currently
   coming into the audio input.
*/

class WindowExercises	: public Component
{
public:
	WindowExercises(std::pair<int,int> measures);
	~WindowExercises();

private:
	Label* label;
	TextButton* buttonYes;
	TextButton* buttonNo;

};

class AudioPitchPage  : public Component,
                        public Timer
{
public:
    //==============================================================================
	static AudioPitchPage* getInstance()
	{
		if (instance == NULL)
		{
			instance = new AudioPitchPage();
		}
		return instance;
	};
    ~AudioPitchPage();

    void paint (Graphics& g);
    void timerCallback();
	void playClicked(File directory, int* pitchPosition);
	void stopClicked();

	std::vector<float> pitches;
	int* pitchPosition;
	std::vector<float> scorePitches;
	std::vector<float> pitchErrorsTable;
	std::vector<float> rythmErrorsTable;
	bool hasScore;
	float getFirstSound();
	void setScore(std::vector<float>* scorePitchesIn, std::vector<std::pair<int,int>> measures);

	void setScoreTablePtr(ScoreTable*);
	void updateSamples(int number, std::vector<float>* samples);
	WindowExercises* window;

private:
	static AudioPitchPage* instance;
	int numberOfSamplesRead;
	int numberOfSamplesRecalculated;
	///int numSamplesX;
    //float samples [2048];
	float pitch[2048];
	float pitchMIDI[2048];
	float frequency;
    float accumulator;
	int test;
	FileOutputStream* fileOut;
	std::vector<float> allSamples;
	std::vector<float> recordedSamples;
	std::vector<float> recordedDistances;
	CriticalSection lock;
	int samplesNumber;
	int begin, end;
	bool isRecording;
	int recordedScoreNumber;
	ScoreTable* scoreTable;
	std::vector<std::pair<int,int>> measuresInSamples;
	float pitchTreshold;
	
	float restsPercentage;
	float pitchPercentage;

	void calculateWrongest();
	void calculateDistances();
	float computePitch(float *my_samples);
	float abs(float a);
	int power(int a, int b);
	float mean(float a[]);
	float max(float a[]);
	float min(float a[]);
	void rewriteScore(std::pair<int,int>);

	
    AudioPitchPage();
    AudioPitchPage (const AudioPitchPage&);
    AudioPitchPage& operator= (const AudioPitchPage&);
};

/*
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

	void playClicked(File directory, int* pitchPosition, Array<float>* pitches);
	void stopClicked();
	
	void setScoreTablePtr(ScoreTable*);

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
};*/

#endif // AUDIO_PITCH_PAGE_H