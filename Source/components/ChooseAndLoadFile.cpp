/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  27 Dec 2013 8:56:44pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
#include "ChooseAndLoadFile.h"
//[/Headers]


//[MiscUserDefs] You can add your own user definitions and misc code here...

//[/MiscUserDefs]
ChooseAndLoadFile* ChooseAndLoadFile::instance;
//==============================================================================
ChooseAndLoadFile::ChooseAndLoadFile ()
{

    //[UserPreSize]
    //[/UserPreSize]
	setComponentID("chooseAndLoadFileID");
    setSize (1, 1);
    setOpaque (true);
	
	desc1 = new Label();
	fileChosen = new Label();
	desc2 = new Label();
	title = new Label();
	desc3 = new Label();
	composer = new Label();
	desc4 = new Label();
	measureNumber = new Label();
	
	addAndMakeVisible(desc1);
	addAndMakeVisible(fileChosen);
	addAndMakeVisible(desc2);
	addAndMakeVisible(title);
	addAndMakeVisible(desc3);
	addAndMakeVisible(composer);
	addAndMakeVisible(desc4);
	addAndMakeVisible(measureNumber);
	
	fileChooser = new FileChooser("Choose file...",File::nonexistent,"*.ly");
	if(!fileChooser->browseForFileToOpen())
		return;

	openFile = fileChooser->getResult();
	data = openFile.loadFileAsString();
	int position=0;
	readScore(position);
	
	desc1->setText(Globals::getInstance()->getTexts()[14], true);
	desc2->setText(Globals::getInstance()->getTexts()[15], true);
	desc3->setText(Globals::getInstance()->getTexts()[16], true);
	desc4->setText(Globals::getInstance()->getTexts()[17], true);
	fileChosen->setText(openFile.getFileName(), true);
	title->setText(Globals::getInstance()->title, true);
	composer->setText(Globals::getInstance()->composer, true);
	measureNumber->setText(String(actualMeasure), true);

	desc1->setFont(Font(50));
	desc1->setBounds(20,30,500,50);
	desc2->setFont(Font(50));
	desc2->setBounds(20,100,500,50);
	desc3->setFont(Font(50));
	desc3->setBounds(20,170,500,50);
	desc4->setFont(Font(50));
	desc4->setBounds(20,240,500,50);
	fileChosen->setFont(Font(50));
	fileChosen->setBounds(550, 30, 300, 50);
	title->setFont(Font(50));
	title->setBounds(550, 100, 300, 50);
	composer->setFont(Font(50));
	composer->setBounds(550, 170, 300, 50);
	measureNumber->setFont(Font(50));
	measureNumber->setBounds(550, 240, 300, 50);
	repaint();

    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

ChooseAndLoadFile::~ChooseAndLoadFile()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

	deleteAndZero(desc1);
	deleteAndZero(desc2);
	deleteAndZero(desc3);
	deleteAndZero(desc4);
	deleteAndZero(fileChosen);
	deleteAndZero(title);
	deleteAndZero(composer);
	deleteAndZero(measureNumber);
	deleteAndZero(fileChooser);

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================

void ChooseAndLoadFile::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::white);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ChooseAndLoadFile::resized()
{
    //[UserResized] Add your own custom resize handling here..
	
/*	desc1->setFont(Font(50));
	desc1->setBounds(0,30,200,50);
	desc2->setFont(Font(50));
	desc2->setBounds(0,100,200,50);
	desc3->setFont(Font(50));
	desc3->setBounds(0,170,200,50);
	desc4->setFont(Font(50));
	desc4->setBounds(0,240,200,50);
	fileChosen->setFont(Font(50));
	fileChosen->setBounds(250, 30, 300, 50);*/
    //[/UserResized]
}

void ChooseAndLoadFile::readScore(int position)
{
	//title -> " -> " -> to co poœrodku
	//composer -> " -> " -> to co poœrodku
	int beg = data.indexOf(position, "title");
	beg = data.indexOf(beg, "\"");
	int end = data.indexOf(beg + 1, "\"");
	String title = data.substring(beg + 1, end);

	beg = data.indexOf(end, "composer");
	beg = data.indexOf(beg, "\"");
	end = data.indexOf(beg + 1, "\"");
	String composer = data.substring(beg + 1, end);
	Globals::getInstance()->title = title;
	Globals::getInstance()->composer = composer;

	int score = data.indexOf(end, "\\score");
	if(score>data.length())
		return;

	score = data.indexOf(score, "\\relative");
	mainOctave = 0;
	String relativeCount = data.substring(score,data.indexOf(score+1,"{"));
	relativeCount = relativeCount.substring(9);
	relativeCount.trim();

	for (int i=0;i<relativeCount.length();i++)
	{
		if(relativeCount.substring(i,i+1) == "'")
		{
			mainOctave++;
			relativeCount = relativeCount.substring(0,relativeCount.length());
		}
		if (relativeCount.substring(i,i+1) == ",")
		{
			mainOctave--;
			relativeCount = relativeCount.substring(0,relativeCount.length());
		}
	}

	Note note;
	note.duration = 0;
	note.pitch = c;
	note.octave = mainOctave;
	
	ScorePart part;
	part.measureNumber = 0;
	part.clef = noClef;
	part.key.pitch = noPitch;
	part.key.form = noForm;
	part.time.count = 0;
	part.time.base = 0;
	part.tempo.base=0;
	part.tempo.value=0;
	part.notes.add(note);
	parts.add(part);
	actualMeasure = 0;
	actualMeasureCount = 0.0;
	
	int begOfLine = data.indexOf(score+1,"\n");
	int endOfLine = data.indexOf(begOfLine+1,"\n");
	String line = data.substring(begOfLine+1,endOfLine-1).trimStart();
	while(!line.isEmpty())
	{
		parseLine(line);
		begOfLine = endOfLine;
		endOfLine = data.indexOf(begOfLine+1,"\n");
		line = data.substring(begOfLine+1,endOfLine-1).trimStart();
	}
	
}

void ChooseAndLoadFile::parseLine(String line)
{
	// clef change notification
	if(line.substring(0,6)=="\\clef ")
	{
		if(parts.getLast().clef!=noClef)
		{
			//create new part
			ScorePart newPart = ScorePart();
			newPart.measureNumber = actualMeasure;
			parts.add(newPart);
		}
		String clefSign = line.substring(6);
		parseClef(clefSign);
	}
	// timing change notification
	else if(line.substring(0,6)=="\\time ")
	{
		if(parts.getLast().time.count !=0 &&
			parts.getLast().time.base != 0)
		{
			//create new part
			ScorePart newPart = ScorePart();
			newPart.measureNumber = actualMeasure;
			parts.add(newPart);
		}
		String timeSign = line.substring(6);
		parseTime(timeSign);
	}
	// key change notification
	else if(line.substring(0,5)=="\\key ")
	{
		if(parts.getLast().key.form != noForm && 
			parts.getLast().key.pitch != noPitch)
		{
			//create new part
			ScorePart newPart = ScorePart();
			newPart.measureNumber = actualMeasure;
			parts.add(newPart);
		}
		String keySign = line.substring(5);
		parseKey(keySign);
	}
	// tempo change notification
	else if(line.substring(0,7)=="\\tempo ")
	{
		if(parts.getLast().tempo.base != 0 && 
			parts.getLast().tempo.value != 0)
		{
			//create new part
			ScorePart newPart = ScorePart();
			newPart.measureNumber = actualMeasure;
			parts.add(newPart);
		}
		String tempoSign = line.substring(7);
		parseTempo(tempoSign);
	}
	// end of score
	else if(line.substring(0,1) == "}")
	{
		return;
	}
	// normal notes
	else
	{
		StringArray array=StringArray();
		array.addTokens(line,false);
		int measureSize = parts.getLast().time.count;
		for (int i=0;i<array.size();i++)
		{
			parseNote(array[i]);
		}
	}
}

void ChooseAndLoadFile::parseClef(String clef)
{
	ScorePart part = parts.getLast();
	if (clef.substring(0,7) == "treble")
		part.clef = treble;
	else if (clef.substring(0,5) == "bass")
		part.clef = bass;
	else
		part.clef = wrongClef;
	String aa = String(part.clef);
	parts.removeLast();
	parts.add(part);
}

void ChooseAndLoadFile::parseTime(String time)
{
	time.trim();
	ScorePart part = parts.getLast();
	part.time.count = (time.substring(0,time.indexOf("/"))).getIntValue();
	part.time.base = (time.substring(time.indexOf("/")+1)).getIntValue();
	parts.removeLast();
	parts.add(part);
}

void ChooseAndLoadFile::parseKey(String key)
{
	ScorePart part = parts.getLast();
	if (key.substring(0,4) == "cis" || key.substring(0,4) == "des")
	{
		part.key.pitch = cis;
	}
	else if (key.substring(0,4) == "dis" || key.substring(0,3) == "es")
	{
		part.key.pitch = dis;
	}
	else if (key.substring(0,4) == "fis" || key.substring(0,4) == "ges")
	{
		part.key.pitch = fis;
	}
	else if (key.substring(0,4) == "gis" || key.substring(0,3) == "as")
	{
		part.key.pitch = gis;
	}
	else if (key.substring(0,4) == "ais" || key.substring(0,4) == "bes")
	{
		part.key.pitch = ais;
	}
	else if (key.substring(0,1) == "c")
	{
		part.key.pitch = c;
	}
	else if (key.substring(0,1) == "d")
	{
		part.key.pitch = d;
	}
	else if (key.substring(0,1) == "e")
	{
		part.key.pitch = e;
	}
	else if (key.substring(0,1) == "f")
	{
		part.key.pitch = f;
	}
	else if (key.substring(0,1) == "g")
	{
		part.key.pitch = g;
	}
	else if (key.substring(0,1) == "a")
	{
		part.key.pitch = a;
	}
	else if (key.substring(0,4) == "b")
	{
		part.key.pitch = b;
	}
	else
	{
		part.key.pitch = wrongPitch;
	}
	
	key = key.substring(key.indexOf("\\")+1);

	if (key.contains("major"))
	{
		part.key.form = major;
	}
	else if (key.contains("minor"))
	{
		part.key.form = minor;
	}
	else
	{
		part.key.form = wrongForm;
	}

	parts.removeLast();
	parts.add(part);
}

void ChooseAndLoadFile::parseTempo(String tempo)
{
	ScorePart part = parts.getLast();
	
	StringArray tempoArray=StringArray();
	tempoArray.addTokens(tempo,false);

	if(tempoArray.size()>2)
	{
		String baseString = tempoArray[tempoArray.size()-3];

		// baza
		// policzenie kropek
		int dotsCount = 0;
		for (int i=0;i<baseString.length();++i)
		{
			if(baseString.substring(i,i+1) == ".")
			{
				dotsCount++;
			}
		}
		// obciêcie kropek
		baseString = baseString.substring(0,baseString.length()-dotsCount+1);
		// wartoœæ
		int duration = baseString.getIntValue();
				
		duration = ceil( (float)part.time.base / (float)duration );
		for (int i=0;i<dotsCount;i++)
		{
			duration *= 1.5;
		}
		part.tempo.base = duration;

		// wartoœæ
		part.tempo.value = tempoArray[tempoArray.size()-1].getIntValue();
	}

	parts.removeLast();
	parts.add(part);
}

void ChooseAndLoadFile::parseNote(String noteString)
{
	ScorePart part = parts.getLast();
	Note note;

	// pitch
	if (noteString.substring(0,3) == "cis" || noteString.substring(0,3) == "des")
	{
		note.pitch = cis;
	}
	else if (noteString.substring(0,3) == "dis" || noteString.substring(0,2) == "es")
	{
		note.pitch = dis;
	}
	else if (noteString.substring(0,3) == "fis" || noteString.substring(0,3) == "ges")
	{
		note.pitch = fis;
	}
	else if (noteString.substring(0,3) == "gis" || noteString.substring(0,2) == "as")
	{
		note.pitch = gis;
	}
	else if (noteString.substring(0,3) == "ais" || noteString.substring(0,3) == "bes")
	{
		note.pitch = ais;
	}
	else if (noteString.substring(0,1) == "c")
	{
		note.pitch = c;
	}
	else if (noteString.substring(0,1) == "d")
	{
		note.pitch = d;
	}
	else if (noteString.substring(0,1) == "e")
	{
		note.pitch = e;
	}
	else if (noteString.substring(0,1) == "f")
	{
		note.pitch = f;
	}
	else if (noteString.substring(0,1) == "g")
	{
		note.pitch = g;
	}
	else if (noteString.substring(0,1) == "a")
	{
		note.pitch = a;
	}
	else if (noteString.substring(0,1) == "b")
	{
		note.pitch = b;
	}
	else if (noteString.substring(0,1) == "r")
	{
		note.pitch = rest;
	}
	else
	{
		note.pitch = wrongPitch;
	}

	if (noteString.substring(0,3) == "cis" || noteString.substring(0,3) == "des" || noteString.substring(0,3) == "dis" || noteString.substring(0,3) == "fis" ||
		noteString.substring(0,3) == "ges" || noteString.substring(0,3) == "gis" || noteString.substring(0,3) == "ais" || noteString.substring(0,3) == "bes" )
	{
		noteString = noteString.substring(3);
	}
	else if (noteString.substring(0,2) == "es" || noteString.substring(0,2) == "as")
	{
		noteString = noteString.substring(2);
	}
	else if (noteString.substring(0,1) == "c" || noteString.substring(0,1) == "d" || noteString.substring(0,1) == "e" || noteString.substring(0,1) == "f" || 
		noteString.substring(0,1) == "g" || noteString.substring(0,1) == "a" || noteString.substring(0,1) == "b" || noteString.substring(0,1) == "r")
	{
		noteString = noteString.substring(1);
	}

	// octave
	if (parts.getLast().notes.size() > 0)
	{
		note.octave = parts.getLast().notes.getLast().octave;
		if (parts.getLast().notes.getLast().pitch - note.pitch > 5)
		{
			++(note.octave);
			//fprintf(aaa,"octave ++ pitch roznica=%d\n",parts.getLast().notes.getLast().pitch - note.pitch);
		}
		else if (parts.getLast().notes.getLast().pitch - note.pitch < -5)
		{
			--(note.octave);
			//fprintf(aaa,"octave -- pitch roznica=%d\n",parts.getLast().notes.getLast().pitch - note.pitch);
		}
	}
	else
	{
		note.octave = parts[parts.size()-2].notes.getLast().octave;
		if (parts[parts.size()-2].notes.getLast().pitch - note.pitch > 5)
		{
			++(note.octave);
		}
		else if (parts[parts.size()-2].notes.getLast().pitch - note.pitch < -5)
		{
			--(note.octave);
		}
	}
	for (int i=0;i<noteString.length();++i)
	{
		if(noteString.substring(i,i+1) == "'")
		{
			note.octave++;
			noteString = noteString.substring(1);
		}
		if (noteString.substring(i,i+1) == ",")
		{
			note.octave--;
			noteString = noteString.substring(1);
		}
	}

	// duration
	int dotsCount = 0;
	for (int i=0;i<noteString.length();++i)
	{
		if(noteString.substring(i,i+1) == ".")
		{
			dotsCount++;
		}
	}
	noteString = noteString.substring(0,noteString.length()-dotsCount+1);
	
	if (noteString.length() == 0)
	{
		if (parts.getLast().notes.size() > 0)
			note.duration = parts.getLast().notes.getLast().duration;
		else
			note.duration = parts[parts.size()-2].notes.getLast().duration;
	}
	else
	{
		float duration = noteString.getIntValue();

		note.duration = (float)part.time.base / (float)duration;
		for (int i=0;i<dotsCount;i++)
		{
			note.duration *= 1.5;
		}
	}

	actualMeasureCount += note.duration;
	if (actualMeasureCount >= parts[parts.size()-1].time.count)
	{
		actualMeasure++;
		actualMeasureCount = 0;
	}
	part.notes.add(note);

	parts.removeLast();
	parts.add(part);
}


//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
Component* chooseAndLoadFileComponent()
{
	return ChooseAndLoadFile::getInstance();
}
ChooseAndLoadFile* chooseAndLoadFile()
{
	return ChooseAndLoadFile::getInstance();
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

END_JUCER_METADATA
*/
#endif

// Static member declarations and resources would go here... (these aren't shown in the code preview)

