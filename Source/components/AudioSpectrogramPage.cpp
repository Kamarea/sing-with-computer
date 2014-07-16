#include "AudioSpectrogramPage.h"
#include "fft.h"
#include <algorithm>

#include "logs.h"
//#include <fftw3.h>

LiveAudioSpectrogramDisplayComp* LiveAudioSpectrogramDisplayComp::instance;

//[MiscUserDefs] You can add your own user definitions and misc code here...
LiveAudioSpectrogramDisplayComp::LiveAudioSpectrogramDisplayComp(std::string folderName)
{
    nextSample = subSample = 0;
    accumulator = 0;
    //zeromem (samples, sizeof (samples));
    setOpaque (true);
	counter=0;
	shiftNumber=0;
	image = Image(Image::RGB,2048,1024,true);
	image.clear(Rectangle<int>(2048,1024),Colours::blue);
	
	std::string fileName = folderName.append("\\spectroSamples.smp");

	fileOutput = new FileOutputStream(File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getParentDirectory().getChildFile(fileName.c_str()));

	allSpectroSamples.clear();
    startTimer (1000 / 5); // use a timer to keep repainting this component
}

LiveAudioSpectrogramDisplayComp::~LiveAudioSpectrogramDisplayComp()
{
	String line;
	for (int i=0;i<allSpectroSamples.size();i++)
	{
		line = "";
		// write spectrogram to output file
		for(int j=0;j<1024;++j)
		{
			line+=String(allSpectroSamples[i][j]);
			line+=',';
		}
		line+="\n";
		fileOutput->write(line.toWideCharPointer(),line.length());
	}

	delete fileOutput;
}

int max(std::vector<float> table, int size)
{
	float max=0;
	int index=-1;
	for (int i=0;i<table.size();++i)
	{
		if (table[i] > max)
		{
			max = table[i];
			index=i;
		}
	}
	return index;
}

void LiveAudioSpectrogramDisplayComp::paint (Graphics& g)
{	
	//int size = allSpectroSamples.size();
	lock.enter();
	int localShiftNumber = shiftNumber;
	lock.exit();
	//shiftNumber = 0;
	//g.setColour(Colours::gold);
	image.moveImageSection(0,0,localShiftNumber,0,2048-localShiftNumber,1024);
	//image.clear(Rectangle<int>(localShiftNumber,1024),Colours::gold);
	/*for(int y=0;y<1024;y++)
		{
			image.setPixelAt(2046,y,Colours::white);
		}*/
	/*int step=1;
	if (localShiftNumber>100)
		step=2;
	if(localShiftNumber>200)
		step=3;
	if(localShiftNumber>500)
		step=4;*/
	for (int i=0; i<localShiftNumber;i++)
	{
		lock.enter();
		int size = allSpectroSamples.size();
/*		String line;
		// write spectrogram to output file
		for(int j=0;j<std::min(1024,(int)(allSpectroSamples.size()));++j)
		{
			line+=String(allSpectroSamples[allSpectroSamples.size()-(shiftNumber-i)*1024 +j]);
			line+=',';
		}
		line+="\n";
		fileOutput->write(line.toWideCharPointer(),line.length());
*/		
		if(allSpectroSamples[size-shiftNumber+i].size() != 1024)
			return;
		for (int j=0;j<1024;j++)
			image.setPixelAt(2047-localShiftNumber+i, j, Colour::fromHSV(0.66+(float)allSpectroSamples[size-(shiftNumber-i)][j]/100,1,1,1));
		lock.exit();
		/*for(int y=0;y<1024;y++)
		{
			image.setPixelAt(2047-i,y,Colour::fromHSV(0.66+(float)paintSamples[2048-shiftNumber+i][y]/100,1,1,1));
		}*/
	}
	
	for(int i=0;i<200;++i)
	{
		image.setPixelAt(2045-i,1003,Colours::white);
		
	}
	lock.enter();
	shiftNumber = shiftNumber - localShiftNumber;
	lock.exit();
	g.drawImage(image,0,0,getWidth(),getHeight(),2048-getWidth(),1024-getHeight(),getWidth(),getHeight());
}

void LiveAudioSpectrogramDisplayComp::timerCallback()
{
    repaint();
}

void LiveAudioSpectrogramDisplayComp::audioDeviceAboutToStart (AudioIODevice*)
{
    zeromem (samples, sizeof (samples));
}

void LiveAudioSpectrogramDisplayComp::audioDeviceStopped()
{
    zeromem (samples, sizeof (samples));
}

void LiveAudioSpectrogramDisplayComp::audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                                                       float** outputChannelData, int numOutputChannels, int numSamples)
{
	//static float temp[1024];
	//static float temp2[1024];

    const int numSubSamples = 1; // how many input samples go onto one pixel.
    const float boost = 5.0f;     // how much to boost the levels to make it more visible.

	for(int i=0;i<1024;i++)
		samples[i]=0.0f;

	int offset = 0;
    for (int i = 0; i < 2560; i+=1)//++i)
    {
        for (int chan = 0; chan < numInputChannels; ++chan)
        {
            if (inputChannelData[chan] != 0)
                accumulator += fabsf (inputChannelData[chan][i]);
        }

        if (i-offset < 1024)
        {
            samples[i-offset] = accumulator * boost;
            accumulator = 0;
        }
		if (i-offset == 1024)
		{	
			// prepare samples for fft
			for(int i=0;i<2048;i++) 
				transformSamples[i]=0.0;
			for(int i=0;i<1024;i++)
				transformSamples[2*i]=samples[i];

			fft1024(transformSamples);

			//float tmp[1024];
			for (int i=0; i<1024; ++i)
				spectroSamples[i]=transformSamples[2*i]*transformSamples[2*i]+transformSamples[2*i+1]*transformSamples[2*i+1];
			/*for(int j=0;j<2047;++j)
			{
				for(int i=0;i<1024;++i)
				{
					paintSamples[j][i]=paintSamples[j+1][i];
				}
			}
			for(int i=0;i<1024;++i)
				paintSamples[2047][i]=spectroSamples[i
				*/
			//int size = allSpectroSamples.size();
			//allSpectroSamples.resize(size+1024);
			lock.enter();
			std::vector<float> tmp;
			tmp.clear();
			tmp.resize(1024,0.0f);
			for(int i=0;i<1024;i++)
				tmp[i]=spectroSamples[i];
			//for(int i=0;i<1024;++i)
			allSpectroSamples.push_back(tmp);
			shiftNumber++;
			lock.exit();
			// compute spectrogram
			//spectroSamples = std::vector<float>();
			//spectroSamples.reserve(1024);
			//for(int i=0;i<1024;i++)
			//	spectroSamples.push_back(0.0f);//tmp[i]);
			/*std::vector<float> tmp2;
			tmp2.resize(1024);
			for(int i=0;i<1024;i++)
				tmp2[i]=tmp[i];

			allSpectroSamples.push_back(tmp2);
			shiftNumber++;
			*/
		/*String line;
		// write spectrogram to output file
		for(int j=0;j<1024;++j)
		{
			line+=String(paintSamples[2048-shiftNumber+i][j]);
			line+=',';
		}
		line+="\n";
		fileOutput->write(line.toWideCharPointer(),line.length());
		*/
			//shiftNumber++;
			// new sample
			for(int j=0;j<512;++j)
				samples[j]=samples[j+512];
			samples[512] = accumulator * boost;
			offset += 512;
		}
		
    }

    // We need to clear the output buffers, in case they're full of junk..
    for (int i = 0; i < numOutputChannels; ++i)
        if (outputChannelData[i] != 0)
            zeromem (outputChannelData[i], sizeof (float) * numSamples);
}



AudioSpectrogramPage::AudioSpectrogramPage (AudioDeviceManager& deviceManager_,std::string folderName)
    : deviceManager (deviceManager_),
      liveAudioSpectrogramDisplayComp (0)
{
	addAndMakeVisible (liveAudioSpectrogramDisplayComp = LiveAudioSpectrogramDisplayComp::getInstance(folderName));

    //[UserPreSize]
    //[/UserPreSize]


    //[Constructor] You can add your own custom stuff here..
    //recorder = new AudioRecorder();
    //deviceManager.addAudioCallback (recorder);
    deviceManager.addAudioCallback (liveAudioSpectrogramDisplayComp);
    //[/Constructor]
}

AudioSpectrogramPage::~AudioSpectrogramPage()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //deviceManager.removeAudioCallback (recorder);
    deviceManager.removeAudioCallback (liveAudioSpectrogramDisplayComp);
    ////recorder = 0;
    //[/Destructor_pre]

    deleteAndZero (liveAudioSpectrogramDisplayComp);
    //deleteAndZero (explanationLabel);
    //deleteAndZero (recordButton);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AudioSpectrogramPage::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::lightgrey);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AudioSpectrogramPage::resized()
{
    liveAudioSpectrogramDisplayComp->setBounds (5, 5, getWidth() - 10, getHeight()-10);
    //explanationLabel->setBounds (160, 150, getWidth() - 169, 216);
    //recordButton->setBounds (8, 88, 136, 40);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

