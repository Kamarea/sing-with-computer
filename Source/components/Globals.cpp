#include "Globals.h"

const String settingsFileName = "data\\settings.txt";

Globals* Globals::instance;

Globals::Globals()
{
	File settingsFile = File(File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getParentDirectory().getChildFile (settingsFileName));
	String data = settingsFile.loadFileAsString();
	
	int begOfLine = 0;
	int endOfLine = data.indexOf(begOfLine+1,"\n");
	String line = data.substring(begOfLine,endOfLine-1).trimStart();

	while(!line.isEmpty())
	{
		parseLine(line);
		begOfLine = endOfLine;
		endOfLine = data.indexOf(begOfLine+1,"\n");
		line = data.substring(begOfLine+1,endOfLine-1).trimStart();
	}
}

void Globals::resaveFile()
{
	File settingsFile = File(File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getParentDirectory().getChildFile (settingsFileName));
	String wholeSettings = L"";
	for (int i = 0; i < settings.size(); i++)
	{
		wholeSettings.append(settings[i].first,settings[i].first.length());
		wholeSettings.append(" : ", 3);
		wholeSettings.append(settings[i].second,settings[i].second.length());
		wholeSettings.append("\n", 1);
	}
	settingsFile.replaceWithText(wholeSettings);
}

void Globals::parseLine(String line)
{
	int nameEnd = line.indexOf(0, " : ");
	std::pair<String, String> pair;
	pair.first = line.substring(0,nameEnd);
	int valueEnd = line.length();
	pair.second = line.substring(nameEnd + 3, valueEnd);
	settings.push_back(pair);
	if (pair.first == "user")
	{
		currentUser = pair.second;
	}
}

String Globals::getUser()
{
	return currentUser;
}

void Globals::setUser(String user_)
{
	currentUser = user_;
	for (int i = 0; i < settings.size(); i++)
	{
		if (settings[i].first == "user")
			settings[i].second = currentUser;
	}
}

float Globals::getMeanNoise()
{
	return meanNoise;
}

std::vector<float> Globals:: getSpectroNoise()
{
	return spectroNoise;
}

void Globals::setMeanNoise(float noise)
{
	meanNoise = noise;
}

void Globals::setSpectroNoise(std::vector<float> noise)
{
	for (int i = 0; i < 1024; i++)
		spectroNoise[i] = noise[i];
}
