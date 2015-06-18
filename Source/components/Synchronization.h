#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include "../component_headers.h"

class SynchronizationAudioCallback;

class Synchronization : public Component,
						public Timer
{
public:
	Synchronization();
	~Synchronization();

	void timerCallback();
	void computeSynchronization();

    AudioDeviceManager deviceManager;
	SynchronizationAudioCallback* audioCallback;
	TextButton* button;
	bool toCompute;
	std::vector<float> played;
	std::vector<float> recorded;

private:
	Label* label;
	Label* result;
};

#endif // SYNCHRONIZATION_H