#include "AudioSpectrogramPage.h"
#include "fft.h"
#include <algorithm>

#include "logs.h"

AudioSpectrogramPage* AudioSpectrogramPage::instance;

//[MiscUserDefs] You can add your own user definitions and misc code here...
AudioSpectrogramPage::AudioSpectrogramPage()
{
	numberOfSamplesRead = 0;
	numberOfSamplesRecalculated = 512;
    setOpaque (true);
	counter=0;
	shiftNumber=0;
	image = Image(Image::RGB,2048,1024,true);
	image.clear(Rectangle<int>(2048,1024),Colours::blue);
	isRecording = false;

	spectroNoise = Globals::getInstance()->getSpectroNoise();

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
			image.setPixelAt(2047-localShiftNumber+i, j, Colour::fromHSV(0.66+
					(float)allSpectroSamples[size-(shiftNumber-i)][j],1,1,1));
		lock.exit();
	}

	g.drawImage(image,0,0,getWidth(),getHeight(),2048-getWidth(),1024-getHeight(),getWidth(),getHeight());
	lock.enter();
	shiftNumber = shiftNumber - localShiftNumber;
	lock.exit();
}

void AudioSpectrogramPage::timerCallback()
{
    repaint();
}

void AudioSpectrogramPage::updateSamples(int number, std::vector<float>* samples)
{
	std::vector<float> basicSamples;
	std::vector<float> preemphasisSamples;
	basicSamples.resize(1024);
	preemphasisSamples.resize(1024);

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
				for (int i = 0; i < 1024; i++)
					basicSamples[i] = allSamples[numberOfSamplesRecalculated - 512 + i];
				preemphasisSamples[0] = basicSamples[0];
				for (int i = 1; i < 1024; i++)
					preemphasisSamples[i] = basicSamples[i] - phi * basicSamples[i - 1];

				for (int i = 0; i < 2048; i++)
					transformSamples[i] = 0.0;
				for (int i = 0; i < 1024; i++)
					transformSamples[2 * i] = preemphasisSamples[i];
				
				fft1024(transformSamples);

				for (int i = 0; i < 1024; ++i)
					spectroSamples[i] = transformSamples[2 * i] * transformSamples[2 * i] +
								transformSamples[2 * i + 1] * transformSamples[2 * i + 1];

				spectroLine.clear();
				spectroLine.resize(1024,0.0f);
				for (int i = 0; i < 1024; i++)
					spectroLine[i] = spectroSamples[i];

				// noise removal
				for (int i = 0; i < 1024; i++)
					spectroLine[i] = std::max<float>(0.0, spectroLine[i] - spectroNoise[i]);

				//calculatePitch(spectroLine);

				lock.enter();
				allSpectroSamples.push_back(spectroLine);
				//pitchFreq.push_back(index);
				shiftNumber++;
				lock.exit();

				
				numberOfSamplesRecalculated += 512;
			}
		}
	}
}

int indexOfMaxInVector(std::vector<float> samples)
{
	int index = -1;
	float max = 0;
	for (int i = 0; i < samples.size(); i++)
	{
		if (samples[i] > max)
		{
			max = samples[i];
			index = i;
		}
	}
	return index;
}

// to jest ta funkcja, która nie dzia³a dobrze
void AudioSpectrogramPage::calculatePitch(std::vector<float> samples)
{
	std::vector<float> spectroCopy;
	spectroCopy.resize(1024);
	for (int i = 0; i < 1024; i++)
		spectroCopy[i] = samples[i];

	std::vector<int> maxes;
	int maxesIndex = 0;

	for (maxesIndex = 0; maxesIndex < 15; maxesIndex++)
	{
		int index = indexOfMaxInVector(spectroCopy);
		if (index >= 0)
		{
			maxes.push_back(index);
			spectroCopy[maxes[maxesIndex]] = 0;
			int k = maxes[maxesIndex] + 1;
			while ((k > 0) && (k < samples.size()) && samples[k - 1] > samples[k])
			{
				spectroCopy[k] = 0;
				k++;
			}
			k = maxes[maxesIndex] - 1;
			while ((k < samples.size() - 1) && (k > 0) && samples[k + 1] > samples[k])
			{
				spectroCopy[k] = 0;
				k--;
			}
		}
	}
	std::sort(maxes.begin(),maxes.end());
	
	float freq = -1;
	if ((maxes[1] - maxes[0] >= maxes[2] - maxes[1] - 1) && 
			(maxes[1] - maxes[0] <= maxes[2] - maxes[1] + 1))
		freq = maxes[1] * spectrumResolution - maxes[0] * spectrumResolution;

	lock.enter();
	pitchFreq.push_back(freq);
	lock.exit();
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
	
	for (int i=begin;i<end;i++)
	{
		fileOutput->write(allSpectroSamples[i].data(),2048);
	}
	isRecording = false;
	lock.exit();
	delete fileOutput;
}
