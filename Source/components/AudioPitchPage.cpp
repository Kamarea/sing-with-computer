#include "AudioPitchPage.h"
//#include "logs.h"

LiveAudioPitchDisplayComp* LiveAudioPitchDisplayComp::instance;
/*	
static Array<float> getPitches(LiveAudioPitchDisplayComp* loader)
{
	return loader->pitches;
}
*/
//[MiscUserDefs] You can add your own user definitions and misc code here...
LiveAudioPitchDisplayComp::LiveAudioPitchDisplayComp()
{
    nextSample = subSample = 0;
    accumulator = 0;
    zeromem (samples, sizeof (samples));
    setOpaque (true);
	test=0;
	frequency=-2;
	pitches=Array<float>();

    startTimer (1000 / 50); // use a timer to keep repainting this component
}

LiveAudioPitchDisplayComp::~LiveAudioPitchDisplayComp()
{
}

void LiveAudioPitchDisplayComp::paint (Graphics& g)
{
    g.fillAll (Colours::white);

    g.setColour (Colours::blue);
	
	//("frequency=%f\r\n",pitch[2046]);
	//audioLog("frequency=%f\r\n",pitch[2047]);
	//audioLog("Samples[0-10]=%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\r\n",samples[0],samples[1],samples[2],
	//	samples[3],samples[4],samples[5],samples[6],samples[7],samples[8],samples[9],samples[10]);
	

    const float midY = getHeight() * 0.5f;
    int sampleNum = (nextSample + numElementsInArray (samples) - 1);
	float oneHzSize = (float)getHeight() / 1800.0f;

	const float log_10_2 = 0.30102999566398119521373889472449;
	for(int i=0;i<2047;++i)
	{
		pitchMIDI[i]=pitchMIDI[i+1];
	}

	// MIDI
	pitchMIDI[2047]=69+12*(log10(pitch[2047]/440)/log_10_2);
	pitchMIDI[2047]-=33;

	pitches.add(pitchMIDI[2047]);
	
	/*audioLog("log = %f\r\n",pitchMIDI[2046]);
	audioLog("log = %f\r\n",pitchMIDI[2047]);
	audioLog("width = %d\r\n",getWidth());
	audioLog("height = %d\r\n",getHeight());
	*/
	String pitchNames[] = {"1A","1AIS","1B",
				"C","CIS","D","DIS","E","F","FIS","G","GIS","A","AIS","B",
				"c","cis","d","dis","e","f","fis","g","gis","a","ais","b",
				"c1","cis1","d1","dis1","e1","f1","fis1","g1","gis1","a1","ais1","b1",
				"c2","cis2","d2","dis2","e2","f2","fis2","g2","gis2","a2","ais2","b2",
				"c3","cis3","d3","dis3","e3","f3","fis3","g3","gis3","a3","ais3","b3"};
	int pitchCount = 51;
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

	int offset = 2048 - getWidth() + 1;
	//audioLog("last midi = %d\r\n",pitchMIDI[getWidth()+offset]);
	//audioLog("init x = %d\r\n",getWidth()+offset);

	g.setColour(Colours::blue);
    for (int x = 2048 - getWidth() + 20; x < 2048; ++x)
    {
        int y=(float)getHeight() - pitchMIDI[x] * semitone;

        g.drawVerticalLine (x-2048+getWidth() - 20, y, y+1);
    }
}

void LiveAudioPitchDisplayComp::timerCallback()
{
    repaint();
}

void LiveAudioPitchDisplayComp::audioDeviceAboutToStart (AudioIODevice*)
{
    zeromem (samples, sizeof (samples));
}

void LiveAudioPitchDisplayComp::audioDeviceStopped()
{
    zeromem (samples, sizeof (samples));
}

void LiveAudioPitchDisplayComp::audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                                                       float** outputChannelData, int numOutputChannels, int numSamples)
{
	//numSamples=2560 = 512*5
	// u¿yjemy sobie tablicy 3*1024, bêdziemy liczyæ dla pierwszej 1024, drugiej, 
	// potem przesuwamy o 2048 i dopisujemy nowe próbki na koniec <- Ÿle!!!
	// 
    for(int i = 0; i < 2048; i+=1)//++i)
    {
        for (int chan = 0; chan < numInputChannels; ++chan)
        {
            if (inputChannelData[chan] != 0)
                accumulator += fabsf (inputChannelData[chan][i]);
        }

        const int numSubSamples = 1; // how many input samples go onto one pixel.
        const float boost = 1.0f;     // how much to boost the levels to make it more visible.

        if (subSample == 0)
        {
            samples[nextSample] = accumulator * boost / numSubSamples;
			//if(samples[nextSample]>2) samples[nextSample]=2;
			//samples[nextSample]-=1;
            nextSample = (nextSample + 1) % numElementsInArray (samples);
            subSample = numSubSamples;
            accumulator = 0;
        }
        else
        {
            --subSample;
        }
    }

	frequency = computePitch(samples);
	frequency /= 1.32f;
	
	for(int i=1;i<2048;i++)
		pitch[i-1]=pitch[i] < 0 ? 0 : pitch[i];
	pitch[2047]=frequency;
	if(pitch[2045]>0 & pitch[2047]>0 & pitch[2046]<=0)
		pitch[2046]=(pitch[2045]+pitch[2047])/2;

	frequency = computePitch(samples+1024);
	frequency /= 1.32f;
	
	for(int i=1;i<2048;i++)
		pitch[i-1]=pitch[i] < 0 ? 0 : pitch[i];
	pitch[2047]=frequency;
	if(pitch[2045]>0 & pitch[2047]>0 & pitch[2046]<=0)
		pitch[2046]=(pitch[2045]+pitch[2047])/2;

    // We need to clear the output buffers, in case they're full of junk..
    for (int i = 0; i < numOutputChannels; ++i)
        if (outputChannelData[i] != 0)
            zeromem (outputChannelData[i], sizeof (float) * numSamples);
}

float LiveAudioPitchDisplayComp::computePitch(float *my_samples){
	float freq=0.0f;				// freq to return
	int lev=6;						// levels of analysis
	float globalMaxTreshold=0.55f;	// threshold of maximum values to consider
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

float LiveAudioPitchDisplayComp::abs(float a){
	if(a>=0)
		return a;
	else
		return-a;
}

int LiveAudioPitchDisplayComp::power(int a, int b){
	if(b>0)
		return a*power(a,b-1);
	else
		return 1;
}

float LiveAudioPitchDisplayComp::mean(float a[]){
	float sum=0;
	for(int k=0;k<1024;k++) sum+=a[k];
	return sum/1024;
}

float LiveAudioPitchDisplayComp::max(float table[]){
	float max=table[0];
	for(int i=1;i<1024;i++)
		if(table[i]>max) max=table[i];
	return max;
}

float LiveAudioPitchDisplayComp::min(float table[]){
	float min=table[0];
	for(int i=1;i<1024;i++)
		if(table[i]<min) min=table[i];
	return min;
}

AudioPitchPage::AudioPitchPage (AudioDeviceManager& deviceManager_)
    : deviceManager (deviceManager_),
      liveAudioPDisplayComp (0)
{
    addAndMakeVisible (liveAudioPDisplayComp = LiveAudioPitchDisplayComp::getInstance());

    //[UserPreSize]
    //[/UserPreSize]

    //[Constructor] You can add your own custom stuff here..
    deviceManager.addAudioCallback (liveAudioPDisplayComp);
	//XmlElement *xml=deviceManager.createStateXml();
	//const String path("D:\\xml.txt");
	//FILE *file;
	//file=fopen("D:\\xml.txt","w");
	//fprintf(file,"%s",xml->getText());
	//fclose(file);
    //[/Constructor]
}

AudioPitchPage::~AudioPitchPage()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    deviceManager.removeAudioCallback (liveAudioPDisplayComp);
    //[/Destructor_pre]

    deleteAndZero (liveAudioPDisplayComp);

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AudioPitchPage::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::lightgrey);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AudioPitchPage::resized()
{
    liveAudioPDisplayComp->setBounds (5, 5, getWidth() - 10, getHeight()-10);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

