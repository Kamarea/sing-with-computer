/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  1 May 2011 12:07:35pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include <sstream>

#include "AudioTabComponent.h"
#include "AudioAmplitudePage.h"
#include "AudioSpectrogramPage.h"
#include "AudioPitchPage.h"

//[/MiscUserDefs]

//==============================================================================
AudioTabComponent::AudioTabComponent ()
    : tabbedComponent (0)
{
	//TabbedComponent* comp = &tabbedComp
	tabbedComponent = new TabbedComponent (TabbedButtonBar::TabsAtTop);
	init(tabbedComponent);

    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}
/*AudioTabComponent::AudioTabComponent (Array<ScorePart> score)
    //: tabbedComponent (0)
{
	//init(tabbedComponent);

    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}
*/
void AudioTabComponent::init(TabbedComponent* tabbedComponent)
{
	
	Time actualTime=Time::getCurrentTime();
	std::ostringstream sstream;
	sstream << "data\\" << actualTime.getYear() << "-" << actualTime.getMonth()+1 << "-" <<
		actualTime.getDayOfMonth() << "_" << actualTime.getHours() << "-" <<
		actualTime.getMinutes();
	std::string folderName = sstream.str();
	File directory = File(File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getParentDirectory().getChildFile (folderName.c_str()));
	directory.createDirectory();
	
    addAndMakeVisible (tabbedComponent);
    tabbedComponent->setTabBarDepth (30);
    tabbedComponent->addTab (L"Spektrogram", Colours::lightgrey, new AudioSpectrogramPage(deviceManager,folderName), true);
    tabbedComponent->addTab (L"Amplituda", Colours::lightgrey, new AudioAmplitudePage(deviceManager,directory), true);
    tabbedComponent->addTab (L"Wysokoœæ", Colours::lightgrey, new AudioPitchPage(deviceManager,folderName), true);
    tabbedComponent->setCurrentTabIndex (0);


    //[UserPreSize]
    deviceManager.initialise (2, 2, 0, true, String::empty, 0);
    //[/UserPreSize]

    setSize (getWidth(), getHeight()/2);
	tabbedComponent->setBounds(0,0,getWidth(),getHeight()/2);//getHeight()/2
	
}

AudioTabComponent::~AudioTabComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (tabbedComponent);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AudioTabComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::white);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AudioTabComponent::resized()
{
    tabbedComponent->setBounds (0,0, getWidth(), getHeight()/2);//getHeight()/2
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//==============================================================================
Component* createAudio()
{
    return new AudioTabComponent();
}/*
Component* createAudio(Array<ScorePart> score)
{
    return new AudioTabComponent(score);
}*/

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AudioDemoTabComponent" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330000013"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ffffffff"/>
  <TABBEDCOMPONENT name="new tabbed component" id="83c980d7793cdced" memberName="tabbedComponent"
                   virtualName="" explicitFocusOrder="0" pos="0 0 0M 0M" orientation="top"
                   tabBarDepth="30" initialTab="0">
    <TAB name="Audio Device Setup" colour="ffd3d3d3" useJucerComp="1"
         contentClassName="" constructorParams="deviceManager" jucerComponentFile="AudioDemoSetupPage.cpp"/>
    <TAB name="File Playback" colour="ffd3d3d3" useJucerComp="1" contentClassName=""
         constructorParams="deviceManager" jucerComponentFile="AudioDemoPlaybackPage.cpp"/>
    <TAB name="Synth Playback" colour="ffd3d3d3" useJucerComp="1" contentClassName=""
         constructorParams="deviceManager" jucerComponentFile="AudioDemoSynthPage.cpp"/>
    <TAB name="Latency Test" colour="ffd3d3d3" useJucerComp="1" contentClassName=""
         constructorParams="deviceManager" jucerComponentFile="AudioDemoLatencyPage.cpp"/>
    <TAB name="Recording" colour="ffd3d3d3" useJucerComp="1" contentClassName=""
         constructorParams="deviceManager" jucerComponentFile="AudioDemoRecordPage.cpp"/>
  </TABBEDCOMPONENT>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
