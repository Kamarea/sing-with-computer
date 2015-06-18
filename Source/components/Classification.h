#ifndef CLASSIFICATION_H
#define CLASSIFICATION_H

#include "../component_headers.h"

class Classification : public Component
{
public:
	Classification();
	~Classification();

private:
	FileChooser* fileChooser;
	File spectroFile;
	std::vector<std::vector<float>> spectros;
	std::vector<std::vector<float>> melSpectros;
	std::vector<std::vector<float>> melCepstros;
	std::vector<float> pitches;

	void calculateMelCepstrum();
	float calculateMel(float);
	float calculateReverseMel(float);
};

#endif // CLASSIFICATION_H