#include "AudioReaderComponent.h"


AudioReaderComponentLive::AudioReaderComponentLive(AudioDeviceManager& deviceManager_)
    : deviceManagerPlayback (deviceManager_),
	  thread ("reading data from files")
{
	std::string folderName = "data";
	File directory = File(File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getParentDirectory().getParentDirectory().getChildFile (folderName.c_str()));
	
	fileChooser = new FileChooser("Choose file...",directory);
	if(!fileChooser->browseForDirectory())
		return;
	File directoryFile = fileChooser->getResult();
	if(!directoryFile.exists())
		return;
	wavFile = directoryFile.getChildFile("rawSamples.wav");
	spectroFile = directoryFile.getChildFile("spectroSamples.smp");
	
    deviceManagerPlayback.addAudioCallback (&audioSourcePlayer);
    audioSourcePlayer.setSource (&transportSource);

    ScopedPointer<FileInputStream> fileIn=new FileInputStream(directoryFile.getChildFile("rawSamples.wav"));
	WavAudioFormat wavFormat;
	
	
    transportSource.stop();
    transportSource.setSource (nullptr);
    currentAudioFileSource = nullptr;

	AudioFormatReader* reader = wavFormat.createReaderFor(fileIn,false);
	
    if (reader != nullptr)
    {
        currentAudioFileSource = new AudioFormatReaderSource (reader, true);

        // ..and plug it into our transport source
        transportSource.setSource (currentAudioFileSource,
                                   32768, // tells it to buffer this many samples ahead
                                   &thread, // this is the background thread to use for reading-ahead
                                   reader->sampleRate);
    }
	
	addAndMakeVisible(pitchDisplay = new PitchDisplay());
	addAndMakeVisible(spectrogramDisplay = new SpectrogramDisplay());

	pitchDisplay->setBounds (0,0, getWidth(), getHeight()/2);
	spectrogramDisplay->setBounds (0,getHeight()/2, getWidth(), getHeight());
    startTimer (1000 / 50); // use a timer to keep repainting this component
	
            transportSource.setPosition (0);
            transportSource.start();

}

AudioReaderComponentLive::~AudioReaderComponentLive(void)
{
    
	transportSource.setSource (nullptr);
    audioSourcePlayer.setSource (nullptr);

    deviceManagerPlayback.removeAudioCallback (&audioSourcePlayer);

	deleteAndZero(pitchDisplay);
	deleteAndZero(spectrogramDisplay);
	deleteAndZero(fileChooser);
}
/*
void AudioReaderComponentLive::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::blue);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}
*/
void AudioReaderComponentLive::resized()
{
	//this->setBounds(0,0,getWidth(),getHeight());
	pitchDisplay->setBounds (0,0, getWidth(), getHeight()/2);
	spectrogramDisplay->setBounds (0,getHeight()/2, getWidth(), getHeight());
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AudioReaderComponentLive::timerCallback()
{
    repaint();
}

void AudioReaderComponentLive::audioDeviceAboutToStart (AudioIODevice*)
{
    zeromem (samples, sizeof (samples));
}

void AudioReaderComponentLive::audioDeviceStopped()
{
    zeromem (samples, sizeof (samples));
}

void AudioReaderComponentLive::audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                                                       float** outputChannelData, int numOutputChannels, int numSamples)
{

}

SpectrogramDisplay::SpectrogramDisplay(void)
{

}

SpectrogramDisplay::~SpectrogramDisplay(void)
{

}

void SpectrogramDisplay::paint (Graphics& g)
{
    g.fillAll (Colours::red);
}

PitchDisplay::PitchDisplay(void)
{

}

PitchDisplay::~PitchDisplay(void)
{

}

void PitchDisplay::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

AudioReaderComponent::AudioReaderComponent(void)
{
	
    deviceManager.initialise (2, 2, 0, true, String::empty, 0);
	
	addAndMakeVisible(audioReader = new AudioReaderComponentLive(deviceManager));
	audioReader->setBounds(0,0,getWidth(),getHeight());
    //deviceManager.addAudioCallback (audioReader);
}


AudioReaderComponent::~AudioReaderComponent(void)
{
    //deviceManager.removeAudioCallback (audioReader);
    deleteAndZero (audioReader);
}
/*
void AudioReaderComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::red);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}
*/
void AudioReaderComponent::resized()
{
    //tabbedComponent->setBounds (0,0, getWidth(), getHeight());//getHeight()/2
	audioReader->setBounds(0,0,getWidth(),getHeight());
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

Component* createAudioReader()
{
    return new AudioReaderComponent();
}
