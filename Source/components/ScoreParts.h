
#ifndef _SCOREPARTS_H_
#define _SCOREPARTS_H_

#include "../component_headers.h"

#define LOG_10_2 0.30102999566398119521373889472449

const String pitchNames[] = {"1A","1AIS","1B",
				"C","CIS","D","DIS","E","F","FIS","G","GIS","A","AIS","B",
				"c","cis","d","dis","e","f","fis","g","gis","a","ais","b",
				"c1","cis1","d1","dis1","e1","f1","fis1","g1","gis1","a1","ais1","b1",
				"c2","cis2","d2","dis2","e2","f2","fis2","g2","gis2","a2","ais2","b2",
				"c3","cis3","d3","dis3","e3","f3","fis3","g3","gis3","a3","ais3","b3"};
const int pitchCount = 51;

struct TimeSignature {
	int count;
	int base;
};

struct Tempo {
	int base;
	int value;
};

enum Clef {
	wrongClef = -1,
	noClef,
	treble,
	bass
};

enum Pitch {
	wrongPitch = -1,
	noPitch,
	c,
	cis,
	d,
	dis,
	e,
	f,
	fis,
	g,
	gis,
	a,
	ais,
	b,
	rest
};

#define	FreqNoPitch 0;
#define	FreqC 261.63;
#define	FreqCis 277.18;
#define	FreqD 293.66;
#define	FreqDis 311.13;
#define	FreqE 329.63;
#define	FreqF 349.23;
#define	FreqFis 369.99;
#define	FreqG 392.00;
#define	FreqGis 415.30;
#define	FreqA 440.00;
#define	FreqAis 466.16;
#define	FreqB 493.88;

enum Form {
	wrongForm = -1,
	noForm,
	major,
	minor
};

struct Key {
	Pitch pitch;
	Form form;
};

struct Note {
	Pitch pitch;
	int duration;
	int octave;
};

struct ScorePart {
	int measureNumber;
	Clef clef;
	TimeSignature time;
	Key key;
	Tempo tempo;
	Array<Note> notes;

	ScorePart(){
		clef = noClef;
		key.pitch = noPitch;
		key.form = noForm;
		time.count = 0;
		time.base = 0;
		tempo.base = 0;
		tempo.value = 0;
		measureNumber = 0;
	}
};

float getPitch(Pitch pitch);

class ChooseAndLoadFile;
//class LiveAudioPitchDisplayComp;
Array<ScorePart> getScore(ChooseAndLoadFile*);//{return Array<ScorePart>();};
//static Array<float> getPitches(LiveAudioPitchDisplayComp*);

#endif // _SCOREPARTS_H_
