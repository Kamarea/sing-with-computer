/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  1 May 2011 12:07:35pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_AUDIODEMOTABCOMPONENT_AUDIODEMOTABCOMPONENT_ADE99E0E__
#define __JUCER_HEADER_AUDIODEMOTABCOMPONENT_AUDIODEMOTABCOMPONENT_ADE99E0E__

//[Headers]     -- You can add your own extra header files here --
#include "../component_headers.h";
#include "AudioAmplitudePage.h"
#include "AudioSpectrogramPage.h"
#include "AudioPitchPage.h"
#include "SoundInput.h"
#include "ScoreImage.h"
//#include "ScoreParts.h"


//[/Headers]
class SineWaveComponent;

class ScoreTable : public Component
{
public:
	ScoreTable();
	~ScoreTable();

    void paint (Graphics& g);
    void resized();
	void updateScores();

	float pitchPercentage;
	float rhythmPercentage;
	float colourPercentage;

private:
	Label* pitchDescription;
	Label* pitchValue;
	Label* rhythmDescription;
	Label* rhythmValue;
	Label* colourDescription;
	Label* colourValue;

};

//==============================================================================
/**
                                                                    //[Comments]
    This component creates the set of tabs that hold the various
    audio demo pages..
                                                                    //[/Comments]
*/
class AudioTabComponent  : public Component, public Timer
{
public:
    //==============================================================================
    AudioTabComponent ();
	AudioTabComponent (Array<ScorePart> score);
    ~AudioTabComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void timerCallback();
	void afterMetronome();

	AudioSpectrogramPage* m_spectroPage;
	AudioAmplitudePage* m_ampliPage;
	AudioPitchPage* m_pitchPage;
	SoundInput* m_soundInput;
	ScoreImage* scoreImage;
	Array<ScorePart> score;
	Array<float> scorePitchesFreq;
	std::vector<float> scorePitchesMIDI;
	Array<Note> scorePitches;
	std::vector<float> allSamples;
	int actualPitchPosition;
	bool isRecording;
	ScopedPointer<AudioRecorder> recorder;
    AudioDeviceManager deviceManager;
	bool hasScore;
	std::vector<std::pair<int,int>> measuresInSamples;
	int sampleRate;
	SineWaveComponent* sineComp;
	
	ImageButton* playButton;
	ImageButton* stopButton;

	ScoreTable* scoreTable;
	File directory;
	int counter;
	int count;
	int tempoMs;
	float firstSound;

	CriticalSection lock;
    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    TabbedComponent* tabbedComponent;
	
	void init(TabbedComponent* tabbedComponent, bool isScore);

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    AudioTabComponent (const AudioTabComponent&);
    const AudioTabComponent& operator= (const AudioTabComponent&);
};


#endif   // __JUCER_HEADER_AUDIODEMOTABCOMPONENT_AUDIODEMOTABCOMPONENT_ADE99E0E__
