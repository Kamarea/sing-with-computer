//#include "../component_headers.h"
#include "ScoreParts.h"
#include "ChooseAndLoadFile.h"

Array<ScorePart> getScore(ChooseAndLoadFile* loader)
{
	Array<ScorePart> array;
	array=loader->parts;
	return array;//loader->parts;
};

float getPitch(Pitch pitch)
{
	float frequency;
	if (pitch == Pitch::noPitch || pitch == Pitch::rest || pitch == Pitch::wrongPitch)
		frequency = FreqNoPitch;
	if (pitch == Pitch::c)
		frequency = FreqC;
	if (pitch == Pitch::cis)
		frequency = FreqCis;
	if (pitch == Pitch::d)
		frequency = FreqD;
	if (pitch == Pitch::dis)
		frequency = FreqDis;
	if (pitch == Pitch::e)
		frequency = FreqE;
	if (pitch == Pitch::f)
		frequency = FreqF;
	if (pitch == Pitch::fis)
		frequency = FreqFis;
	if (pitch == Pitch::g)
		frequency = FreqG;
	if (pitch == Pitch::gis)
		frequency = FreqGis;
	if (pitch == Pitch::a)
		frequency = FreqA;
	if (pitch == Pitch::ais)
		frequency = FreqAis;
	if (pitch == Pitch::b)
		frequency = FreqB;

	return frequency;
};
