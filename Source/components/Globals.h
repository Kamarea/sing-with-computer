#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "../component_headers.h"

const float log_10_2 = 0.30102999566398119521373889472449;

const float phi = 0.95;

const float pi = 3.14159265359;

const float spectrumResolution = 44100.0 / 1024.0;

enum lang {
	pl = 0,
	eng = 1
};

class Globals
{
public:
	static Globals* getInstance()
	{
		if (instance == NULL)
		{
			instance = new Globals();
		}
		return instance;
	};
	~Globals();

	String getUser();
	void setUser(String);
	void resaveFile();

	float getMeanNoise();
	std::vector<float> getSpectroNoise();
	void setMeanNoise(float);
	void setSpectroNoise(std::vector<float>);
	std::vector<String> getTexts();
	bool getShowScore();

	String title;
	String composer;

private:
	static Globals* instance;
	String currentUser;
	float meanNoise;
	std::vector<float> spectroNoise;
	enum lang currentLang;
	bool showScore;


	std::vector<std::vector<String>> languagePack;

	std::vector<std::pair<String, String>> settings;

	void parseLine(String line);
	void parseSpectroNoise(String);
	void loadLanguagePack();

	Globals();
    Globals (const Globals&);
    Globals& operator = (const Globals&);
};

#endif // GLOBALS_H