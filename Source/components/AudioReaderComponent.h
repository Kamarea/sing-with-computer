
#ifndef _AUDIO_READER_TAB_COMPONENT_H_
#define _AUDIO_READER_TAB_COMPONENT_H_

#include "../component_headers.h"
//#include "../../JuceLibraryCode/modules/juce_gui_basics/filebrowser/juce_DirectoryContentsList.h"

class DemoThumbnailComp;

class SpectrogramDisplay : public Component
{
public:
	SpectrogramDisplay(File inFile, int samplerate);
	~SpectrogramDisplay(void);
	void updatePosition(int positionChange);
    void paint (Graphics& g);

private:
	Image image;
	std::vector<std::vector<float>> spectros;
	int actualPosition;
	int localActualPosition;
	int sampleRate;
	CriticalSection lock;
};

class PitchDisplay : public Component
{
public:
	PitchDisplay(File inFile, int samplerate);
	~PitchDisplay(void);
	void updatePosition(int positionChange);
    void paint (Graphics& g);

private:
	std::vector<float> pitches;
	int actualPosition;
	int temp;
	int sampleRate;
	CriticalSection lock;
};

class AudioReaderComponentLive : public Component, 
                               public FileBrowserListener,
//                             public AudioIODeviceCallback,
                             public Timer
{
public:
	AudioReaderComponentLive (AudioDeviceManager& deviceManager_);
	~AudioReaderComponentLive();

	void fileClicked (const File& file, const MouseEvent& e);
	void showFile (const File& file);
//    void paint (Graphics& g);
    void resized();
	
    void timerCallback();
    void selectionChanged();
    void fileDoubleClicked (const File& file);
    void browserRootChanged (const File&) {}

//    void audioDeviceAboutToStart (AudioIODevice* device);
//    void audioDeviceStopped();
//    void audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
//                                float** outputChannelData, int numOutputChannels, int numSamples);
	

private:
	//FileChooser* fileChooser;
	bool reading;
    float samples [2048];
	File wavFile;
	File spectroFile;
	File pitchFile;
	File directoryFile;
	
    AudioFormatManager formatManager;
	//list of files in current directory
    DirectoryContentsList directoryList;

    TimeSliceThread thread;
	// audio playing files
	AudioSourcePlayer audioSourcePlayer;
    AudioTransportSource transportSource;
    ScopedPointer<AudioFormatReaderSource> currentAudioFileSource;

	// displaying pitch and spectro
	int counter;
	SpectrogramDisplay* spectrogramDisplay;
	PitchDisplay* pitchDisplay;
	void loadFiles();
	
	// searching for a file and loading it
    void loadFileIntoTransport (const File& audioFile);
    FileTreeComponent* fileTreeComp;
	// thumbnail of a file amplitude
    //DemoThumbnailComp* thumbnail;
	// filter to see only .wav files
	WildcardFileFilter* filter;
	
    AudioDeviceManager& deviceManagerPlayback;
};

class AudioReaderComponent : public Component
{
public:
	AudioReaderComponent(void);
	~AudioReaderComponent(void);
	
//    void paint (Graphics& g);
    void resized();
	AudioReaderComponentLive* audioReader;
	
private:
    AudioDeviceManager deviceManager;
};

#endif //_AUDIO_READER_TAB_COMPONENT_H_