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

	spectroNoise.resize(1024);
	loadLanguagePack();
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
	
	if (pair.first == "lang")
	{
		if (pair.second == "pl")
			currentLang = pl;
		if (pair.second == "eng")
			currentLang = eng;
	}
	
	if (pair.first == "meanNoise")
	{
		meanNoise = pair.second.getFloatValue();
	}

	if (pair.first == "spectroNoise")
	{
		parseSpectroNoise(pair.second);
	}

	if (pair.first == "showScore")
	{
		showScore = pair.second.getIntValue();
	}

	if (pair.first == "shift")
	{
		shift = pair.second.getIntValue();
	}

	if (pair.first == "pitchTreshold")
	{
		pitchTreshold = pair.second.getFloatValue();
	}
}

void Globals::parseSpectroNoise(String line)
{
	spectroNoise.resize(1024);
	int begin = 0;
	int end = line.indexOf(begin+1,",");
	for (int i = 0; i < 1024; i++)
	{
		spectroNoise[i] = line.substring(begin,end).getFloatValue();
		begin = end+1;
		end = line.indexOf(begin+1,",");
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
	for (int i = 0; i < settings.size(); i++)
	{
		if (settings[i].first == "meanNoise")
			settings[i].second = String(meanNoise);
	}
}

void Globals::setSpectroNoise(std::vector<float> noise)
{
	for (int i = 0; i < 1024; i++)
		spectroNoise[i] = noise[i];
	String line = String();
	for (int i = 0; i < 1024; i++)
	{
		line.append(String(spectroNoise[i]),100);
		line.append(",",1);
	}
	line.append("\n",1);
	
	for (int i = 0; i < settings.size(); i++)
	{
		if (settings[i].first == "spectroNoise")
			settings[i].second = line;
	}
}

std::vector<String> Globals::getTexts()
{
	return languagePack[currentLang];
}

bool Globals::getShowScore()
{
	return showScore;
}

void Globals::setShift(int shift_in)
{
	shift = shift_in;
	for (int i = 0; i < settings.size(); i++)
	{
		if (settings[i].first == "shift")
			settings[i].second = String(shift);
	}
}

int Globals::getShift()
{
	return shift;
}

float Globals::getPitchTreshold()
{
	return pitchTreshold;
}

void Globals::loadLanguagePack()
{
	std::vector<String> plTexts;
	languagePack.push_back( plTexts);
	
	std::vector<String> engTexts;
	languagePack.push_back( engTexts);

	languagePack[0].push_back(L"Magisterka!");// 0
	languagePack[1].push_back(L"Sing with computer");
	languagePack[0].push_back(L"Wybierz u¿ytkownika");// 1
	languagePack[1].push_back(L"Choose a user");
	languagePack[0].push_back(L"Wybierz u¿ytkownika:");// 2
	languagePack[1].push_back(L"Choose a user");
	languagePack[0].push_back(L"Wybierz");// 3
	languagePack[1].push_back(L"Choose");
	languagePack[0].push_back(L"lub dodaj nowego:");// 4
	languagePack[1].push_back(L"or add a new one:");
	languagePack[0].push_back(L"Dodaj");// 5
	languagePack[1].push_back(L"Add");
	languagePack[0].push_back(L"U¿ytkownik ju¿ istnieje!");//6
	languagePack[1].push_back(L"User already exists!");
	languagePack[0].push_back(L"Glowne");//7
	languagePack[1].push_back(L"Main");
	languagePack[0].push_back(L"Dodatki");//8
	languagePack[1].push_back(L"Addons");
	languagePack[0].push_back(L"Za³aduj plik");//9
	languagePack[1].push_back(L"Load file");
	languagePack[0].push_back(L"Œpiewaj");//10
	languagePack[1].push_back(L"Sing");
	languagePack[0].push_back(L"Koniec");//11
	languagePack[1].push_back(L"Quit");
	languagePack[0].push_back(L"Za³aduj nagranie");//12
	languagePack[1].push_back(L"Load audio file");
	languagePack[0].push_back(L"Pomiar szumu");//13
	languagePack[1].push_back(L"Noise measurement");
	languagePack[0].push_back(L"Wybrano plik");//14
	languagePack[1].push_back(L"File chosen");
	languagePack[0].push_back(L"Tytu³");//15
	languagePack[1].push_back(L"Title");
	languagePack[0].push_back(L"Kompozytor");//16
	languagePack[1].push_back(L"Composer");
	languagePack[0].push_back(L"Liczba taktów");//17
	languagePack[1].push_back(L"Measure number");
	languagePack[0].push_back(L"Oscylogram");//18
	languagePack[1].push_back(L"Oscilogram");
	languagePack[0].push_back(L"Wysokoœæ");//19
	languagePack[1].push_back(L"Pitch");
	languagePack[0].push_back(L"Spektrogram");//20
	languagePack[1].push_back(L"Spectrogram");
	languagePack[0].push_back(L"Przeæwiczyæ?");//21
	languagePack[1].push_back(L"Practise?");
	languagePack[0].push_back(L"Czy chcesz przeæwiczyæ takty ");//22
	languagePack[1].push_back(L"Do you want to practise measures ");
	languagePack[0].push_back(L"Tak");//23
	languagePack[1].push_back(L"Yes");
	languagePack[0].push_back(L"Nie");//24
	languagePack[1].push_back(L"No");
	languagePack[0].push_back(L"Synchronizacja");//25
	languagePack[1].push_back(L"Synchronization");
	languagePack[0].push_back(L"Kliknij aby sprzeprowadziæ synchronizacjê miêdzy mikrofonem a g³oœnikami");//26
	languagePack[1].push_back(L"Click to calculate synchronization between microphone and speakers");
	languagePack[0].push_back(L"Synchronizuj");//27
	languagePack[1].push_back(L"Synchronize");
	languagePack[0].push_back(L"Przesuniêcie miêdzy wejœciem a wyjœciem wynosi ");//28
	languagePack[1].push_back(L"The shift between input and output is ");
	languagePack[0].push_back(L" próbek.");//29
	languagePack[1].push_back(L" samples.");
	languagePack[0].push_back(L"Obliczanie...");//30
	languagePack[1].push_back(L"Computing...");
	languagePack[0].push_back(L" Nie uda³o siê poprawnie obliczyæ przesuniêcia. Spróbuj ponownie.");//31
	languagePack[1].push_back(L" Synchronization failed. Try again.");
	languagePack[0].push_back(L"Klasyfikacja");//32
	languagePack[1].push_back(L"Classification");
	languagePack[0].push_back(L"Intonacja");//33
	languagePack[1].push_back(L"Pitch");
	languagePack[0].push_back(L"Rytm");//34
	languagePack[1].push_back(L"Rhythm");
	languagePack[0].push_back(L"Barwa");//35
	languagePack[1].push_back(L"Colour");
	languagePack[0].push_back(L"dobra");//36
	languagePack[1].push_back(L"good");
	languagePack[0].push_back(L"z³a");//37
	languagePack[1].push_back(L"bad");
	languagePack[0].push_back(L"b. z³a");//38
	languagePack[1].push_back(L"v. bad");
}