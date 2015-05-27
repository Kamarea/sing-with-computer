#include "ScoreImage.h"

ScoreImage::ScoreImage(Array<ScorePart> scoreParts_)
	: scoreParts(scoreParts_)
{
	scoreImage = Image(Image::PixelFormat::RGB,3000,1000,true);
	Graphics graphics (scoreImage);
	graphics.fillAll(Colours::white);
	graphics.setColour(Colours::black);
	graphics.drawHorizontalLine(1000 / 2, 0, 3000);
	graphics.drawHorizontalLine(1000 / 2 - 1, 0, 3000);
	graphics.drawHorizontalLine(1000 / 2 + 1, 0, 3000);
	graphics.drawHorizontalLine(1000 / 2 - 1000 / 24, 0, 3000);
	graphics.drawHorizontalLine(1000 / 2 - 1000 / 24 - 1, 0, 3000);
	graphics.drawHorizontalLine(1000 / 2 - 1000 / 24 + 1, 0, 3000);
	graphics.drawHorizontalLine(1000 / 2 + 1000 / 24, 0, 3000);
	graphics.drawHorizontalLine(1000 / 2 + 1000 / 24 - 1, 0, 3000);
	graphics.drawHorizontalLine(1000 / 2 + 1000 / 24 + 1, 0, 3000);
	graphics.drawHorizontalLine(1000 / 2 - 2 * 1000 / 24, 0, 3000);
	graphics.drawHorizontalLine(1000 / 2 - 2 * 1000 / 24 - 1, 0, 3000);
	graphics.drawHorizontalLine(1000 / 2 - 2 * 1000 / 24 + 1, 0, 3000);
	graphics.drawHorizontalLine(1000 / 2 + 2 * 1000 / 24, 0, 3000);
	graphics.drawHorizontalLine(1000 / 2 + 2 * 1000 / 24 - 1, 0, 3000);
	graphics.drawHorizontalLine(1000 / 2 + 2 * 1000 / 24 + 1, 0, 3000);
	actualPoint = 10;
	drawMetrum(&graphics);
	startTimer (1000 / 5); 
}

ScoreImage::~ScoreImage()
{
};

void ScoreImage::drawMetrum(Graphics* graphics)
{
	int metrumValue = scoreParts[0].time.count;
	int metrumBase= scoreParts[0].time.base;
	graphics->setFont(3 * 1000 / 24);
	graphics->drawText(String(metrumValue), actualPoint, 1000 / 2 - 2 * 1000 / 24, 2 * 1000 / 24, 2 * 1000 / 24, Justification::left, false);
	graphics->drawText(String(metrumBase), actualPoint, 1000 / 2, 2 * 1000 / 24, 2 * 1000 / 24, Justification::left, false);

	actualPoint += 3 * 1000 / 24;
}

void ScoreImage::paint (Graphics& g)
{
	float scale = (float)getHeight() / 1000;
	g.drawImage(scoreImage.rescaled(3000 * scale,getHeight()),0,0,getWidth(),getHeight(),0,0,getWidth(),getHeight());
}

void ScoreImage::resized()
{
}

void ScoreImage::timerCallback()
{
	repaint();
}
