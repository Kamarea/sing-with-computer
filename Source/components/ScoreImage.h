#include "ScoreParts.h"

class ScoreImage :	public Component,
					public Timer
{
public:
	ScoreImage(Array<ScorePart> scoreParts);
	~ScoreImage();

	void paint (Graphics& g);
    void resized();
    void timerCallback();

private:
	Image scoreImage;
	Array<ScorePart> scoreParts;
};
