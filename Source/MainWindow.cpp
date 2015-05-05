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
        //invokeDirectly (showAudioReader, true);
    }

    ~ContentComp()
    {
       //#if JUCE_OPENGL
       // openGLContext.detach();
       //#endif

		//deleteAndZero(textField);
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
        const char* const names[] = { "Demo", nullptr };

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
            menu.addCommandItem (commandManager, showAudioReader);

            menu.addSeparator();
            menu.addCommandItem (commandManager, StandardApplicationCommandIDs::quit);
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
        const CommandID ids[] = { showAudio,loadFile,showAudioReader
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
            result.setInfo ("Audio", "Shows the audio demo", demosCategory, 0);
            result.setTicked (currentDemoId == showAudio);
            result.addDefaultKeypress ('1', ModifierKeys::commandModifier);
            break;
		case loadFile:
			result.setInfo("Load file", "Loads music file", demosCategory, 0);
            result.setTicked (currentDemoId == loadFile);
            result.addDefaultKeypress ('2', ModifierKeys::commandModifier);
			break;
		case showAudioReader:
			result.setInfo("AudioReader", "Plays recorded pieces", demosCategory, 0);
            result.setTicked (currentDemoId == showAudioReader);
            result.addDefaultKeypress ('3', ModifierKeys::commandModifier);
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
				//Array<ScorePart> score = ((ChooseAndLoadFile*)currentComp)->parts;
				//Component* temp = createAudio(score);
				//showDemo (temp);
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
		showAudio                  = 0x2005,
		loadFile				   = 0x2006,
		showAudioReader			   = 0x2007
    };


};

//==============================================================================
MainAppWindow::MainAppWindow()
    : DocumentWindow ("Magisterka!",//JUCEApplication::getInstance()->getApplicationName(),
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
    clearContentComponent();
}

void MainAppWindow::closeButtonPressed()
{
    JUCEApplication::getInstance()->systemRequestedQuit();
}
