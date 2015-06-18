#ifndef CHOOSE_AND_LOAD_FILE_H
#define CHOOSE_AND_LOAD_FILE_H

#include "ScoreParts.h"
#include "../component_headers.h"

class ChooseAndLoadFile  : public Component
{
public:
	static ChooseAndLoadFile* getInstance()
	{
		if (instance == NULL)
		{
			instance = new ChooseAndLoadFile();
		}
		return instance;
	}
    ~ChooseAndLoadFile();

    void paint (Graphics& g);
    void resized();

    juce_UseDebuggingNewOperator

	Array<ScorePart> parts;
private:
	
	static ChooseAndLoadFile* instance;
	FileChooser* fileChooser;
	File openFile;
	String data;
	int actualMeasure;
	float actualMeasureCount;
	int mainOctave;
	Label* desc1;
	Label* fileChosen;
	Label* desc2;
	Label* measureNumber;
	Label* desc3;
	Label* title;
	Label* desc4;
	Label* composer;


	void readScore(int position);
	void parseLine(String line);
	void parseClef(String clef);
	void parseTime(String time);
	void parseKey(String key);
	void parseTempo(String tempo);
	void parseNote(String note);
    
    ChooseAndLoadFile ();
    ChooseAndLoadFile (const ChooseAndLoadFile&);
    const ChooseAndLoadFile& operator= (const ChooseAndLoadFile&);
};

#endif   // CHOOSE_AND_LOAD_FILE_H

