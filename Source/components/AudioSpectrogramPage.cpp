#include "AudioSpectrogramPage.h"
#include "fft.h"
#include <algorithm>

#include "logs.h"
//#include <fftw3.h>

AudioSpectrogramPage* AudioSpectrogramPage::instance;

//[MiscUserDefs] You can add your own user definitions and misc code here...
AudioSpectrogramPage::AudioSpectrogramPage()
{
    //nextSample = subSample = 0;
    //accumulator = 0;
    //zeromem (samples, sizeof (samples));
	numberOfSamplesRead = 0;
	numberOfSamplesRecalculated = 512;
    setOpaque (true);
	counter=0;
	shiftNumber=0;
	image = Image(Image::RGB,2048,1024,true);
	image.clear(Rectangle<int>(2048,1024),Colours::blue);
	isRecording = false;

	allSpectroSamples.clear();
    startTimer (1000 / 5); // use a timer to keep repainting this component
}

AudioSpectrogramPage::~AudioSpectrogramPage()
{
	lock.enter();
	if(isRecording)
		stopClicked();
	lock.exit();
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

void AudioSpectrogramPage::paint (Graphics& g)
{	
	lock.enter();
	int localShiftNumber = shiftNumber;
	lock.exit();
	image.moveImageSection(0,0,localShiftNumber,0,2048-localShiftNumber,1024);
	
	for (int i=0; i<localShiftNumber;i++)
	{
		lock.enter();
		int size = allSpectroSamples.size();

		if(allSpectroSamples[size-shiftNumber+i].size() != 1024)
			return;
		for (int j=0;j<1024;j++)
			image.setPixelAt(2047-localShiftNumber+i, j, Colour::fromHSV(0.66+(float)allSpectroSamples[size-(shiftNumber-i)][j]/100,1,1,1));
		lock.exit();
	}
	
	lock.enter();
	shiftNumber = shiftNumber - localShiftNumber;
	lock.exit();
	g.drawImage(image,0,0,getWidth(),getHeight(),2048-getWidth(),1024-getHeight(),getWidth(),getHeight());
}

void AudioSpectrogramPage::timerCallback()
{
    repaint();
}

void AudioSpectrogramPage::updateSamples(int number, std::vector<float>* samples)
{
	if(numberOfSamplesRead < number)
	{
		for (int i = numberOfSamplesRead; i < number; i++)
		{
			allSamples.push_back((*samples)[i]);
		}
		numberOfSamplesRead = number;

		float tempSamples[2048];
		std::vector<float> spectroLine;
		if (numberOfSamplesRead >= 3 * 512)
		{
			while(number - numberOfSamplesRecalculated >= 1024)
			{
				for (int i = 0; i < 2048; i++)
					transformSamples[i] = 0.0;
				for (int i = 0; i < 1024; i++)
					transformSamples[2 * i] = allSamples[numberOfSamplesRecalculated - 512 + i];
				
				fft1024(transformSamples);

				for (int i = 0; i < 1024; ++i)
					spectroSamples[i] = transformSamples[2 * i] * transformSamples[2 * i] +
								transformSamples[2 * i + 1] * transformSamples[2 * i + 1];

				spectroLine.clear();
				spectroLine.resize(1024,0.0f);
				for (int i = 0; i < 1024; i++)
					spectroLine[i] = spectroSamples[i];

				lock.enter();
				allSpectroSamples.push_back(spectroLine);
				shiftNumber++;
				lock.exit();
				
				numberOfSamplesRecalculated += 512;
			}
		}
	}
}

void AudioSpectrogramPage::playClicked(File directory)
{
	fileOutput = new FileOutputStream(directory.getChildFile("spectroSamples.smp"));
	lock.enter();
	begin = allSpectroSamples.size();
	isRecording=true;
	lock.exit();
}

void AudioSpectrogramPage::stopClicked()
{
	lock.enter();
	end = allSpectroSamples.size();
	String line;
	// TODO change
	for (int i=begin;i<end;i++)
	{
		fileOutput->write(allSpectroSamples[i].data(),2048);
	}
	isRecording = false;
	lock.exit();
	delete fileOutput;
}
