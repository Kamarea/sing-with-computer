#include "NoiseMeasurement.h"
#include "fft.h"

class NoiseButtonListener : public Button::Listener
{
public:
	NoiseButtonListener(NoiseMeasurement* comp_)
	{
		comp = comp_;
	}
	void buttonClicked(Button* button)
	{
		if (button->getButtonText() == "START")
		{
			// start clicked
			comp->addCallback();
		}
		else if (button->getButtonText() == "STOP")
		{
			// stop clicked
			comp->removeCallback();
			comp->calculateNoise();
		}
	}
	NoiseMeasurement* comp;
};

NoiseMeasurement::NoiseMeasurement()
{
	NoiseButtonListener* listener = new NoiseButtonListener(this);
	startButton = new TextButton();
	startButton->setButtonText("START");
	startButton->addListener(listener);
	addAndMakeVisible(startButton);
	stopButton= new TextButton;
	stopButton->setButtonText("STOP");
	stopButton->addListener(listener);
	addAndMakeVisible(stopButton);
	
    setSize (getWidth(), getHeight());
	startButton->setBounds(getWidth() / 2 - 55,getHeight()/2 - 20,50,20);
	stopButton->setBounds(getWidth() / 2 + 55,getHeight()/2 - 20,50,20);
	
	soundInput = SoundInput::getInstance();
	
    deviceManager.initialise (2, 2, 0, true, String::empty, 0);
}

NoiseMeasurement::~NoiseMeasurement()
{
	deleteAndZero(startButton);
	deleteAndZero(stopButton);
}

void NoiseMeasurement::resized()
{
	startButton->setBounds(getWidth() / 2 - 55,getHeight()/2 - 20,50,20);
	stopButton->setBounds(getWidth() / 2 + 55,getHeight()/2 - 20,50,20);
}

void NoiseMeasurement::addCallback()
{
	deviceManager.addAudioCallback(soundInput);
}

void NoiseMeasurement::removeCallback()
{
	deviceManager.removeAudioCallback(soundInput);
}

void NoiseMeasurement::calculateNoise()
{
	soundInput->lock.enter();
	for (int i = 0; i < soundInput->allSamples.size(); i++)
	{
		allSamples.push_back(soundInput->allSamples[i]);
	}
	soundInput->lock.exit();
	deleteAndZero(soundInput);

	// mean amplitude
	meanAmplitude = 0;
	for (int i = 0; i < allSamples.size(); i++)
		meanAmplitude += allSamples[i];
	meanAmplitude /= allSamples.size();

	Globals::getInstance()->setMeanNoise(meanAmplitude);

	// mean spectrogram
	float tempSamples[2048];
	std::vector<float> spectroLine;
	int numberOfSamplesRecalculated = 512;
	float basicSamples[1024];
	float preemphasisSamples[1024];
	float transformSamples[2048];
	float spectroSamples[2048];
	std::vector<std::vector<float>> allSpectroSamples;
	while(allSamples.size() - numberOfSamplesRecalculated >= 1024)
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

		lock.enter();
		allSpectroSamples.push_back(spectroLine);
		lock.exit();
				
		numberOfSamplesRecalculated += 512;
	}

	meanSpectro.resize(1024, 0);
	for (int i = 0; i < allSpectroSamples.size(); i++)
	{
		for (int j = 0; j < 1024; j++)
		{
			meanSpectro[j] += allSpectroSamples[i][j];
		}
	}
	for (int i = 0; i < 1024; i++)
		meanSpectro[i] /= allSpectroSamples.size();

	Globals::getInstance()->setSpectroNoise(meanSpectro);
}

Component* createNoiseMeasurement()
{
	return new NoiseMeasurement();
}