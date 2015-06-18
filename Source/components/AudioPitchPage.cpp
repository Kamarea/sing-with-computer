#include "AudioPitchPage.h"
#include "AudioTabComponent.h"
#include <list>
//#include "logs.h"

AudioPitchPage* AudioPitchPage::instance;

//[MiscUserDefs] You can add your own user definitions and misc code here...
AudioPitchPage::AudioPitchPage()
{
    accumulator = 0;
	numberOfSamplesRead = 0;
	numberOfSamplesRecalculated = 512;
	recordedScoreNumber = 0;
    setOpaque (true);
	test=0;
	frequency=-2;
	samplesNumber=0;
	isRecording = false;
	hasScore = false;
	pitchTreshold = Globals::getInstance()->getPitchTreshold();

    startTimer (1000 / 50); // use a timer to keep repainting this component
}

AudioPitchPage::~AudioPitchPage()
{
	lock.enter();
	if(isRecording)
		stopClicked();
	lock.exit();
}

void AudioPitchPage::paint (Graphics& g)
{
    g.fillAll (Colours::white);

    const float midY = getHeight() * 0.5f;
	float oneHzSize = (float)getHeight() / 1800.0f;
		
	float semitone = (float)getHeight()/pitchCount;

	// draw background
	g.setFont(semitone);
	for (float i=0.5;i<pitchCount;++i)
	{
		if((int)i%2==0)
		{
			g.setColour(Colours::lightgrey);
			g.fillRect(0.0, i*semitone, (float)getWidth(), semitone);
		}
		else
		{
			g.setColour(Colours::white);
			g.fillRect(0.0, i*semitone, (float)getWidth(), semitone);
		}
		g.setColour(Colours::black);
		g.drawFittedText(pitchNames[pitchCount-(int)i-1],0,(int)((i)*semitone),50,(int)semitone,Justification::left,1);
		g.drawFittedText(pitchNames[pitchCount-(int)i-1],getWidth()-50,(int)((i)*semitone),50,(int)semitone,Justification::right,1);
	}

	// draw pitches
	
	lock.enter();
	// wczytaæ ile jest do narysowania - max getWidth();
	// dla tylu ile jest do  narysowania narysowaæ
	int drawNumber = pitches.size() > (getWidth() - 40) ? 
		(getWidth() - 40) : pitches.size();
	lock.exit();
	// draw score pitches
	if (isRecording)
	{
		int recordedSize = (pitches.size() - begin) > (getWidth() - 40) ?
			(getWidth() - 40) : (pitches.size() - begin);
		g.setColour(Colours::green);
		for (int x = 0; x < recordedSize; ++x)
		{
			if (x < scorePitches.size())
			{
				int y=getHeight() - (scorePitches[x] - 33) * semitone;
				g.drawVerticalLine (getWidth() - 20  - recordedSize + x, y, y+1);
			}
		}
	}
	// draw recorded pitches
	g.setColour(Colours::blue);
	lock.enter();
    for (int x = 0; x < drawNumber; ++x)
    {
        int y=(float)getHeight() - 
			(pitches[pitches.size() - drawNumber + x] - 33) * semitone;
        g.drawVerticalLine (getWidth() - 20 -drawNumber + x, y, y+1);
    }
	lock.exit();
	
	//g.drawFittedText(String(numberOfSamplesRead),getWidth()/2,100,100,100,Justification::left,1);
}

void AudioPitchPage::timerCallback()
{
    repaint();
}

void AudioPitchPage::updateSamples(int number, std::vector<float>* samples)
{
	if(numberOfSamplesRead < number)
	{
		// przepisac sample
		for (int i = numberOfSamplesRead; i < number; i++)
		{
			allSamples.push_back((*samples)[i]);
		}
		numberOfSamplesRead = number;

		float tempSamples[1024];
		float pitch;
		float tempPitch;
		int tempSize;
		if (numberOfSamplesRead >= 3 * 512)
		{
			while(number - numberOfSamplesRecalculated >= 1024)
			{
				for (int i = 0; i < 1024; i++)
					tempSamples[i] = allSamples[numberOfSamplesRecalculated - 512 + i];
				tempPitch = computePitch(tempSamples) / 1.32f;
				pitch=std::max<float>(0.0, 69 + 12 * (log10(tempPitch / 440) / log_10_2));

				lock.enter();
				tempSize = pitches.size();
				if (tempSize > 1)
				{
					if (pitch > 0 && pitches[tempSize - 1] <= 0 && pitches[tempSize - 2] > 0)
					{
						pitches[tempSize - 1] = (pitch + pitches[tempSize - 2]) / 2;
					}
				}
				
				pitches.push_back(pitch);
				lock.exit();

				numberOfSamplesRecalculated += 512;
			}
		}
	}
}

float AudioPitchPage::computePitch(float *my_samples){
	float freq=0.0f;				// freq to return
	int lev=6;						// levels of analysis
	float globalMaxTreshold=pitchTreshold;	// threshold of maximum values to consider
	int maxFreq = 3000;				// minimum distance to consider valid
	int diffLevs = 3;				// number of diferences to go through
	int fs=44100;					// sample rate
	float oldMode=0;
	if(frequency>0) oldMode = fs/frequency;

	int newWidth;

	float a[6][1024];
	zeromem(a,sizeof(a));
	for(int i=0;i<1024;i++) a[0][i]=my_samples[i];

	float aver = mean(a[0]);
	float globalMin=min(a[0]);
	float globalMax=max(a[0]);
	float maxTresh = globalMaxTreshold * (globalMax - aver) + aver;
	float minTresh = globalMaxTreshold * (globalMin - aver) + aver;

	float d[6][1024];
	zeromem(d,sizeof(d));
	int minDist;
	int maxCount[6];
	int minCount[6];
	zeromem(maxCount,sizeof(maxCount));
	zeromem(minCount,sizeof(minCount));
	int maxIndices[6][512];
	int minIndices[6][512];
	int differs[1024];

	int climber;
	bool canExt;
	int tooClose;
	float test;
	int currentOffset;
	int numer;
	int numerJ;
	float mode[5];
	mode[0]=0;
	float sum;
	int count;

	int level,j,k;	// control variables

	// pitch detection
	// 6 levels of tracking until no pitch found
	for(level=1;level<lev;level++){
		newWidth=(int)(1024.0f/(power(2,level)));
		//audioLog("newWidth=%d\n",newWidth);
		//perform FLWT
		for(j=0;j<newWidth;j++)
		{
			d[level][j]=a[level-1][2*j+1]-a[level-1][2*j];
			a[level][j]=a[level-1][2*j]+d[level][j]/2;
		}

		// Find the maxes of the current approximation
		minDist = fs/maxFreq/power(2,level);
		minDist = minDist>1 ? minDist : 1;
		//audioLog("minDist=%d\n",minDist);

		// kierunek poczatku probek
		climber=0;
		//sprawdzic = 0
		if(a[level][1]-a[level][0]>0) climber=1;
		else climber=-1;

		//audioLog("climber=%d\n",climber);
		
		canExt=true;	// Tracks whether an extreme can be found (based on zero crossing)
		tooClose=0;	// Tracks how many more samples nust be moved before another extreme

		for(j=1;j<newWidth-1;j++)
		{
			// kierunek ostatnich dwoch probek
			test = a[level][j]-a[level][j-1];

			// poczatek probek rosnacy, aktualnie malejaco
			if(climber >= 0 && test < 0)
			{
				// poprzednia próbka
				if(a[level][j-1] >= maxTresh && canExt && tooClose == 0)
				{
					maxCount[level]++;
					maxIndices[level][maxCount[level]]=j-1;
					canExt = false;
					tooClose = minDist;
				}
				climber = -1;
			}
			else if(climber <=0 && test > 0)
			{
				if(a[level][j-1] <= minTresh && canExt && tooClose == 0)
				{
					minCount[level]++;
					minIndices[level][minCount[level]] = j-1;
					canExt = false;
					tooClose = minDist;
				}
				climber = 1;
			}

			if((a[level][j]<aver && a[level][j-1]>aver) || (a[level][j]>aver && a[level][j-1]<aver))
				canExt = true;

			if(tooClose != 0)
				tooClose--;
		}
		
		// calculate the mode distance between peaks at each level
		if(maxCount[level] >= 2 && minCount[level] >=2)
		{

			// calculate the differences at diffLevs distances
			zeromem(differs,sizeof(differs));
			currentOffset=0;
			for(j=1;j<=3;j++)
			{
				for(k=0;k<maxCount[level]-j;k++)
					differs[currentOffset + k] = abs(maxIndices[level][j+k] - maxIndices[level][k]);
				currentOffset+=maxCount[level]-j;
				for(k=0;k<minCount[level]-j;k++)
					differs[currentOffset + k] = abs(minIndices[level][j+k] - minIndices[level][k]);
				currentOffset+=minCount[level]-j;
			}
			// Require at least two agreeing differs to yield a mode
			numer=1;
			numerJ=0;
			mode[level]=0;
			for(j=0;j<currentOffset;j++)
			{
				for(k=0;k<currentOffset;k++)
				{
					if(abs(differs[k] - differs[j])<=minDist)
						numerJ++;
				}
				if(differs[j]>0){
				if(numerJ>=numer && numerJ > (int)(newWidth/differs[j]/4))
					if(numerJ==numer)
					{
						if(oldMode!=0 && abs(differs[j]-oldMode/power(2,level))<minDist)
							mode[level]=differs[j];
						else if(oldMode==0 && differs[j]>1.95*mode[level] && differs[j]<2.05*mode[level])
							mode[level]=differs[j];
					}
					else
					{
						numer = numerJ;
						mode[level]=differs[j];
					}
				else if(numerJ == numer-1 && oldMode!=0 && abs(differs[j]-oldMode/power(2,level))<minDist)
					mode[level]=differs[j];
				}
			}
			// Set the mode via averaging
			if(mode[level]!=0)
			{
				sum=0;
				count=0;
				for(j=0;j<currentOffset;j++)
				{
					if(abs(mode[level]-differs[j])<=minDist)
					{
						sum+=differs[j];
						count++;
					}
				}
				mode[level]=sum/count;
			}
			// Determine if the modes are shared
			if(mode[level-1]!=0 && maxCount[level]>=2 && minCount[level]>=2)
			{
				if(abs(mode[level-1]-2*mode[level])<=minDist)
				{
					freq=fs/mode[level-1]/power(2,level-1);
					return freq;
				}
			}
		}
	}
	return -1;
}

float AudioPitchPage::abs(float a){
	if(a>=0)
		return a;
	else
		return-a;
}

int AudioPitchPage::power(int a, int b){
	if(b>0)
		return a*power(a,b-1);
	else
		return 1;
}

float AudioPitchPage::mean(float a[]){
	float sum=0;
	for(int k=0;k<1024;k++) sum+=a[k];
	return sum/1024;
}

float AudioPitchPage::max(float table[]){
	float max=table[0];
	for(int i=1;i<1024;i++)
		if(table[i]>max) max=table[i];
	return max;
}

float AudioPitchPage::min(float table[]){
	float min=table[0];
	for(int i=1;i<1024;i++)
		if(table[i]<min) min=table[i];
	return min;
}

void AudioPitchPage::setScoreTablePtr(ScoreTable* table)
{
	scoreTable = table;
}

void AudioPitchPage::playClicked(File directory, int* position)
{
	pitchPosition = position;
	*position = 0;
	
	fileOut = new FileOutputStream(directory.getChildFile("pitchSamples.smp"));
	lock.enter();
	begin=pitches.size();
	isRecording = true;
	lock.exit();
	restsPercentage = 0;
	pitchPercentage = 0;
	
	
	numberOfSamplesRead = 0;
	numberOfSamplesRecalculated = 512;
	pitches.clear();
	pitchErrorsTable.clear();
	rythmErrorsTable.clear();
	allSamples.clear();
	recordedSamples.clear();
	recordedDistances.clear();
}

void AudioPitchPage::stopClicked()
{
	lock.enter();
	end=pitches.size();
	String line;
	std::vector<float> writeSamples;
	writeSamples.resize(end-begin);
	// TODO change
	for (int i=begin;i<end;i++)
	{
		writeSamples[i-begin] = pitches[i];
	}


	isRecording = false;
	for (int i = 0; i < writeSamples.size(); i++)
		fileOut->writeFloat(writeSamples[i]);
	delete fileOut;
	lock.exit();

	if(hasScore)
	{
		recordedSamples.resize(writeSamples.size());
		for (int i = 0; i < writeSamples.size(); i++)
		{
			recordedSamples[i] = writeSamples [i];
		}
		calculateDistances();
	}
}

void AudioPitchPage::calculateDistances()
{
	// wysokoœæ done
	// rytm done

	int recordedNumber = std::min<int>(scorePitches.size(), end - begin);
	int restErrors = 0;
	int rythmErrors = 0;
	float wrongPitches = 0.0;
	recordedDistances.resize(std::min<int>(end - begin, scorePitches.size()));
	std::vector<float> shortScore;
	shortScore.push_back(scorePitches[0]);
	for(int i = 0; i < recordedNumber; i++)
	{
		if (scorePitches[i] != shortScore[shortScore.size() - 1])
		{
			shortScore.push_back(scorePitches[i]);
		}
	}
	shortScore.push_back(scorePitches[recordedNumber - 1]);
	int shortScoreIter = 0;
	
	pitchErrorsTable.resize(std::min<int>(end - begin, scorePitches.size()), 0);
	rythmErrorsTable.resize(std::min<int>(end - begin, scorePitches.size()), 0);

	for (int i = 0; i < recordedNumber; i++)
	{
		if ((recordedSamples[i] <= 0 && scorePitches[i] > 0) ||
			(recordedSamples[i] > 0 && scorePitches[i] <= 0))
		{
			restErrors++;
			rythmErrorsTable[i] = 1;
		}
		else
		{
			recordedDistances[i] = abs(recordedSamples[i] - scorePitches[i]);
			if(recordedDistances[i] > 0.4)
			{
				if (abs(recordedSamples[i] - shortScore[std::min<int>(shortScore.size() - 1,shortScoreIter + 1)]) < 0.4)
				{
					recordedDistances[i] = abs(recordedSamples[i] - 
							shortScore[std::min<int>(shortScore.size() - 1,shortScoreIter + 1)]);
					if (shortScoreIter < shortScore.size())
						shortScoreIter++;
					rythmErrors++;
					rythmErrorsTable[i] = 1;
				}
				else if (abs(recordedSamples[i] - shortScore[std::min<int>(shortScore.size() - 1,shortScoreIter + 1)]) < 0.4)
				{
					recordedDistances[i] = abs(recordedSamples[i] - 
							shortScore[std::min<int>(shortScore.size() - 1,shortScoreIter + 1)]);
					rythmErrors++;
					rythmErrorsTable[i] = 1;
				}
				else
				{
					if (recordedDistances[i] < 0.75)
					{
						wrongPitches += 0.5 * recordedDistances[i];
						pitchErrorsTable[i] = 0.5 * recordedDistances[i];
					}
					else
					{
						wrongPitches += 1;
						pitchErrorsTable[i] = 1;
					}
				}
			}
			else
			{
				if(shortScoreIter < shortScore.size())
					if (scorePitches[i] != shortScore[shortScoreIter])
						shortScoreIter++;
			}
		}
	}

	if (scorePitches.size() >= measuresInSamples[measuresInSamples.size() - 1].second - 1)
		calculateWrongest();

	float restsWrongPercentage = (float)(restErrors + rythmErrors) / (float)recordedNumber;
	float pitchWrongPercentage = (float)wrongPitches / (float)recordedNumber;
	restsPercentage = 100 * (1 - restsWrongPercentage);
	pitchPercentage = 100 * (1 - pitchWrongPercentage);

	scoreTable->pitchPercentage = pitchPercentage;
	scoreTable->rhythmPercentage = restsPercentage;
	scoreTable->updateScores();
}

void AudioPitchPage::calculateWrongest()
{
	std::list<std::pair<int, int>> wrongs;
	std::vector<int> errors;
	errors.resize(pitchErrorsTable.size(), 0);
	// pitch
	for (int i = 0; i < errors.size(); i++)
	{
		if (pitchErrorsTable[i] > 0)
			errors[i] = 1;
	}
	// rhythm
	for (int i = 0; i < errors.size(); i++)
	{
		if (rythmErrorsTable[i] > 0)
			errors[i] = 1;
	}

	// add all errors as pairs
	int i = 0;
	while (i < errors.size())
	{
		if (errors[i] > 0)
		{
			std::pair<int, int> pair;
			pair.first = i;
			i++;
			while (i < errors.size() && errors[i] > 0)
			{
				i++;
			}
			pair.second = i;
			wrongs.push_back(pair);
		}
		i++;
	}

	// remove small not errors to smoothe
	for (std::list<std::pair<int,int>>::iterator it = wrongs.begin(); it != wrongs.end();)
	{
		if (++it != wrongs.end() && it->first - (--it)->second < 5)
		{
			int beg = it->first;
			int end = (++it)->second;
			(--it)->second = end;
			std::list<std::pair<int,int>>::iterator it2(it);
			it2++;
			wrongs.remove(*it2);
		}
		else
			if (it != wrongs.end())
				it++;

	}

	// remove small errors
	for (std::list<std::pair<int,int>>::iterator it = wrongs.begin(); it != wrongs.end();)
	{
		if (it->second - it->first < 5)
		{
			std::list<std::pair<int,int>>::iterator it2(it);
			it++;
			wrongs.remove(*it2);
		}
		else
			if (it != wrongs.end())
				it++;

	}
	// znaleŸæ najd³u¿szy b³¹d
	int max = 0;
	std::pair<int, int> longest;
	for (std::list<std::pair<int,int>>::iterator it = wrongs.begin(); it != wrongs.end(); it++)
	{
		if (it->second - it->first > max)
		{
			max = it->second - it->first;
			longest.first = it->first;
			longest.second = it->second;
		}
	}
	// dopasowaæ do pocz¹tków taktów
	std::pair<int, int> measures;

	// pocz¹tek
	int j = 0;
	while (j < measuresInSamples.size() - 1 && longest.first > measuresInSamples[j].second)
	{
		j++;
	}
	if (j != 0)
	{
		longest.first = measuresInSamples[j - 1].second;
		measures.first = measuresInSamples[j - 1].first;
	}
	else
	{
		longest.first = measuresInSamples[j].second;
		measures.first = measuresInSamples[j].first;
	}

	// koniec
	j = 0;
	while (j < measuresInSamples.size() - 1 && longest.second > measuresInSamples[j].second)
	{
		j++;
	}
	longest.second = measuresInSamples[j].second - 1;
	measures.second = measuresInSamples[j - 1].first;
	
	// sprawdziæ, czy nie za krótki fragment, ew. wyd³u¿yæ
	if ( measures.second - measures.first < 3)
	{
		if (measures.first > 1)
		{
			measures.first--;
			longest.first = measuresInSamples[measures.first].second;
		}
		if (measures.second < measuresInSamples.size() - 2)
		{
			measures.second++;
			longest.second = measuresInSamples[measures.second + 1].second - 1;
		}
	}

	// okienko - czy przeæwiczyæ
	window = new WindowExercises(measures);
	addAndMakeVisible(window);
	//userChoice->setBounds(0,0,500,200);
	int result = DialogWindow::showModalDialog(Globals::getInstance()->getTexts()[21], window, this->findParentComponentOfClass<AudioTabComponent>(), Colours::white, false);
	deleteAndZero(window);
	// jeœli tak, to przepisujemy scorePitches na te z³e
	if (result == 1)
	{
		rewriteScore(longest);
	}
	// i lecim dalej
}

void AudioPitchPage::rewriteScore(std::pair<int,int> measures)
{
	// kopia tego, co ma byæ
	std::vector<float> tmp;
	tmp.resize(measures.second - measures.first);
	for (int i = measures.first; i < measures.second; i++)
	{
		tmp[i - measures.first] = scorePitches[i];
	}
	// clear wektora
	scorePitches.clear();
	// wpisaæ na wektor kopiê
	for(int i = 0; i < tmp.size(); i++)
	{
		scorePitches.push_back(tmp[i]);
	}
}

void AudioPitchPage::setScore(std::vector<float>* scorePitchesIn, std::vector<std::pair<int,int>> measures)
{
	if (scorePitches.size() == 0)
	{
		scorePitches = *scorePitchesIn;
	}
	measuresInSamples = measures;
}

float AudioPitchPage::getFirstSound()
{
	return scorePitches[0];
}

class ExercisesButtonListener : public Button::Listener
{
public:
	ExercisesButtonListener(WindowExercises* comp)
	{
		comp_ = comp;
	}
	void buttonClicked(Button* button)
	{
		if (button->getButtonText() == Globals::getInstance()->getTexts()[23])
		{
			if (DialogWindow* dw = comp_->findParentComponentOfClass<DialogWindow>())
					dw->exitModalState(1);
		}
		else 
		{
			if (DialogWindow* dw = comp_->findParentComponentOfClass<DialogWindow>())
					dw->exitModalState(0);
		}
	}
	WindowExercises* comp_;
};

WindowExercises::WindowExercises(std::pair<int,int> measures)
{
	setBounds(0,0,300,100);

	label = new Label();
	String string = Globals::getInstance()->getTexts()[22];
	string.append(String(measures.first), 10);
	string.append(" - ", 10);
	string.append(String(measures.second), 10);
	string.append("?", 10);
	label->setText(string, true);
	addAndMakeVisible(label);
	label->setBounds(50, 20, 200, 30);
	
	ExercisesButtonListener* listener = new ExercisesButtonListener(this);
	buttonYes = new TextButton();
	buttonYes->setButtonText(Globals::getInstance()->getTexts()[23]);
	buttonYes->setBounds(50, 60, 70, 30);
	buttonYes->addListener(listener);
	addAndMakeVisible(buttonYes);
	
	buttonNo = new TextButton();
	buttonNo->setButtonText(Globals::getInstance()->getTexts()[24]);
	buttonNo->setBounds(180, 60, 70, 30);
	buttonNo->addListener(listener);
	addAndMakeVisible(buttonNo);
}

WindowExercises::~WindowExercises()
{
	deleteAndZero(label);
	deleteAndZero(buttonYes);
	deleteAndZero(buttonNo);
}
