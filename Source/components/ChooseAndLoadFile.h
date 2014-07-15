/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  27 Dec 2013 8:47:51pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_CHOOSEANDLOADFILE_CHOOSEANDLOADFILE_D15E5DEA__
#define __JUCER_HEADER_CHOOSEANDLOADFILE_CHOOSEANDLOADFILE_D15E5DEA__

//[Headers]     -- You can add your own extra header files here --
#include "ScoreParts.h"
//[/Headers]


//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class ChooseAndLoadFile  : public Component
{
public:
    //==============================================================================
	static ChooseAndLoadFile* getInstance()
	{
		if (instance == NULL)
		{
			instance = new ChooseAndLoadFile();
		}
		return instance;
	}
    ~ChooseAndLoadFile();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();

    //==============================================================================
    juce_UseDebuggingNewOperator

	Array<ScorePart> parts;
private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	
	static ChooseAndLoadFile* instance;
	FileChooser* fileChooser;
	File openFile;
	String data;
	int actualMeasure;
	int mainOctave;

	void readScore(int position);
	void parseLine(String line, FILE* log);
	void parseClef(String clef, FILE* log);
	void parseTime(String time, FILE* log);
	void parseKey(String key, FILE* log);
	void parseTempo(String tempo, FILE* log);
	void parseNote(String note, FILE* log);
    //[/UserVariables]

    //==============================================================================


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    ChooseAndLoadFile ();
    ChooseAndLoadFile (const ChooseAndLoadFile&);
    const ChooseAndLoadFile& operator= (const ChooseAndLoadFile&);
};

#endif   // __JUCER_HEADER_CHOOSEANDLOADFILE_CHOOSEANDLOADFILE_D15E5DEA__

