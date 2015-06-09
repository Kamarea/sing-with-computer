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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include <sstream>

#include "AudioTabComponent.h"
#include <math.h>

//[/MiscUserDefs]

class SineWaveComponent :	public Component,
							public AudioIODeviceCallback,
							public Timer
{
public:
	SineWaveComponent(int firstSound, int count_in, int tempoMs, AudioTabComponent* component)
	{
		sound = 440 * std::pow(2,((float)firstSound-57)/12);
		actualSound = sound;
		actualAmpli = 1;
		sampleRate = 44100;
		sampleNumber = 0;
		tempo = tempoMs;
		count = count_in;
		counter = 0;
		m_component = component;
		startTimer (tempoMs);
	}
	~SineWaveComponent()
	{
	}

	void audioDeviceAboutToStart (AudioIODevice* device)
	{
	}
    void audioDeviceStopped()
	{
	}
    void audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                                float** outputChannelData, int numOutputChannels, int numSamples)
	{
		float value;
		for(int i = 0; i < numSamples; i++)
		{
			value = actualAmpli * std::sin(1.6 * actualSound * ((float)((sampleNumber + i)) / sampleRate));
			if (value < 0)
				value *= (-1);
			for (int chan = 0; chan < numOutputChannels; chan++)
			{
				outputChannelData[chan][i] = value;
			}
		}
		
		sampleNumber+=numSamples;
	}
	
    void timerCallback()
	{
		if (actualAmpli == 1)
			actualAmpli = 0;
		else
		{
			actualAmpli = 1;
			counter++;
			if (counter == count)
				m_component->afterMetronome();
		}
	}
private:
	float sound;
	float actualSound;
	int actualAmpli;
	int sampleRate;
	int sampleNumber;
	int tempo;
	int counter;
	int count;
	AudioTabComponent* m_component;
};

//==============================================================================
class PlayListener: public Button::Listener 
{
public:
	PlayListener(AudioTabComponent* component)
	{
		m_component = component;
	};
	void buttonClicked(Button* button)
	{		
		
		m_component->playButton->setEnabled(false);
		Time actualTime=Time::getCurrentTime();

		std::ostringstream sstream;
		sstream << "data\\" << Globals::getInstance()->getUser() << "\\" << actualTime.getYear() << "-" << actualTime.getMonth()+1 << "-" <<
			actualTime.getDayOfMonth() << "_" << actualTime.getHours() << "-" <<
			actualTime.getMinutes();
		std::string folderName = sstream.str();
		File directory = File(File::getSpecialLocation(File::currentExecutableFile).
			getParentDirectory().getParentDirectory().getChildFile (folderName.c_str()));
		directory.createDirectory();
		m_component->directory = directory;
		if(m_component->hasScore)
		{
			m_component->m_pitchPage->setScore(&(m_component->scorePitchesMIDI), m_component->measuresInSamples);
			// wygranie pocz¹tkowego dŸwiêku i metronomu
			float firstSound = m_component->m_pitchPage->getFirstSound();
			int tempoMs = 60 * 1000 / m_component->score[0].tempo.value;
			int count = m_component->score[0].time.count;
			m_component->count = count;
			m_component->counter = 1;
			m_component->firstSound = firstSound;
			m_component->tempoMs = tempoMs;
			m_component->m_soundInput->isPaused = true;

			m_component->sineComp = new SineWaveComponent(firstSound, count, tempoMs, m_component);
			m_component->deviceManager.addAudioCallback (m_component->sineComp);
		} else
		{
			m_component->afterMetronome();
		}
	};
	AudioTabComponent* m_component;
};

class StopListener: public Button::Listener 
{
public:
	StopListener(AudioTabComponent* component)
	{
		m_component = component;
	};
	void buttonClicked(Button* button)
	{
		m_component->isRecording = false;
		m_component->m_spectroPage->stopClicked();
		m_component->m_pitchPage->stopClicked();
		if (Globals::getInstance()->getShowScore())
		{
			if (m_component->hasScore)
				m_component->scoreImage->stopClicked();
		}
		m_component->recorder->stop();
		m_component->deviceManager.removeAudioCallback (m_component->recorder);
		m_component->recorder = 0;
		m_component->stopButton->setEnabled(false);
		m_component->playButton->setEnabled(true);
	};
	AudioTabComponent* m_component;
};

AudioTabComponent::AudioTabComponent ()
    : tabbedComponent (0),
	actualPitchPosition (0),
    keyboardComponent (0),
	sampleRate(0)
{
	tabbedComponent = new TabbedComponent (TabbedButtonBar::TabsAtTop);
	init(tabbedComponent,false);
}

AudioTabComponent::AudioTabComponent (Array<ScorePart> scoreParts)
    : tabbedComponent (0),
	actualPitchPosition (0),
    keyboardComponent (0),
	sampleRate(0)
{
	
	tabbedComponent = new TabbedComponent (TabbedButtonBar::TabsAtTop);
	init(tabbedComponent,true);
	scoreTable = new ScoreTable();
	addAndMakeVisible (scoreTable);
	score = scoreParts;

	keyboardComponent = new MidiKeyboardComponent (keyboardState, MidiKeyboardComponent::horizontalKeyboard);
	if (Globals::getInstance()->getShowScore())
	{
		scoreImage = new ScoreImage(score);
		addAndMakeVisible(scoreImage);
	}

	AudioDeviceManager::AudioDeviceSetup setup;
	deviceManager.getAudioDeviceSetup(setup);
	sampleRate = (setup.sampleRate * 60) / 512 / 2.1;

	m_pitchPage->hasScore = true;
	m_pitchPage->setScoreTablePtr(scoreTable);
	// zamiana z dziedziny zmian na czas
	// n / min -> kwantujê do 10/sek -> 600 / min -> 600 / n
	int actualTempo;
	int actualTempoBase;
	Pitch lastPitch = score[0].notes[0].pitch;
	int lastOctave = score[0].notes[0].octave;
	int count = 0;
	int base = 0;
	int actualMeasure = 1;
	measuresInSamples.push_back(std::pair<int,int>(1,0));
	for (int i=0; i < score.size(); i++)
	{
		actualTempo = score[i].tempo.value;
		actualTempoBase = score[i].tempo.base;
		base = score[i].time.count;

		// dla k = 0; note.value * (600 / n) -> zapisaæ czêstotliwoœæ w tabeli
		// dlugosc = note.value / tempo.base
		int length;
		float freq = 0;
		float pitchMIDI;
		for (int k = 1; k < score[i].notes.size(); k++)
		{
			// zmienic, dostosowac sampleRate
			// przeniesc odliczanie ze score
			// dorobic przeliczanie score do updatePitches
			// dorobic rysowanie score w PitchPage
			// potem narysowac pitch w spectrogramie
			length = (sampleRate / actualTempo) * (score[i].notes[k].duration);
			freq = getPitch(score[i].notes[k].pitch);
			if (score[i].notes[k].octave > 0)
				freq *= 2 * score[i].notes[k].octave; 
			if (score[i].notes[k].octave < 0)
				freq /= (-1) * 2 * score[i].notes[k].octave;
			lastPitch = score[i].notes[k].pitch;
			lastOctave = score[i].notes[k].octave;
			pitchMIDI = std::max<float>(0.0, 69+12*(log10(freq / 440)/log_10_2));
			for (int l = 0; l < length; l++)
			{
				scorePitches.add(score[i].notes[k]);
				scorePitchesFreq.add(freq);
				scorePitchesMIDI.push_back(pitchMIDI);
			}
			count += score[i].notes[k].duration;
			if (count >= base)
			{
				actualMeasure++;
				measuresInSamples.push_back(std::pair<int,int>(actualMeasure,scorePitchesMIDI.size()));
				count = 0;
			}
		}
	}

    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

void AudioTabComponent::init(TabbedComponent* tabbedComponent, bool isScore)
{		
	counter = 0;
	hasScore = isScore;
    addAndMakeVisible (tabbedComponent);
    tabbedComponent->setTabBarDepth (30);

	m_soundInput = SoundInput::getInstance();
	deviceManager.addAudioCallback(m_soundInput);
	m_ampliPage =  AudioAmplitudePage::getInstance();
	m_spectroPage =  AudioSpectrogramPage::getInstance();
	m_pitchPage =  AudioPitchPage::getInstance();
    tabbedComponent->addTab (Globals::getInstance()->getTexts()[18], Colours::lightgrey, m_ampliPage, true);
    tabbedComponent->addTab (Globals::getInstance()->getTexts()[20], Colours::lightgrey, m_spectroPage, true);
    tabbedComponent->addTab (Globals::getInstance()->getTexts()[19], Colours::lightgrey, m_pitchPage, true);
    tabbedComponent->setCurrentTabIndex (0);

	playButton = new ImageButton("play");
	File playImageFile = File(File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getChildFile("images").getChildFile("bplay.png"));
	Image playImage = ImageCache::getFromFile(playImageFile);
	File kplayImageFile = File(File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getChildFile("images").getChildFile("kplay.png"));
	Image kplayImage = ImageCache::getFromFile(kplayImageFile);
	playButton->setImages(true, true, true, 
							playImage, 1.0000f, Colour (0x0),
                            Image(), 1.0000f, Colour (0x0),
                            kplayImage, 1.0000f, Colour (0x0));
	
	PlayListener *playListener = new PlayListener(this);
	playButton->addListener(playListener);
    addAndMakeVisible (playButton);
	stopButton = new ImageButton("stop");
	File stopImageFile = File(File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getChildFile("images").getChildFile("bstop.png"));
	Image stopImage = ImageCache::getFromFile(stopImageFile);
	stopButton->setImages(true, true, true, 
							stopImage, 1.0000f, Colour (0x0),
                            Image(), 1.0000f, Colour (0x0),
                            Image(), 1.0000f, Colour (0x0));
	StopListener *stopListener = new StopListener(this);
	stopButton->addListener(stopListener);
	stopButton->setEnabled(false);
    addAndMakeVisible (stopButton);

    //[UserPreSize]
    deviceManager.initialise (2, 2, 0, true, String::empty, 0);
    //[/UserPreSize]

    setSize (getWidth(), getHeight()/2);
	//tabbedComponent->setBounds(0,getHeight()/2,getWidth(),getHeight()/2);//getHeight()/2
	
	startTimer(20);
}

AudioTabComponent::~AudioTabComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]
	//if(stopButton->isEnabled())
	//	stopButton->triggerClick();
	deviceManager.removeAudioCallback (m_soundInput);

    deleteAndZero (tabbedComponent);
	deleteAndZero (playButton);
	deleteAndZero (stopButton);
	deleteAndZero (m_soundInput);
	if (Globals::getInstance()->getShowScore())
		deleteAndZero (scoreImage);
	if (hasScore)
		deleteAndZero (scoreTable);
	
    deleteAndZero (keyboardComponent);
    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

void AudioTabComponent::timerCallback()
{
	lock.enter();
	m_soundInput->lock.enter();
	int actualNumber = allSamples.size();
	for(int i = actualNumber; i < m_soundInput->allSamples.size(); i++)
	{
		allSamples.push_back(m_soundInput->allSamples[i]);
	}
	actualNumber = allSamples.size();
	m_soundInput->lock.exit();
	// launch all calculators
	m_ampliPage->updateSamples(actualNumber, &allSamples);
	m_pitchPage->updateSamples(actualNumber, &allSamples);
	m_spectroPage->updateSamples(actualNumber, &allSamples);
	lock.exit();

}

void AudioTabComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::white);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AudioTabComponent::resized()
{
    /*tabbedComponent->setBounds (0,getHeight()/2, getWidth(), getHeight()/2);//getHeight()/2
    playButton->setBounds(getWidth()-100,getHeight()/2-50,50,50);
	stopButton->setBounds(getWidth()-50,getHeight()/2-50,50,50);
	*/
	if(hasScore)
	{
		tabbedComponent->setBounds (0, getHeight() / 2 , getWidth(), getHeight() / 2);
		scoreTable->setBounds (getWidth() - 200,50,200,getHeight()/ 2 - 30);
		if (Globals::getInstance()->getShowScore())
			scoreImage->setBounds(0,0,getWidth() - 200, getHeight() / 2);
	}
	else
	{
		tabbedComponent->setBounds (0, 50, getWidth(), getHeight() - 50);
	}
    playButton->setBounds(getWidth()-100,0,50,50);
	stopButton->setBounds(getWidth()-50,0,50,50);
	//[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AudioTabComponent::afterMetronome()
{
	if (hasScore)
	{
		deviceManager.removeAudioCallback (sineComp);
		deleteAndZero(sineComp);
	}
	m_soundInput->isPaused = false;
	recorder = new AudioRecorder();
	deviceManager.addAudioCallback (recorder);
	recorder->startRecording (directory.getNonexistentChildFile("rawSamples", ".wav"));
	isRecording = true;
	m_spectroPage->playClicked(directory);
	m_pitchPage->playClicked(directory, &(actualPitchPosition));
	if (Globals::getInstance()->getShowScore())
	{
		if (hasScore)
			scoreImage->playClicked();
	}
	stopButton->setEnabled(true);
}

ScoreTable::ScoreTable()
{
	pitchDescription = new Label(String::empty, L"pitch");
	rythmDescription = new Label(String::empty, L"rythm");
	pitchValue = new Label(String::empty, L"0%");
	rythmValue = new Label(String::empty, L"0%");

	pitchDescription->setFont(Font (14.0000f, Font::plain));
    pitchDescription->setJustificationType (Justification::bottomLeft);
    pitchDescription->setEditable (false, false, false);
    pitchDescription->setColour (TextEditor::textColourId, Colours::black);
    pitchDescription->setColour (TextEditor::backgroundColourId, Colour (0x0));

	rythmDescription->setFont(Font (14.0000f, Font::plain));
    rythmDescription->setJustificationType (Justification::bottomLeft);
    rythmDescription->setEditable (false, false, false);
    rythmDescription->setColour (TextEditor::textColourId, Colours::black);
    rythmDescription->setColour (TextEditor::backgroundColourId, Colour (0x0));

	pitchValue->setFont(Font (14.0000f, Font::plain));
    pitchValue->setJustificationType (Justification::bottomLeft);
    pitchValue->setEditable (false, false, false);
    pitchValue->setColour (TextEditor::textColourId, Colours::black);
    pitchValue->setColour (TextEditor::backgroundColourId, Colour (0x0));

	rythmValue->setFont(Font (14.0000f, Font::plain));
    rythmValue->setJustificationType (Justification::bottomLeft);
    rythmValue->setEditable (false, false, false);
    rythmValue->setColour (TextEditor::textColourId, Colours::black);
    rythmValue->setColour (TextEditor::backgroundColourId, Colour (0x0));

	addAndMakeVisible(pitchDescription);
	addAndMakeVisible(pitchValue);
	addAndMakeVisible(rythmDescription);
	addAndMakeVisible(rythmValue);
}

ScoreTable::~ScoreTable()
{
	deleteAndZero(pitchDescription);
	deleteAndZero(rythmDescription);
	deleteAndZero(pitchValue);
	deleteAndZero(rythmValue);
}

void ScoreTable::paint(Graphics& g)
{
	g.fillAll(Colours::lightgrey);
}

void ScoreTable::resized()
{
	pitchDescription->setBounds(getWidth() / 100,10,100,20);
	rythmDescription->setBounds(getWidth() / 100,40,100,20);

	pitchValue->setBounds(150,10,50,20);
	rythmValue->setBounds(150,40,50,20);
}

void ScoreTable::updateScores()
{
	int whole;
	int rest;
	String tempString("");

	//pitch
	whole = (int)pitchPercentage;
	rest = (int)((pitchPercentage - whole) * 100);
	tempString += whole;
	tempString += ",";
	tempString += rest;
	tempString += "%";
	pitchValue->setText(tempString, sendNotification);
	tempString = "";
	
	//rythm
	whole = (int)rythmPercentage;
	rest = (int)((rythmPercentage - whole) * 100);
	tempString += whole;
	tempString += ",";
	tempString += rest;
	tempString += "%";
	rythmValue->setText(tempString, sendNotification);
	tempString = "";

	repaint();
}


//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//==============================================================================
Component* createAudio()
{
    return new AudioTabComponent();
}
Component* createAudio(Array<ScorePart> score)
{
    return new AudioTabComponent(score);
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AudioDemoTabComponent" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330000013"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ffffffff"/>
  <TABBEDCOMPONENT name="new tabbed component" id="83c980d7793cdced" memberName="tabbedComponent"
                   virtualName="" explicitFocusOrder="0" pos="0 0 0M 0M" orientation="top"
                   tabBarDepth="30" initialTab="0">
    <TAB name="Audio Device Setup" colour="ffd3d3d3" useJucerComp="1"
         contentClassName="" constructorParams="deviceManager" jucerComponentFile="AudioDemoSetupPage.cpp"/>
    <TAB name="File Playback" colour="ffd3d3d3" useJucerComp="1" contentClassName=""
         constructorParams="deviceManager" jucerComponentFile="AudioDemoPlaybackPage.cpp"/>
    <TAB name="Synth Playback" colour="ffd3d3d3" useJucerComp="1" contentClassName=""
         constructorParams="deviceManager" jucerComponentFile="AudioDemoSynthPage.cpp"/>
    <TAB name="Latency Test" colour="ffd3d3d3" useJucerComp="1" contentClassName=""
         constructorParams="deviceManager" jucerComponentFile="AudioDemoLatencyPage.cpp"/>
    <TAB name="Recording" colour="ffd3d3d3" useJucerComp="1" contentClassName=""
         constructorParams="deviceManager" jucerComponentFile="AudioDemoRecordPage.cpp"/>
  </TABBEDCOMPONENT>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
