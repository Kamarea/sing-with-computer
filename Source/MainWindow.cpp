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
Component* createSynchronization();
Component* createClassification();

class ContentComp  : public Component,
                     public MenuBarModel,
                     public ApplicationCommandTarget
{
public:
    ContentComp (MainAppWindow& mainWindow_)
        : mainWindow (mainWindow_),
          currentDemoId (0)
    {
        setOpaque (true);
		userChoice = new UserChoice();
		addAndMakeVisible(userChoice);
		DialogWindow::showModalDialog(Globals::getInstance()->getTexts()[1], userChoice, this, Colours::white, false);
    }

    ~ContentComp()
    {
		deleteAndZero(userChoice);
    }

    void paint (Graphics& g)
    {
        g.fillAll (Colours::white);
    }
	
	void showDemo (Component* comp)
    {
        currentComp = comp;
        addAndMakeVisible (currentComp);
		
        currentComp->setBounds ("0, 0, parent.width, parent.height");
    }

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
			menu.addCommandItem(commandManager, synchronize);
            menu.addCommandItem (commandManager, showAudioReader);
            menu.addCommandItem (commandManager, classify);
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

	ApplicationCommandTarget* getNextCommandTarget()
    {
        return findFirstTargetParentComponent();
    }

	 void getAllCommands (Array <CommandID>& commands)
    {
        const CommandID ids[] = { 
			loadFile, 
			showAudio,  
			quit,
			getNoise,
			synchronize,
			showAudioReader,
			classify
        };

        commands.addArray (ids, numElementsInArray (ids));
    }

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
            result.addDefaultKeypress ('q', ModifierKeys::commandModifier);
			break;
		case getNoise:
			result.setInfo(Globals::getInstance()->getTexts()[13], "Measures noise", demosCategory, 0);
            result.setTicked (currentDemoId == getNoise);
            result.addDefaultKeypress ('n', ModifierKeys::commandModifier);
			break;
		case synchronize:
			result.setInfo(Globals::getInstance()->getTexts()[25], "Synchronize", demosCategory, 0);
            result.setTicked (currentDemoId == getNoise);
            result.addDefaultKeypress ('s', ModifierKeys::commandModifier);
			break;
		case classify:
			result.setInfo(Globals::getInstance()->getTexts()[32], "Classify", demosCategory, 0);
            result.setTicked (currentDemoId == getNoise);
            result.addDefaultKeypress ('c', ModifierKeys::commandModifier);
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
			
        case synchronize:
			showDemo(createSynchronization());
		    currentDemoId = synchronize;
			break;

		case classify:
			showDemo(createClassification());
		    currentDemoId = classify;
			break;

        default:
            return false;
        };

        return true;
	}

private:
    MainAppWindow& mainWindow;
    OldSchoolLookAndFeel oldLookAndFeel;
    ScopedPointer<Component> currentComp;
    int currentDemoId;
	TextButton* testButton;
	UserChoice* userChoice;

    StringArray getRenderingEngines()
    {
        StringArray renderingEngines (getPeer()->getAvailableRenderingEngines());

       #if JUCE_OPENGL
        renderingEngines.add ("Use OpenGL Renderer");
       #endif

        return renderingEngines;
    }

    enum CommandIDs
    {
		showAudio					= 0x2005,
		loadFile					= 0x2006,
		showAudioReader				= 0x2007,
		quit						= 0x2008,
		getNoise					= 0x2009,
		synchronize					= 0x2010,
		classify					= 0x2011
    };


};

MainAppWindow::MainAppWindow()
    : DocumentWindow (Globals::getInstance()->getTexts()[0],
                      Colours::azure,
                      DocumentWindow::allButtons,true)
{
	setResizable (true, false);
    setResizeLimits (400, 300, 8192, 8192);

	ContentComp* contentComp = new ContentComp (*this);

    commandManager.registerAllCommandsForTarget (contentComp);
    commandManager.registerAllCommandsForTarget (JUCEApplication::getInstance());

    addKeyListener (commandManager.getKeyMappings());

    setContentOwned (contentComp, false);

    setMenuBar (contentComp);

    contentComp->setApplicationCommandManagerToWatch (&commandManager);

    setVisible (true);
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
