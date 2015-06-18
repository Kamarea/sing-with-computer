#include "ScoreImage.h"

ScoreImage::ScoreImage(Array<ScorePart> scoreParts_)
	: score(scoreParts_),
	scoreHeight(1000),
	actualPosition(0),
	isRecording(false)
{
	noteHeight = scoreHeight / 24;
	// key
	File violinImageFile = File(File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getChildFile("images").getChildFile("klucz-wiolinowy.png"));
	Image violinImage = ImageCache::getFromFile(violinImageFile);
	keyImage = Image(Image::PixelFormat::RGB,200,1000,true);
	Graphics keyGraphics (keyImage);
	keyGraphics.fillAll(Colours::white);
	keyGraphics.setColour(Colours::black);
	keyGraphics.fillRect(0,scoreHeight / 2 - 1, 200, 3);
	keyGraphics.fillRect(0,scoreHeight / 2 - noteHeight - 1, 200, 3);
	keyGraphics.fillRect(0,scoreHeight / 2 + noteHeight - 1, 200, 3);
	keyGraphics.fillRect(0,scoreHeight / 2 - 2 * noteHeight - 1, 200, 3);
	keyGraphics.fillRect(0,scoreHeight / 2 + 2 * noteHeight - 1, 200, 3);
	keyGraphics.drawImageWithin(violinImage, 0, scoreHeight / 2 - 3.5 * noteHeight, 200, 7 * noteHeight, RectanglePlacement(4 + 32 + 128));

	//score
	scoreWidth = 10000;
	scoreImage = Image(Image::PixelFormat::RGB,10000,1000,true);
	Graphics graphics (scoreImage);
	graphics.fillAll(Colours::white);
	// staff
	graphics.setColour(Colours::black);
	graphics.fillRect(0,scoreHeight / 2 - 1, scoreWidth, 3);
	graphics.fillRect(0,scoreHeight / 2 - noteHeight - 1, scoreWidth, 3);
	graphics.fillRect(0,scoreHeight / 2 + noteHeight - 1, scoreWidth, 3);
	graphics.fillRect(0,scoreHeight / 2 - 2 * noteHeight - 1, scoreWidth, 3);
	graphics.fillRect(0,scoreHeight / 2 + 2 * noteHeight - 1, scoreWidth, 3);

	actualPoint = 10;
	// metrum
	metrumValue = score[0].time.count;
	metrumBase= score[0].time.base;
	drawMetrum(&graphics);

	// notes
	File flagImageFile = File(File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getChildFile("images").getChildFile("ogonek.png"));
	flagImage = ImageCache::getFromFile(flagImageFile);

	float count = 0;
	for (int i = 1; i < score[0].notes.size(); i++)
	{
		drawNote(&graphics, score[0].notes[i]);
		count += score[0].notes[i].duration;
		if (count >= metrumValue)
		{
			drawBarLine(&graphics);
			count = 0;
		}
	}
	
	startTimer (25); 
}

ScoreImage::~ScoreImage()
{
};

void ScoreImage::drawMetrum(Graphics* graphics)
{
	graphics->setFont(3 * noteHeight);
	graphics->drawText(String(metrumValue), actualPoint, scoreHeight / 2 - 2 * noteHeight, 2 * noteHeight, 2 * noteHeight, Justification::left, false);
	graphics->drawText(String(metrumBase), actualPoint, scoreHeight / 2, 2 * noteHeight, 2 * noteHeight, Justification::left, false);

	actualPoint += 3 * noteHeight;
}

void ScoreImage::drawBarLine(Graphics* g)
{
	g->fillRect(actualPoint, scoreHeight / 2 - 2 * noteHeight, 5, 4 * noteHeight);
	actualPoint += 30;
}

void ScoreImage::drawNote(Graphics* g, Note note)
{
		int pitch = note.pitch;
		pitch += note.octave * 14;
		pitch = (pitch / 2);
		int move = pitch - 7;
		g->setColour(Colours::black);
		int ellipseY = scoreHeight / 2 - noteHeight / 2 * move - noteHeight / 2;
		
		if (note.duration / metrumBase >= 0.5)
			g->drawEllipse(actualPoint, ellipseY, 2 * noteHeight, noteHeight, 3);
		else
			g->fillEllipse(actualPoint, ellipseY, 2 * noteHeight, noteHeight);
		if ( move < -5 || move > 5)
		{
			g->drawHorizontalLine(ellipseY + noteHeight / 2, actualPoint - noteHeight / 4, actualPoint + 2 * noteHeight + noteHeight / 4);
			g->drawHorizontalLine(ellipseY + noteHeight / 2 - 1, actualPoint - noteHeight / 4, actualPoint + 2 * noteHeight + noteHeight / 4);
			g->drawHorizontalLine(ellipseY + noteHeight / 2 + 1, actualPoint - noteHeight / 4, actualPoint + 2 * noteHeight + noteHeight / 4);
		}

		// stem
		if (note.duration / metrumBase != 1)
		{
			if (move < 0)
				g->fillRect(actualPoint + 2 * noteHeight, ellipseY + noteHeight / 2 - 4 * noteHeight, 4, 4 * noteHeight);
			else
				g->fillRect(actualPoint, ellipseY + noteHeight / 2, 4, 4 * noteHeight);
			actualPoint += 4;
		}
		// flags
		if (note.duration / metrumBase < 0.25)
		{
			// 8
			if (move >= 0)
			{
				g->drawImageTransformed(flagImage, AffineTransform::verticalFlip(flagImage.getHeight()).followedBy(
					AffineTransform::scale((float)flagImage.getWidth() / (noteHeight / 2) / 8, 
					(float)flagImage.getHeight() / (noteHeight / 2) / 8).followedBy(
					AffineTransform::translation(actualPoint, ellipseY + 2 * noteHeight - noteHeight / 2))));
			}
			else
				g->drawImageWithin(flagImage, actualPoint + 2 * noteHeight, ellipseY - 4 * noteHeight + noteHeight / 2, 
				noteHeight / 2, 3 * noteHeight, RectanglePlacement(1 + 8 + 64 + 128));

			// 16
			if (note.duration / metrumBase < 0.125)
			{
				if (move >= 0)
				{
					g->drawImageTransformed(flagImage, AffineTransform::verticalFlip(flagImage.getHeight()).followedBy(
						AffineTransform::scale((float)flagImage.getWidth() / (noteHeight / 2) / 8, 
						(float)flagImage.getHeight() / (noteHeight / 2) / 8).followedBy(
						AffineTransform::translation(actualPoint, ellipseY + noteHeight / 2))));
				}
				else
					g->drawImageWithin(flagImage, actualPoint + 2 * noteHeight, ellipseY - 3 * noteHeight + noteHeight / 2, 
					noteHeight / 2, 3 * noteHeight, RectanglePlacement(1 + 8 + 64 + 128));
			}
		}
		

		actualPoint += 2 * noteHeight;
		actualPoint += 4 * 2 * noteHeight * note.duration;
}

void ScoreImage::paint (Graphics& g)
{
	if (isRecording)
		actualPosition += 4 * 2 * noteHeight / 20 * 2.1;
	float scale = (float)getHeight() / scoreHeight;
	g.drawImage(keyImage.rescaled(200 * scale,getHeight()), 0, 0, 200 * scale, getHeight(),0,0,200 * scale,getHeight());
	g.drawImage(scoreImage.rescaled(scoreWidth * scale,getHeight()),200 * scale,0,getWidth(),getHeight(),actualPosition,0,getWidth() - 200 * scale,getHeight());
	g.setColour(Colours::darkgreen);
	g.drawVerticalLine((200 + 10 + 3 * noteHeight) * scale, 0, getHeight());
	g.setColour(Colours::black);
	g.setFont(40);
	g.drawFittedText(Globals::getInstance()->title,getWidth() / 2 - 100, 10, 200, 50, 36, 1);
	g.setFont(20);
	g.drawFittedText(Globals::getInstance()->composer,getWidth() - 220, 60, 200, 30, 34, 1);
}

void ScoreImage::playClicked()
{
	isRecording = true;
	int tempoBase = score[0].tempo.base;
	int tempoValue = score[0].tempo.value;
	// base - 4 - 4 * 2 * scoreHeight / 24 * note.duration
}

void ScoreImage::stopClicked()
{
	isRecording = false;
	actualPosition = 0;
}

void ScoreImage::resized()
{
}

void ScoreImage::timerCallback()
{
	repaint();
}
