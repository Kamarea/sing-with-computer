#include "UserChoice.h"

class UserButtonListener : public Button::Listener
{
public:
	UserButtonListener(UserChoice* component)
	{
		component_ = component;
	}
	void buttonClicked(Button* button)
	{
		if (button->getButtonText() == Globals::getInstance()->getTexts()[3])
		{
			// choose button clicked
			String currUser = component_->userNames[component_->usersListBox->getSelectedId() - 1];
			Globals::getInstance()->setUser(currUser);
			if (DialogWindow* dw = component_->findParentComponentOfClass<DialogWindow>())
				dw->exitModalState(1234);
		}
		else if (button->getButtonText() == Globals::getInstance()->getTexts()[5])
		{
			// add button clicked
			String user = component_->newUserField->getText();
			bool exists = false;
			for (int i = 0; i < component_->userNames.size(); i++)
			{
				if (component_->userNames[i] == user)
				{
					exists = true;
				}
			}
			if(exists == true)
			{
				OkWind* okWind = new OkWind();
				component_->addAndMakeVisible(okWind);
				DialogWindow::showModalDialog(Globals::getInstance()->getTexts()[6], okWind, component_, Colours::white, true);
				deleteAndZero(okWind);
			}
			else
			{
				File parentDirectory = File(File::getSpecialLocation(File::currentExecutableFile).
					getParentDirectory().getParentDirectory().getChildFile ("data"));
				File dir = parentDirectory.getChildFile(user);
				dir.createDirectory();

				Globals::getInstance()->setUser(user);
				if (DialogWindow* dw = component_->findParentComponentOfClass<DialogWindow>())
					dw->exitModalState(1234);
			}
		}
	}
	UserChoice* component_;
};

class OkButtonListener : public Button::Listener
{
public:
	OkButtonListener(OkWind* comp)
	{
		comp_ = comp;
	}
	void buttonClicked(Button* button)
	{
		if (DialogWindow* dw = comp_->findParentComponentOfClass<DialogWindow>())
				dw->exitModalState(1234);
	}
	OkWind* comp_;
};

OkWind::OkWind()
{
	label = new Label(String::empty, Globals::getInstance()->getTexts()[6]);
	button = new TextButton();
	button->setButtonText("OK");
	OkButtonListener* list = new OkButtonListener(this);
	button->addListener(list);
	addAndMakeVisible(label);
	addAndMakeVisible(button);
	setBounds(0,0,300,100);
	label->setBounds((getWidth() - 150) / 2,20,150,20);
	button->setBounds((getWidth() - 50) / 2,60,50,20);
}

OkWind::~OkWind()
{
	deleteAndZero(label);
	deleteAndZero(button);
}

UserChoice::UserChoice()
	:	thread ("reading data from files"),
		usersList(0, thread)
{
	chooseUserLabel = new Label(String::empty, Globals::getInstance()->getTexts()[2]);
	addAndMakeVisible(chooseUserLabel);

	File directory = File(File::getSpecialLocation(File::currentExecutableFile).
		getParentDirectory().getParentDirectory().getChildFile("data"));
	usersList.setDirectory(directory, true, false);
    thread.startThread (4);
	while (usersList.isStillLoading())
	{
	}
	int dirNumber = usersList.getNumFiles();

	DirectoryContentsList::FileInfo info;
	for (int i = 0; i < dirNumber; i++)
	{
		usersList.getFileInfo(i, info);
		if (info.isDirectory)
			userNames.push_back(info.filename);
	}
	setBounds(0,0,500,200);

	usersListBox = new ComboBox();
	usersListBox->setEditableText(false);
	for (int i = 0; i < userNames.size(); i++)
	{
		usersListBox->addItem(userNames[i],i + 1);
	}
	addAndMakeVisible(usersListBox);

	for (int i = 0; i < userNames.size(); i++)
		if (userNames[i] == Globals::getInstance()->getUser())
		{
			usersListBox->setSelectedId(i+1);
		}

	UserButtonListener* listener = new UserButtonListener(this);

	chooseUserButton = new TextButton();
	chooseUserButton->setButtonText(Globals::getInstance()->getTexts()[3]);
	chooseUserButton->addListener(listener);
	addAndMakeVisible(chooseUserButton);

	addUserLabel = new Label(String::empty, Globals::getInstance()->getTexts()[4]);
	addAndMakeVisible(addUserLabel);

	newUserField = new TextEditor();
	addAndMakeVisible(newUserField);

	addUserButton = new TextButton();
	addUserButton->setButtonText(Globals::getInstance()->getTexts()[5]);
	addUserButton->addListener(listener);
	addAndMakeVisible(addUserButton);

	chooseUserLabel->setBounds((getWidth() - 150) / 2,20,150,20);
	usersListBox->setBounds((getWidth() - 230) / 2,60,150,20);
	chooseUserButton->setBounds(getWidth() - (getWidth() - 230) / 2 - 70, 60, 70, 20);
	addUserLabel->setBounds((getWidth() - 150) / 2,100,150,20);
	newUserField->setBounds((getWidth() - 220) / 2,140,140,20);
	addUserButton->setBounds(getWidth() - (getWidth() - 220) / 2 - 70, 140, 70, 20);
}

UserChoice::~UserChoice()
{
	deleteAndZero(chooseUserLabel);
	deleteAndZero(usersListBox);
	deleteAndZero(chooseUserButton);
	deleteAndZero(addUserLabel);
	deleteAndZero(newUserField);
	deleteAndZero(addUserButton);
}