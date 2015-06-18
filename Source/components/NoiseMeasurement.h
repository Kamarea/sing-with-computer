#ifndef NOISE_MEASUREMENT_H
#define NOISE_MEASUREMENT_H

#include "../component_headers.h"
#include "SoundInput.h"

class NoiseMeasurement : public Component
{
public:
	NoiseMeasurement();
	~NoiseMeasurement();
	
    void resized();
	void getBeginTime();
	void calculateNoise();
	void addCallback();
	void removeCallback();


private:
	SoundInput* soundInput;
	std::vector<float> allSamples;
	CriticalSection lock;
	int beginTime;
    AudioDeviceManager deviceManager;

	TextButton* startButton;
	TextButton* stopButton;

	float meanAmplitude;
	std::vector<float> meanSpectro;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    NoiseMeasurement (const NoiseMeasurement&);
    const NoiseMeasurement& operator= (const NoiseMeasurement&);
};

#endif // NOISE_MEASUREMENT_H