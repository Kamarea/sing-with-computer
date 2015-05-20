#include "AudioAmplitudePage.h"

AudioRecorder::AudioRecorder()
    : backgroundThread ("Audio Recorder Thread"),
        sampleRate (0), activeWriter (0)
{
    backgroundThread.startThread();
}

AudioRecorder::~AudioRecorder()
{
    stop();
}

//==============================================================================
void AudioRecorder::startRecording (const File& file)
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

void AudioRecorder::stop()
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

bool AudioRecorder::isRecording() const
{
    return activeWriter != 0;
}

//==============================================================================
void AudioRecorder::audioDeviceAboutToStart (AudioIODevice* device)
{
    sampleRate = device->getCurrentSampleRate();
}

void AudioRecorder::audioDeviceStopped()
{
    sampleRate = 0;
}

void AudioRecorder::audioDeviceIOCallback (const float** inputChannelData, int /*numInputChannels*/,
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


AudioAmplitudePage* AudioAmplitudePage::instance;

//[MiscUserDefs] You can add your own user definitions and misc code here...
AudioAmplitudePage::AudioAmplitudePage()
{
    accumulator = 0;
    setOpaque (true);
	
	numberOfSamplesRead = 0;
	numberOfSamplesRecalculated = 0;
	
	startTimer (1000 / 50); // use a timer to keep repainting this component
}

AudioAmplitudePage::~AudioAmplitudePage()
{
	// First, clear this pointer to stop the audio callback from using our writer object..
        
}

void AudioAmplitudePage::paint (Graphics& g)
{
    g.fillAll (Colours::white);

    g.setColour (Colours::black);
    const float midY = getHeight() * 0.5f;
    //int sampleNum = (nextSample + numElementsInArray (samples) - 1);

	lock.enter();
	// wczytaæ ile jest do narysowania - max getWidth();
	// dla tylu ile jest do  narysowania narysowaæ
	int drawNumber = amplitudes.size() > (getWidth()) ? 
		(getWidth()) : amplitudes.size();

    for (int x = 0; x < drawNumber; ++x)
    {
        int y=midY * amplitudes[amplitudes.size() - drawNumber + x];
		// getWidth() - 20 -drawNumber + x
        g.drawVerticalLine (getWidth() - drawNumber + x, midY - y, midY + y);
    }
	lock.exit();
	//g.drawHorizontalLine(midY, 0, getWidth());
}

void AudioAmplitudePage::timerCallback()
{
    repaint();
}

void AudioAmplitudePage::updateSamples(int number, std::vector<float>* samples)
{
	int rate = 100;
	if(numberOfSamplesRead < number)
	{
		// przepisac sample
		for (int i = numberOfSamplesRead; i < number; i++)
		{
			allSamples.push_back((*samples)[i]);
		}
		numberOfSamplesRead = number;

		int count = 0;
		float accumulator = 0;
		if (numberOfSamplesRead >= rate)
		{
			while (number - numberOfSamplesRecalculated >= rate)
			{
				if (count < rate)
				{
					accumulator += allSamples[numberOfSamplesRecalculated];
					count++;
				}
				else
				{
					lock.enter();
					amplitudes.push_back(accumulator / rate);
					lock.exit();
					accumulator = 0;
					count = 0;
				}

				numberOfSamplesRecalculated++;
			}
		}
	}
}

/*

void AudioAmplitudePage::playClicked(File folderName)
{
	file = folderName.getNonexistentChildFile("rawSamples", ".wav");
	
    recorder = new AudioRecorder();
    deviceManager.addAudioCallback (recorder);
	recorder->startRecording (file);
	isRecording = true;
}

void AudioAmplitudePage::stopClicked()
{
	isRecording = false;
    recorder->stop();
	//Time::waitForMillisecondCounter(500);
    deviceManager.removeAudioCallback (recorder);
	recorder = 0;
}
*/