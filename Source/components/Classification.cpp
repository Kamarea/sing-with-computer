#include "Classification.h"

Classification::Classification()
{
	std::string folderName = "data\\";
	File directory = File(File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().
		getParentDirectory().getChildFile (folderName.c_str()).getChildFile(Globals::getInstance()->getUser()));
	fileChooser = new FileChooser(L"Choose a file...", directory, "spectroSamples.smp");
	if(!fileChooser->browseForFileToOpen())
		return;

	spectroFile = fileChooser->getResult();
	
	// spectros
	FileInputStream* stream = new FileInputStream(spectroFile);
	float sample;
	int count = 0;
	std::vector<float> spectroSample;
	spectroSample.clear();

	while(!stream->isExhausted())
	{
		sample = stream->readFloat();
		spectroSample.push_back(sample);
		count++;
		if(count == 1024)
		{
			spectros.push_back(spectroSample);
			spectroSample.clear();
			count = 0;
		}
	}
	deleteAndZero(stream);

	// pitches
	File pitchFile = spectroFile.getParentDirectory().getChildFile("pitchSamples.smp");
	stream = new FileInputStream(pitchFile);
	float pitch;
	while(!stream->isExhausted())
	{
		pitch = stream->readFloat();
		pitch = 440 * std::pow(2,(pitch-69)/12);;
		pitches.push_back(pitch);
	}
	deleteAndZero(stream);
	deleteAndZero(fileChooser);

	calculateMelCepstrum();
}

Classification::~Classification()
{
}

void Classification::calculateMelCepstrum()
{
	float filterMin = 50;
	float filterMax = 22050;

	filterMin = calculateMel(filterMin);
	filterMax = calculateMel(filterMax);
	int filterCount = 50;
	std::vector<float> filterPoints;
	float filterStep = (filterMax - filterMin) / (filterCount + 1);
	for (int i = 0; i < filterCount + 2; i++)
	{
		filterPoints.push_back(filterMin + filterStep * i);
	}
	for (int i = 0; i < filterPoints.size(); i++)
	{
		filterPoints[i] = calculateReverseMel(filterPoints[i]);
	}
	for (int i = 0; i < filterPoints.size(); i++)
	{
		filterPoints[i] = std::floor(filterPoints[i] / (44100 / 1024));
	}

	std::vector<std::vector<float>> filterBank;
	std::vector<float> filter;
	float length;
	for (int i = 0; i < filterCount; i++)
	{
		filter.clear();
		filter.resize(1024,0.0);
		length = filterPoints[i + 1] - filterPoints[i];
		for (int k = filterPoints[i]; k < filterPoints[i + 1]; k++)
		{
			filter[k] = (k - filterPoints[i]) / length;
		}
		length = filterPoints[i + 2] - filterPoints[i + 1];
		for (int k = filterPoints[i + 1]; k < filterPoints[i + 2]; k++)
		{
			filter[k] = (1 - (k - filterPoints[i + 1]) / length);
		}
		filterBank.push_back(filter);
	}

	// compute MFC
	melSpectros.clear();
	for (int i = 0; i < spectros.size(); i++)
	{
		std::vector<float> coefficients;
		coefficients.clear();
		// for all filters
		for (int j = 0; j < filterCount; j++)
		{
			float mfc = 0.0;
			for (int k = 0; k < 1024; k++)
			{
				mfc += spectros[i][k] * filterBank[j][k];
			}
			coefficients.push_back(mfc);
		}
		melSpectros.push_back(coefficients);
	}

	// compute mfcc
	melCepstros.clear();
	for (int i = 0; i < spectros.size(); i++)
	{
		std::vector<float> coefficients;
		for (int k = 0; k < filterCount; k++)
		{
			float mfcc = 0.0;
			for (int l = 0; l < filterCount; l++)
			{
				mfcc += log (melSpectros[i][l]) * std::cos ((k * (2 * l + 1) * pi / (2 * filterCount)));
			}
			// liftering
			int lambda = std::floor(pitches[i] / (44100 / 1024));
			if (k < lambda)
				mfcc = mfcc * (1 + (lambda / 2) * std::sin((pi * k) / lambda));
			coefficients.push_back(mfcc);
		}
		melCepstros.push_back(coefficients);
	}


	File cepstrum = spectroFile.getParentDirectory().getChildFile("cepstrum.smp");
	String data = "";
	for (int i = 0; i < melCepstros.size(); i++)
	{
		for (int j = 0; j < filterCount; j++)
		{
			data += String(melCepstros[i][j]);
			data += ", ";
		}
		data += String(pitches[i]);
		data += "\n";
	}
	FileOutputStream* stream = new FileOutputStream(cepstrum);
	stream->writeString(data);
	delete stream;
}

float Classification::calculateMel(float sample)
{
	return 1125 * std::log (1 + (sample / 700));
}

float Classification::calculateReverseMel(float sample)
{
	return 700 * (std::exp (sample / 1125) - 1);
}

Component* createClassification()
{
	return new Classification();
}