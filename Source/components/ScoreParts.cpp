//#include "../component_headers.h"
#include "ScoreParts.h"
#include "ChooseAndLoadFile.h"

Array<ScorePart> getScore(ChooseAndLoadFile* loader)
{
	Array<ScorePart> array;
	array=loader->parts;
	return array;//loader->parts;
};