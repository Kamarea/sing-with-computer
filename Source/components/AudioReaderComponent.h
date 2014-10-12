
#ifndef _AUDIO_READER_TAB_COMPONENT_H_
#define _AUDIO_READER_TAB_COMPONENT_H_

#include "../component_headers.h"

class SpectrogramDisplay : public Component
{
public:
	SpectrogramDisplay(void);
	~SpectrogramDisplay(void);
    void paint (Graphics& g);
};

class PitchDisplay : public Component
{
public:
	PitchDisplay(void);
	~PitchDisplay(void);
    void paint (Graphics& g);

};

class AudioReaderComponentLive : public Component, 
//                             public AudioIODeviceCallback,
                             public Timer
{
public:
	AudioReaderComponentLive (AudioDeviceManager& deviceManager_);
	~AudioReaderComponentLive();
	
//    void paint (Graphics& g);
    void resized();
	
    void timerCallback();
	void loadClicked();
	AudioThumbnailCache thumbnailCache;

//    void audioDeviceAboutToStart (AudioIODevice* device);
//    void audioDeviceStopped();
//    void audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
//                                float** outputChannelData, int numOutputChannels, int numSamples);
	

private:
	FileChooser* fileChooser;
    float samples [2048];
	File wavFile;
	File spectroFile;
	File pitchFile;
	File directoryFile;
	
    TimeSliceThread thread;
	AudioSourcePlayer audioSourcePlayer;
    AudioTransportSource transportSource;
    ScopedPointer<AudioFormatReaderSource> currentAudioFileSource;

	int counter;
	SpectrogramDisplay* spectrogramDisplay;
	PitchDisplay* pitchDisplay;
	void loadFiles();
	
    AudioDeviceManager& deviceManagerPlayback;
};

class AudioReaderComponent : public Component
{
public:
	AudioReaderComponent(void);
	~AudioReaderComponent(void);
	
//    void paint (Graphics& g);
    void resized();
	TextButton* loadButton;
	AudioReaderComponentLive* audioReader;
	
private:
    AudioDeviceManager deviceManager;
	//AudioThumbnail thumbnail;
};

#endif //_AUDIO_READER_TAB_COMPONENT_H_