
#ifndef _SCOREPARTS_H_
#define _SCOREPARTS_H_

#include "../component_headers.h"

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

class ChooseAndLoadFile;
//class LiveAudioPitchDisplayComp;
Array<ScorePart> getScore(ChooseAndLoadFile*);//{return Array<ScorePart>();};
//static Array<float> getPitches(LiveAudioPitchDisplayComp*);

#endif // _SCOREPARTS_H_
