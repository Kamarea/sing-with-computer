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
	void playClicked();
	void stopClicked();

	bool isRecording;

private:
	Image scoreImage;
	Image keyImage;
	Image flagImage;
	Array<ScorePart> score;
	int actualPoint;
	int scoreHeight;
	int scoreWidth;
	int noteHeight;
	int metrumValue;
	int metrumBase;
	int actualPosition;

	void drawMetrum(Graphics*);
	void drawBarLine(Graphics*);
	void drawNote(Graphics*, Note);
};
