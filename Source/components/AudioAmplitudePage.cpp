#include "AudioAmplitudePage.h"

class AudioRecorder  : public AudioIODeviceCallback
{
public:
    AudioRecorder()
        : backgroundThread ("Audio Recorder Thread"),
          sampleRate (0), activeWriter (0)
    {
        backgroundThread.startThread();
    }

    ~AudioRecorder()
    {
        stop();
    }

    //==============================================================================
    void startRecording (const File& file)
    {
        stop();

        if (sampleRate > 0)
        {
            // Create an OutputStream to write to our destination file...
            file.deleteFile();
            ScopedPointer<FileOutputStream> fileStream (file.createOutputStream());

            if (fileStream != 0)
            {
                // Now create a WAV writer object that writes to our output stream...
                WavAudioFormat wavFormat;
                AudioFormatWriter* writer = wavFormat.createWriterFor (fileStream, sampleRate, 1, 16, StringPairArray(), 0);

                if (writer != 0)
                {
                    fileStream.release(); // (passes responsibility for deleting the stream to the writer object that is now using it)

                    // Now we'll create one of these helper objects which will act as a FIFO buffer, and will
                    // write the data to disk on our background thread.
                    threadedWriter = new AudioFormatWriter::ThreadedWriter (writer, backgroundThread, 32768);

                    // And now, swap over our active writer pointer so that the audio callback will start using it..
                    const ScopedLock sl (writerLock);
                    activeWriter = threadedWriter;
                }
            }
        }
    }

    void stop()
    {
        // First, clear this pointer to stop the audio callback from using our writer object..
        {
            const ScopedLock sl (writerLock);
            activeWriter = 0;
        }

        // Now we can delete the writer object. It's done in this order because the deletion could
        // take a little time while remaining data gets flushed to disk, so it's best to avoid blocking
        // the audio callback while this happens.
        threadedWriter = 0;
    }

    bool isRecording() const
    {
        return activeWriter != 0;
    }

    //==============================================================================
    void audioDeviceAboutToStart (AudioIODevice* device)
    {
        sampleRate = device->getCurrentSampleRate();
    }

    void audioDeviceStopped()
    {
        sampleRate = 0;
    }

    void audioDeviceIOCallback (const float** inputChannelData, int /*numInputChannels*/,
                                float** outputChannelData, int numOutputChannels,
                                int numSamples)
    {
        const ScopedLock sl (writerLock);

        if (activeWriter != 0)
            activeWriter->write (inputChannelData, numSamples);

        // We need to clear the output buffers, in case they're full of junk..
        for (int i = 0; i < numOutputChannels; ++i)
            if (outputChannelData[i] != 0)
                zeromem (outputChannelData[i], sizeof (float) * numSamples);
    }

private:
    TimeSliceThread backgroundThread; // the thread that will write our audio data to disk
    ScopedPointer<AudioFormatWriter::ThreadedWriter> threadedWriter; // the FIFO used to buffer the incoming data
    double sampleRate;

    CriticalSection writerLock;
    AudioFormatWriter::ThreadedWriter* volatile activeWriter;
};


struct FileHeader
{
	char chunkID[4];
	int chunkSize;
	char format[4];

	char subchunk1ID[4];
	int subchunk1Size;
	short int audioFormat;
	short int numChannels;
	int sampleRate;
	int byteRate;
	short int blockAlign;
	short int bitsPerSample;

	char subchunk2ID[4];
	int subchunk2Size;
};

LiveAudioAmplitudeDisplayComp* LiveAudioAmplitudeDisplayComp::instance;

//[MiscUserDefs] You can add your own user definitions and misc code here...
LiveAudioAmplitudeDisplayComp::LiveAudioAmplitudeDisplayComp()
{
    nextSample = subSample = 0;
    accumulator = 0;
    zeromem (samples, sizeof (samples));
    setOpaque (true);
	
	startTimer (1000 / 50); // use a timer to keep repainting this component
}

LiveAudioAmplitudeDisplayComp::~LiveAudioAmplitudeDisplayComp()
{
	// First, clear this pointer to stop the audio callback from using our writer object..
        
}

void LiveAudioAmplitudeDisplayComp::paint (Graphics& g)
{
    g.fillAll (Colours::white);

    g.setColour (Colours::black);
    const float midY = getHeight() * 0.5f;
    int sampleNum = (nextSample + numElementsInArray (samples) - 1);

    for (int x = jmin (getWidth(), (int) numElementsInArray (samples)); --x >= 0;)
    {
        const float sampleSize = midY * samples [sampleNum-- % numElementsInArray (samples)];
        g.drawVerticalLine (x, midY - sampleSize, midY + sampleSize);
    }
}

void LiveAudioAmplitudeDisplayComp::timerCallback()
{
    repaint();
}

void LiveAudioAmplitudeDisplayComp::audioDeviceAboutToStart (AudioIODevice*)
{
    zeromem (samples, sizeof (samples));
}

void LiveAudioAmplitudeDisplayComp::audioDeviceStopped()
{
    zeromem (samples, sizeof (samples));
}

void convertAudio(const float** inputData, int numChanIn, int** outputData, int numSamples)
{
	for (int i=0;i<numSamples;++i)
	{
		for (int chan=0;chan<numChanIn;++chan)
		{
			//if (inputData[chan][i] >= 1.0)
				outputData[chan][i] = 32767;
			//else if (inputData[chan][i] <= -1.0)
			//	outputData[chan][i] = -32767;
			//else
			//	outputData[chan][i] = (int)(inputData[chan][i] * 32767.0);
		}
	}
}

void LiveAudioAmplitudeDisplayComp::audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                                                       float** outputChannelData, int numOutputChannels, int numSamples)
{
	/*int **inputInt;
	inputInt = new int*[numInputChannels];
	for (int i=0; i<numInputChannels; ++i)
		inputInt[i] = new int[numSamples];

	convertAudio(inputChannelData, numInputChannels,inputInt,numSamples);
	audioFormatWriter->write(inputChannelData,numSamples);*/

	short int tmp;
	char charArray[2];
    for (int i = 0; i < numSamples; ++i)
    {
        for (int chan = 0; chan < numInputChannels; ++chan)
        {
            if (inputChannelData[chan] != 0)
			{
                accumulator += fabsf (inputChannelData[chan][i]);
				/*tmp = inputChannelData[chan][i] * 32768;
				charArray[0] = tmp >> 8;
				charArray[1] = tmp;
				fileOutput << charArray[0];
				fileOutput << charArray[1];*/
			}
        }

        const int numSubSamples = 600; // how many input samples go onto one pixel.
        const float boost = 1.0f;     // how much to boost the levels to make it more visible.

        if (subSample == 0)
        {
            samples[nextSample] = accumulator * boost / numSubSamples;
            nextSample = (nextSample + 1) % numElementsInArray (samples);
            subSample = numSubSamples;
            accumulator = 0;
        }
        else
        {
            --subSample;
        }
    }

    // We need to clear the output buffers, in case they're full of junk..
    for (int i = 0; i < numOutputChannels; ++i)
        if (outputChannelData[i] != 0)
            zeromem (outputChannelData[i], sizeof (float) * numSamples);
}



AudioAmplitudePage::AudioAmplitudePage (AudioDeviceManager& deviceManager_, File folderName)
    : deviceManager (deviceManager_),
      liveAudioAmplitudeDisplayComp (0)
{
	addAndMakeVisible (liveAudioAmplitudeDisplayComp = LiveAudioAmplitudeDisplayComp::getInstance());

    //addAndMakeVisible (explanationLabel = new Label (String::empty,
    //                                                 L"This page demonstrates how to record a wave file from the live audio input..\n\nPressing record will start recording a file in your \"Documents\" folder."));
    //addAndMakeVisible (explanationLabel = new Label (String::empty,
     //                                                L""));
    //explanationLabel->setFont (Font (15.0000f, Font::plain));
    //explanationLabel->setJustificationType (Justification::topLeft);
    //explanationLabel->setEditable (false, false, false);
    //explanationLabel->setColour (TextEditor::textColourId, Colours::black);
    //explanationLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    //addAndMakeVisible (recordButton = new TextButton (String::empty));
    //recordButton->setButtonText (L"Record");
    //recordButton->addListener (this);
    //recordButton->setColour (TextButton::buttonColourId, Colour (0xffff5c5c));
    //recordButton->setColour (TextButton::textColourOnId, Colours::black);
	//[UserPreSize]
    //[/UserPreSize]

    //setSize (600, 500);


    //[Constructor] You can add your own custom stuff here..
    //recorder = new AudioRecorder();
    //deviceManager.addAudioCallback (recorder);
    deviceManager.addAudioCallback (liveAudioAmplitudeDisplayComp);

	//std::string fileName = "rawSamples.wav";
	//File file (File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getParentDirectory().getParentDirectory().getChildFile(folderName.c_str()).getNonexistentChildFile ("rawSamples", ".wav"));
		//getChildFile(fileName.c_str());
	//File file = File(File::getSpecialLocation (File::userDocumentsDirectory)
    //                   .getNonexistentChildFile ("rawSamples", ".wav"));
	file = folderName.getNonexistentChildFile("rawSamples", ".wav");
	
    recorder = new AudioRecorder();
    deviceManager.addAudioCallback (recorder);
	startTimer(200);
    //[/Constructor]
}

AudioAmplitudePage::~AudioAmplitudePage()
{
	
    recorder->stop();
	Time::waitForMillisecondCounter(500);
    //[Destructor_pre]. You can add your own custom destruction code here..
    //deviceManager.removeAudioCallback (recorder);
    deviceManager.removeAudioCallback (recorder);
    deviceManager.removeAudioCallback (liveAudioAmplitudeDisplayComp);
	recorder = 0;
    ////recorder = 0;
    //[/Destructor_pre]

    deleteAndZero (liveAudioAmplitudeDisplayComp);
    //deleteAndZero (explanationLabel);
    //deleteAndZero (recordButton);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AudioAmplitudePage::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::lightgrey);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AudioAmplitudePage::resized()
{
    liveAudioAmplitudeDisplayComp->setBounds (5, 5, getWidth() - 10, getHeight()-10);
    //explanationLabel->setBounds (160, 150, getWidth() - 169, 216);
    //recordButton->setBounds (8, 88, 136, 40);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AudioAmplitudePage::timerCallback()
{	
	recorder->startRecording (file);
	stopTimer();
}
