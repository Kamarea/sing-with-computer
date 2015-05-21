/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic outline for a simple desktop window.

  ==============================================================================
*/

#ifndef __MAINWINDOW_H_3DF5D75A__
#define __MAINWINDOW_H_3DF5D75A__

//#include "../JuceLibraryCode/JuceHeader.h"
#include "UserChoice.h"


//==============================================================================
class MainAppWindow   : public DocumentWindow
{
public:
    //==============================================================================
    MainAppWindow();
    ~MainAppWindow();

    void closeButtonPressed();

    // the command manager object used to dispatch command events
    ApplicationCommandManager commandManager;

private:
    //ScopedPointer<Component> taskbarIcon;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainAppWindow)
};


#endif  // __MAINWINDOW_H_3DF5D75A__
