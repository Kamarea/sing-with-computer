#include "Synchronization.h"

class SynchronizationAudioCallback :	public Component,
										public AudioIODeviceCallback
{
public:
	SynchronizationAudioCallback(Synchronization* component)
	{
		
		m_component = component;
		actualAmpli = 0;
		sampleRate = 44100;
	}

	~SynchronizationAudioCallback()
	{
	}

	void init()
	{
		sampleNumber = 0;
		count = 6;
		counter = random.nextInt(10);
	}

	void audioDeviceAboutToStart (AudioIODevice* device)
	{
		played.clear();
		recorded.clear();
	}
    void audioDeviceStopped()
	{
	}

    void audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                                float** outputChannelData, int numOutputChannels, int numSamples)
	{
		counter--;
		if (counter <= 0)
		{
			if (actualAmpli == 1)
				actualAmpli = 0;
			else
				actualAmpli = 1;
			count--;
			if (count <= 0)
			{
				m_component->deviceManager.removeAudioCallback(this);
				m_component->played = played;
				m_component->recorded = recorded;
				m_component->toCompute = true;
			}
			counter = random.nextInt(10);
		}

		float value;
		for (int i = 0; i < numSamples; i++)
		{
			value = actualAmpli * std::sin(1.58 * 440 * ((float)((sampleNumber + i)) / sampleRate));
			if (value < 0)
				value *= (-1);
			for (int chan = 0; chan < numOutputChannels; chan++)
			{
				outputChannelData[chan][i] = value;
			}
			played.push_back(value);
		}
		for (int i = 0; i < numSamples; i++)
		{
			float recordedValue = 0;
			for (int chan = 0; chan < numInputChannels; chan++)
			{
				recordedValue += inputChannelData[chan][i];
			}
			recorded.push_back(recordedValue);
		}
		
		sampleNumber+=numSamples;
	}
	
private:
	int actualAmpli;
	int sampleRate;
	int count;
	int counter;
	int sampleNumber;
	std::vector<float> played;
	std::vector<float> recorded;
	Synchronization* m_component;
	Random random;
};

class SynchroListener : public ButtonListener
{
public: 
	SynchroListener(Synchronization* comp)
	{
		m_comp = comp;
	}

	void buttonClicked(Button* button)
	{
		m_comp->audioCallback->init();
		m_comp->deviceManager.addAudioCallback (m_comp->audioCallback);
		m_comp->button->setEnabled(false);
	}

	Synchronization* m_comp;
};

Synchronization::Synchronization()
{
	setComponentID("synchronization");
	
    setSize (1000, 500);

	label = new Label(String::empty, Globals::getInstance()->getTexts()[26]);
	label->setFont(Font(30));
	label->setBounds(0,0,getWidth(),100);
	label->setJustificationType(Justification(36));
	addAndMakeVisible(label);

	button = new TextButton();
	button->setButtonText(Globals::getInstance()->getTexts()[27]);
	button->setBounds(getWidth() / 2 - 70,100,140,50);
	SynchroListener* listener = new SynchroListener(this);
	button->addListener(listener);
	addAndMakeVisible(button);

	result = new Label();
	result->setFont(Font(20));
	result->setBounds(50, 200, 500, 50);
	addAndMakeVisible(result);
	
    deviceManager.initialise (2, 2, 0, true, String::empty, 0);
	toCompute = false;
	
	audioCallback = new SynchronizationAudioCallback(this);

	startTimer(100);
}

Synchronization::~Synchronization()
{
	deleteAndZero(label);
	deleteAndZero(button);
	deleteAndZero(audioCallback);
	deleteAndZero(result);
}

void Synchronization::timerCallback()
{
	if (toCompute)
	{
		computeSynchronization();
	}
}

void Synchronization::computeSynchronization()
{	
	toCompute = false;
	result->setText(Globals::getInstance()->getTexts()[30],sendNotification);
	repaint();

	std::vector<float> sums;

	std::vector<float> played_re;
	std::vector<float> recorded_re;

	// zmniejszenie objêtoœci wektorów
	for (int i = 0; i < played.size() - 10; i += 10)
	{
		float value = 0;
		for (int j = 0; j < 10; j ++)
			value += std::abs(played[i + j]);
		played_re.push_back(value);
		value = 0;
		for (int j = 0; j < 10; j ++)
			value += std::abs(recorded[i + j]);
		recorded_re.push_back(value);
	}
	// obliczenie korelacji
	for (int i = 0; i < played_re.size(); i++)
	{
		int size = played_re.size() - i;
		float sum = 0;
		for (int j = 0; j < size; j++)
		{
			sum += played_re[i + j] * recorded_re[j] * 10000;
		}
		sums.push_back(sum);
	}

	// maksymalna korelacja
	int max = 0;
	int shift = -1;
	for (int i = 0; i < sums.size(); i++)
	{
		if (sums[i] > max)
		{
			max = sums[i];
			shift = i;
		}
	}
	shift *= 10;

	if (shift < 2560)
	{
		button->setEnabled(true);
		result->setText(Globals::getInstance()->getTexts()[31],sendNotification);
	}
	else
	{
		String tmp = Globals::getInstance()->getTexts()[28];
		tmp += shift;
		tmp += Globals::getInstance()->getTexts()[29], Globals::getInstance()->getTexts()[29].length();
		result->setText(tmp,sendNotification);
		Globals::getInstance()->setShift(shift);
	}
	repaint();
}

Component* createSynchronization()
{
	return new Synchronization();
}