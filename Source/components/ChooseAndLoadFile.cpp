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
	
	fileChooser = new FileChooser("Choose file...",File::nonexistent,"*.ly");
	if(!fileChooser->browseForFileToOpen())
		return;

	openFile = fileChooser->getResult();
	data = openFile.loadFileAsString();
	int position=0;
	readScore(position);

    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

ChooseAndLoadFile::~ChooseAndLoadFile()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

	
	deleteAndZero(fileChooser);

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================

void ChooseAndLoadFile::readScore(int position)
{
	int score = data.indexOf(position,"\\score");
	if(score>data.length())
		return;
	//String tmp = String(score);
	
	FILE* aaa = fopen("aaa.txt","w");

	score = data.indexOf(score,"\\relative");
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
	
	fprintf(aaa,"%s\n",relativeCount);
	ScorePart part;
	part.measureNumber = 0;
	part.clef = noClef;
	part.key.pitch = noPitch;
	part.key.form = noForm;
	part.time.count = 0;
	part.time.base = 0;
	part.tempo.base=0;
	part.tempo.value=0;
	parts.add(part);
	actualMeasure = 0;

	Note note;
	note.duration = 0;
	note.pitch = c;
	note.octave = mainOctave;
	
	int begOfLine = data.indexOf(score+1,"\n");
	int endOfLine = data.indexOf(begOfLine+1,"\n");
	String line = data.substring(begOfLine+1,endOfLine-1).trimStart();
	fprintf(aaa,"%s\n",line);
	while(!line.isEmpty())
	{
		parseLine(line,aaa);
		begOfLine = endOfLine;
		endOfLine = data.indexOf(begOfLine+1,"\n");
		line = data.substring(begOfLine+1,endOfLine-1).trimStart();
		fprintf(aaa,"%s\n",line);
	}
	
	fclose(aaa);
}

void ChooseAndLoadFile::parseLine(String line, FILE* aaa)
{
	fprintf(aaa,"line: %s\n",line);
	// clef change notification
	if(line.substring(0,6)=="\\clef ")
	{
		fprintf(aaa,"clef\n");
		if(parts.getLast().clef!=noClef)
		{
			//create new part
			ScorePart newPart = ScorePart();
			newPart.measureNumber = actualMeasure;
			parts.add(newPart);
			fprintf(aaa,"newPart clef\n");
		}
		String clefSign = line.substring(6);
		parseClef(clefSign,aaa);
	}
	// timing change notification
	else if(line.substring(0,6)=="\\time ")
	{
		fprintf(aaa,"time\n");
		if(parts.getLast().time.count !=0 &&
			parts.getLast().time.base != 0)
		{
			//create new part
			ScorePart newPart = ScorePart();
			newPart.measureNumber = actualMeasure;
			parts.add(newPart);
			fprintf(aaa,"newPart time\n");
		}
		String timeSign = line.substring(6);
		parseTime(timeSign,aaa);
	}
	// key change notification
	else if(line.substring(0,5)=="\\key ")
	{
		fprintf(aaa,"key\n");
		if(parts.getLast().key.form != noForm && 
			parts.getLast().key.pitch != noPitch)
		{
			//create new part
			ScorePart newPart = ScorePart();
			newPart.measureNumber = actualMeasure;
			parts.add(newPart);
			fprintf(aaa,"newPart key\n");
		}
		String keySign = line.substring(5);
		parseKey(keySign,aaa);
	}
	// tempo change notification
	else if(line.substring(0,7)=="\\tempo ")
	{
		fprintf(aaa,"tempo\n");
		if(parts.getLast().tempo.base != 0 && 
			parts.getLast().tempo.value != 0)
		{
			//create new part
			ScorePart newPart = ScorePart();
			newPart.measureNumber = actualMeasure;
			parts.add(newPart);
			fprintf(aaa,"newPart tempo\n");
		}
		String tempoSign = line.substring(7);
		parseTempo(tempoSign,aaa);
	}
	// end of score
	else if(line.substring(0,1) == "}")
	{
		return;
	}
	// normal notes
	else
	{
		fprintf(aaa,"normal notes\n");
		StringArray array=StringArray();
		array.addTokens(line,false);
		int measureSize = parts.getLast().time.count;
		for (int i=0;i<array.size();i++)
		{
			fprintf(aaa,"read:%s\n",array[i]);
			parseNote(array[i],aaa);
		}
	}
}

void ChooseAndLoadFile::parseClef(String clef, FILE* aaa)
{
	ScorePart part = parts.getLast();
	if (clef.substring(0,7) == "treble")
		part.clef = treble;
	else if (clef.substring(0,5) == "bass")
		part.clef = bass;
	else
		part.clef = wrongClef;
	String aa = String(part.clef);
	fprintf(aaa,"clef recognized: %s\n",aa);
	parts.removeLast();
	parts.add(part);
}

void ChooseAndLoadFile::parseTime(String time, FILE* aaa)
{
	time.trim();
	ScorePart part = parts.getLast();
	part.time.count = (time.substring(0,time.indexOf("/"))).getIntValue();
	part.time.base = (time.substring(time.indexOf("/")+1)).getIntValue();
	String aa = String(part.time.count)+"/"+String(part.time.base);
	fprintf(aaa,"time recognized: %s\n",aa);
	parts.removeLast();
	parts.add(part);
}

void ChooseAndLoadFile::parseKey(String key, FILE* aaa)
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

	String aa = String(part.key.pitch)+" "+String(part.key.form);
	fprintf(aaa,"key recognized: %s\n",aa);
	parts.removeLast();
	parts.add(part);
}

void ChooseAndLoadFile::parseTempo(String tempo, FILE* aaa)
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

	fprintf(aaa,"tempo recognized: %d = %d\n",part.tempo.base, part.tempo.value);
	parts.removeLast();
	parts.add(part);
}

void ChooseAndLoadFile::parseNote(String noteString, FILE* aaa)
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
			//fprintf(aaa,"octave ++");
		}
		else if (parts[parts.size()-2].notes.getLast().pitch - note.pitch < -5)
		{
			--(note.octave);
			//fprintf(aaa,"octave --");
		}
	}
	// TODO: zmiana oktawy dla bliskich dŸwiêków
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
		int duration = noteString.getIntValue();

		note.duration = ceil( (float)part.time.base / (float)duration );
		for (int i=0;i<dotsCount;i++)
		{
			note.duration *= 1.5;
		}
	}

	part.notes.add(note);

	String aa = String(note.pitch)+" "+String(note.duration)+" "+String(note.octave);
	fprintf(aaa,"note recognized: %s\n",aa);
	parts.removeLast();
	parts.add(part);
}

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
    //[/UserResized]
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

