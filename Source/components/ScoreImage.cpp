#include "ScoreImage.h"

ScoreImage::ScoreImage(Array<ScorePart> scoreParts_)
	: scoreParts(scoreParts_)
{

}

ScoreImage::~ScoreImage()
{
};

void ScoreImage::paint (Graphics& g)
{
}

void ScoreImage::resized()
{
}

void ScoreImage::timerCallback()
{
	repaint();
}
