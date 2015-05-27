/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic outline for a simple desktop window.

  ==============================================================================
*/

#include "component_headers.h"
#include "MainWindow.h"
#include "components\logs.h"
#include "components\ChooseAndLoadFile.h"

Component* createAudio();
Component* chooseAndLoadFileComponent();
ChooseAndLoadFile* chooseAndLoadFile();
Component* createAudioReader();
Component* createAudio(Array<ScorePart> score);
Component* createNoiseMeasurement();

/*TextEditor* textField;


void audioLog(const char* fmt,...){
	va_list args;
	va_start(args,fmt);
	char text[256];
	vsprintf(text,fmt,args);
	textField->insertTextAtCaret(text);
	va_end(args);
}*/

//==============================================================================
class ContentComp  : public Component,
                     public MenuBarModel,
                     public ApplicationCommandTarget
{
public:
    //==============================================================================
    ContentComp (MainAppWindow& mainWindow_)
        : mainWindow (mainWindow_),
          currentDemoId (0)
    {
        setOpaque (true);
		userChoice = new UserChoice();
		addAndMakeVisible(userChoice);
		//userChoice->setBounds(0,0,500,200);
		DialogWindow::showModalDialog(Globals::getInstance()->getTexts()[1], userChoice, this, Colours::white, false);
        //invokeDirectly (showAudioReader, true);
    }

    ~ContentComp()
    {
       //#if JUCE_OPENGL
       // openGLContext.detach();
       //#endif

		deleteAndZero(userChoice);
    }

    void paint (Graphics& g)
    {
        g.fillAll (Colours::white);
    }
	
    //==============================================================================
	void showDemo (Component* comp)
    {
        currentComp = comp;
        addAndMakeVisible (currentComp);

		/*textField = new TextEditor(L"logs");
		currentComp->addAndMakeVisible(textField);
		textField->setReadOnly(true);
		textField->setCaretVisible(false);
		textField->setMultiLine(true);
		textField->setText(L"Logs:\r\n");
		textField->moveCaretToEnd();
		textField->setBounds(10,30,500,200);*/
		
        currentComp->setBounds ("0, 0, parent.width, parent.height");
    }

    //==============================================================================
    StringArray getMenuBarNames()
    {
        const char* const names[] = { 
			Globals::getInstance()->getTexts()[7].getCharPointer(), 
			Globals::getInstance()->getTexts()[8].getCharPointer(), 
			nullptr };

        return StringArray (names);
    }

	PopupMenu getMenuForIndex (int menuIndex, const String&)
    {
		ApplicationCommandManager* commandManager = &(mainWindow.commandManager);

		PopupMenu menu;

		if (menuIndex == 0)
        {            
			menu.addCommandItem(commandManager, loadFile);
            menu.addCommandItem (commandManager, showAudio);

            menu.addSeparator();
            menu.addCommandItem (commandManager, quit);
        }
		else if (menuIndex == 1)
		{
			menu.addCommandItem(commandManager, getNoise);
            menu.addCommandItem (commandManager, showAudioReader);
		}

        return menu;
    }

	void menuItemSelected (int menuItemID, int)
    {
		if (menuItemID >= 5001 && menuItemID < 5010)
        {
            const int engineIndex = menuItemID - 5001;

            getPeer()->setCurrentRenderingEngine (engineIndex);
        }
	}

	//==============================================================================
    // The following methods implement the ApplicationCommandTarget interface, allowing
    // this window to publish a set of actions it can perform, and which can be mapped
    // onto menus, keypresses, etc.

	ApplicationCommandTarget* getNextCommandTarget()
    {
        // this will return the next parent component that is an ApplicationCommandTarget (in this
        // case, there probably isn't one, but it's best to use this method in your own apps).
        return findFirstTargetParentComponent();
    }

	 void getAllCommands (Array <CommandID>& commands)
    {
        // this returns the set of all commands that this target can perform..
        const CommandID ids[] = { 
			loadFile, 
			showAudio,  
			quit,
			getNoise,
			showAudioReader
        };

        commands.addArray (ids, numElementsInArray (ids));
    }

	 // This method is used when something needs to find out the details about one of the commands
    // that this object can perform..
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
    {
        const String generalCategory ("General");
        const String demosCategory ("Demos");

        switch (commandID)
        {
        case showAudio:
            result.setInfo (Globals::getInstance()->getTexts()[10], "Shows the audio demo", demosCategory, 0);
            result.setTicked (currentDemoId == showAudio);
            result.addDefaultKeypress ('a', ModifierKeys::commandModifier);
            break;
		case loadFile:
			result.setInfo(Globals::getInstance()->getTexts()[9], "Loads music file", demosCategory, 0);
            result.setTicked (currentDemoId == loadFile);
            result.addDefaultKeypress ('l', ModifierKeys::commandModifier);
			break;
		case showAudioReader:
			result.setInfo(Globals::getInstance()->getTexts()[12], "Plays recorded pieces", demosCategory, 0);
            result.setTicked (currentDemoId == showAudioReader);
            result.addDefaultKeypress ('r', ModifierKeys::commandModifier);
			break;
		case quit:
			result.setInfo(Globals::getInstance()->getTexts()[11], "Quit", demosCategory, 0);
            result.setTicked (currentDemoId == quit);
            result.addDefaultKeypress ('Q', ModifierKeys::commandModifier);
			break;
		case getNoise:
			result.setInfo(Globals::getInstance()->getTexts()[13], "Quit", demosCategory, 0);
            result.setTicked (currentDemoId == getNoise);
            result.addDefaultKeypress ('n', ModifierKeys::commandModifier);
			break;

        default:
            break;
        };
    }

	bool perform (const InvocationInfo& info)
    {

		switch (info.commandID)
        {

        case showAudio:
			if (currentDemoId == loadFile)
			{
				Component* currentComp = findChildWithID("chooseAndLoadFileID");
				showDemo (createAudio(((ChooseAndLoadFile*)currentComp)->parts));
			}
			else
				showDemo (createAudio());
            currentDemoId = showAudio;
            break;
			
        case loadFile:
			showDemo(chooseAndLoadFileComponent());
		    currentDemoId = loadFile;
			break;

        case showAudioReader:
			showDemo(createAudioReader());
		    currentDemoId = showAudioReader;
			break;

        case getNoise:
			showDemo(createNoiseMeasurement());
		    currentDemoId = getNoise;
			break;

        case quit:
			mainWindow.userTriedToCloseWindow();
			break;

        default:
            return false;
        };

        return true;
	}

private:
    //==============================================================================
    MainAppWindow& mainWindow;
    OldSchoolLookAndFeel oldLookAndFeel;
    ScopedPointer<Component> currentComp;
    int currentDemoId;
	TextButton* testButton;
	UserChoice* userChoice;

	//==============================================================================
    StringArray getRenderingEngines()
    {
        StringArray renderingEngines (getPeer()->getAvailableRenderingEngines());

       #if JUCE_OPENGL
        renderingEngines.add ("Use OpenGL Renderer");
       #endif

        return renderingEngines;
    }

	//==============================================================================
    enum CommandIDs
    {
		showAudio					= 0x2005,
		loadFile					= 0x2006,
		showAudioReader				= 0x2007,
		quit						= 0x2008,
		getNoise					= 0x2009
    };


};

//==============================================================================
MainAppWindow::MainAppWindow()
    : DocumentWindow (Globals::getInstance()->getTexts()[0],//JUCEApplication::getInstance()->getApplicationName(),
                      Colours::azure,
                      DocumentWindow::allButtons,true)
{
    //centreWithSize (500, 400);
    //setVisible (true);
	setResizable (true, false); // resizability is a property of ResizableWindow
    setResizeLimits (400, 300, 8192, 8192);

	ContentComp* contentComp = new ContentComp (*this);

    commandManager.registerAllCommandsForTarget (contentComp);
    commandManager.registerAllCommandsForTarget (JUCEApplication::getInstance());

    // this lets the command manager use keypresses that arrive in our window to send
    // out commands
    addKeyListener (commandManager.getKeyMappings());

    // sets the main content component for the window to be this tabbed
    // panel. This will be deleted when the window is deleted.
    setContentOwned (contentComp, false);

    // this tells the DocumentWindow to automatically create and manage a MenuBarComponent
    // which uses our contentComp as its MenuBarModel
    setMenuBar (contentComp);

    // tells our menu bar model that it should watch this command manager for
    // changes, and send change messages accordingly.
    contentComp->setApplicationCommandManagerToWatch (&commandManager);

    setVisible (true);

	//taskbarIcon = new DemoTaskbarComponent();
}

MainAppWindow::~MainAppWindow()
{
	setMenuBar (nullptr);
	Globals::getInstance()->resaveFile();
    clearContentComponent();
}

void MainAppWindow::closeButtonPressed()
{
    JUCEApplication::getInstance()->systemRequestedQuit();
}
