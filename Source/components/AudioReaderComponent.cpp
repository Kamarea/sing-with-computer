#include "AudioReaderComponent.h"

AudioReaderComponentLive::AudioReaderComponentLive(AudioDeviceManager& deviceManager_)
    : deviceManagerPlayback (deviceManager_),
	  counter(0),
	  reading(true),
	  thread ("reading data from files"),
	  directoryList(0, thread)
{
    formatManager.registerBasicFormats();
	std::string folderName = "data\\";
	File directory = File(File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getParentDirectory().getChildFile (folderName.c_str()).getChildFile(Globals::getInstance()->getUser()));
	
    addAndMakeVisible (fileTreeComp = new FileTreeComponent (directoryList));
	directoryList.setDirectory (directory, true, true);
    thread.startThread (3);

    fileTreeComp->setColour (FileTreeComponent::backgroundColourId, Colours::white);
    fileTreeComp->addListener (this);

    deviceManagerPlayback.addAudioCallback (&audioSourcePlayer);
    audioSourcePlayer.setSource (&transportSource);
}

AudioReaderComponentLive::~AudioReaderComponentLive(void)
{
    transportSource.stop();
	transportSource.setSource (nullptr);
    audioSourcePlayer.setSource (nullptr);

    deviceManagerPlayback.removeAudioCallback (&audioSourcePlayer);
    fileTreeComp->removeListener (this);

	deleteAndZero(pitchDisplay);
	deleteAndZero(spectrogramDisplay);
}

void AudioReaderComponentLive::loadFileIntoTransport (const File& audioFile)
{
    transportSource.stop();
    transportSource.setSource (nullptr);
    currentAudioFileSource = nullptr;

    AudioFormatReader* reader = formatManager.createReaderFor (audioFile);

    if (reader != nullptr)
    {
        currentAudioFileSource = new AudioFormatReaderSource (reader, true);

        transportSource.setSource (currentAudioFileSource,
                                   32768,
                                   &thread,
                                   reader->sampleRate);
    }
}

void AudioReaderComponentLive::showFile (const File& file)
{

    loadFileIntoTransport (file);
	pitchFile = file.getParentDirectory().getChildFile("pitchSamples.smp");
	spectroFile = file.getParentDirectory().getChildFile("spectroSamples.smp");
	
	AudioDeviceManager::AudioDeviceSetup setup;
	deviceManagerPlayback.getAudioDeviceSetup(setup);
	addAndMakeVisible(pitchDisplay = new PitchDisplay(pitchFile, setup.sampleRate));
	addAndMakeVisible(spectrogramDisplay = new SpectrogramDisplay(spectroFile, setup.sampleRate));
	reading = false;
	
	pitchDisplay->setBounds (0,0, getWidth(), getHeight()/2);
	spectrogramDisplay->setBounds (0,getHeight()/2, getWidth(), getHeight()/2);

	transportSource.setPosition (0);
    transportSource.start();

	startTimer (2560 * 1000 / setup.sampleRate); // use a timer to keep repainting displays
}

void AudioReaderComponentLive::selectionChanged()
{
    showFile (fileTreeComp->getSelectedFile());
}

void AudioReaderComponentLive::fileClicked (const File&, const MouseEvent&)
{
}

void AudioReaderComponentLive::fileDoubleClicked (const File&)
{
}

void AudioReaderComponentLive::resized()
{
	this->setBounds(0,0,getWidth(),getHeight());
	if(reading)
		fileTreeComp->setBounds (16, 8, getWidth() - 32, getHeight() - 8);
	else
	{
		pitchDisplay->setBounds (0,0, getWidth(), getHeight()/2);
		spectrogramDisplay->setBounds (0,getHeight()/2, getWidth(), getHeight()/2);
	}
}

void AudioReaderComponentLive::timerCallback()
{
	pitchDisplay->updatePosition(16);
	spectrogramDisplay->updatePosition(16);
    repaint();
}

SpectrogramDisplay::SpectrogramDisplay(File inFile, int samplerate)
	: actualPosition(0),
	localActualPosition(0),
	sampleRate(samplerate)
{
	FileInputStream* stream = new FileInputStream(inFile);
	float sample;
	int count = 0;
	std::vector<float> spectroSample;
	spectroSample.clear();

	while(!stream->isExhausted())
	{
		sample = stream->readFloat();
		spectroSample.push_back(sample);
		count++;
		if(count == 1024)
		{
			spectros.push_back(spectroSample);
			spectroSample.clear();
			count = 0;
		}
	}

	deleteAndZero(stream);
	image = Image(Image::RGB,2048,1024,true);
	image.clear(Rectangle<int>(2048,1024),Colours::blue);
}

SpectrogramDisplay::~SpectrogramDisplay(void)
{

}

void SpectrogramDisplay::updatePosition(int positionChange)
{
	lock.enter();
	actualPosition += positionChange;
	lock.exit();
}

void SpectrogramDisplay::paint (Graphics& g)
{
	int lastPosition = localActualPosition;
	lock.enter();
	localActualPosition = actualPosition;
	lock.exit();
    g.fillAll (Colours::red);
	
	int localShiftNumber = localActualPosition - lastPosition;
	image.moveImageSection(0,0,localShiftNumber,0,2048-localShiftNumber,1024);
	if( localActualPosition > spectros.size())
		return;
	for (int i=0; i<localShiftNumber;i++)
	{


		for (int j=0;j<1024;j++)		
			image.setPixelAt(2047-localShiftNumber+i, j, Colour::fromHSV(0.66+(float)spectros[localActualPosition-(localShiftNumber-i)][j],1,1,1));

	}
	g.drawImage(image,0,0,getWidth(),getHeight(),2048-getWidth(),1024-getHeight(),getWidth(),getHeight());
	
	g.drawFittedText(String(getHeight()),getWidth()/2,100,100,100,Justification::left,1);
	g.drawFittedText(String(getWidth()),getWidth()/2,150,100,100,Justification::left,1);
}

PitchDisplay::PitchDisplay(File inFile, int samplerate)
	: actualPosition(0),
	temp(0),
	sampleRate(samplerate)
{
	pitches.clear();
	FileInputStream* stream = new FileInputStream(inFile);
	float sample;
	int bytesRead = 256;
	while(!stream->isExhausted())
	{
		sample = stream->readFloat();
		pitches.push_back(sample);
	}
	
	deleteAndZero(stream);
}

PitchDisplay::~PitchDisplay(void)
{
}

const String pitchNames[] = {"1A","1AIS","1B",
				"C","CIS","D","DIS","E","F","FIS","G","GIS","A","AIS","B",
				"c","cis","d","dis","e","f","fis","g","gis","a","ais","b",
				"c1","cis1","d1","dis1","e1","f1","fis1","g1","gis1","a1","ais1","b1",
				"c2","cis2","d2","dis2","e2","f2","fis2","g2","gis2","a2","ais2","b2",
				"c3","cis3","d3","dis3","e3","f3","fis3","g3","gis3","a3","ais3","b3"};
const int pitchCount = 51;

void PitchDisplay::updatePosition(int positionChange)
{
	actualPosition += positionChange;
}

void PitchDisplay::paint (Graphics& g)
{
    g.fillAll (Colours::white);

    const float midY = getHeight() * 0.5f;
	float oneHzSize = (float)getHeight() / 1800.0f;
		
	float semitone = (float)getHeight()/pitchCount;
	
	if( actualPosition > pitches.size())
		return;
	// draw background
	g.setFont(semitone);
	for (float i=0.5;i<pitchCount;++i)
	{
		if((int)i%2==0)
		{
			g.setColour(Colours::lightgrey);
			g.fillRect(0.0, i*semitone, (float)getWidth(), semitone);
		}
		else
		{
			g.setColour(Colours::white);
			g.fillRect(0.0, i*semitone, (float)getWidth(), semitone);
		}
		g.setColour(Colours::black);
		g.drawFittedText(pitchNames[pitchCount-(int)i-1],0,(int)((i)*semitone),50,(int)semitone,Justification::left,1);
		g.drawFittedText(pitchNames[pitchCount-(int)i-1],getWidth()-50,(int)((i)*semitone),50,(int)semitone,Justification::right,1);
	}
	//g.drawFittedText(String(actualPosition),getWidth()/2,100,100,100,Justification::left,1);

	// draw pitches
	g.setColour(Colours::blue);
	
	// wczytaæ ile jest do narysowania - max getWidth();
	// dla tylu ile jest do  narysowania narysowaæ
	int drawNumber = actualPosition > (getWidth() - 40) ? 
		(actualPosition - getWidth() + 40) : actualPosition;
    for (int x = 0; x < drawNumber; ++x)
    {
		float pitch = pitches[actualPosition - drawNumber + x];
        int y=(float)getHeight() - (pitch - 33) * semitone;
        g.drawVerticalLine (getWidth() - 20 - drawNumber + x, y, y + 1);
    }
}

AudioReaderComponent::AudioReaderComponent(void)
{
	
    deviceManager.initialise (2, 2, 0, true, String::empty, 0);
	
	addAndMakeVisible(audioReader = new AudioReaderComponentLive(deviceManager));

}


AudioReaderComponent::~AudioReaderComponent(void)
{
    deleteAndZero (audioReader);
}

void AudioReaderComponent::resized()
{
	audioReader->setBounds(0,0,getWidth(), getHeight());
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

Component* createAudioReader()
{
    return new AudioReaderComponent();
}
