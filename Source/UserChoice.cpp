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
		if (button->getButtonText() == L"Choose")
		{
			// choose button clicked
		}
		else if (button->getButtonText() == L"Add")
		{
			// add button clicked
		}
	}
	UserChoice* component_;
};

UserChoice::UserChoice()
	:	thread ("reading data from files"),
		usersList(0, thread)
{
	chooseUserLabel = new Label(String::empty, L"Choose user:");
	addAndMakeVisible(chooseUserLabel);

	File directory = File(File::getSpecialLocation(File::currentExecutableFile).
		getParentDirectory().getParentDirectory().getChildFile("data"));
	usersList.setDirectory(directory, true, false);
    thread.startThread (4);
	while (usersList.isStillLoading())
	{
	}
	int dirNumber = usersList.getNumFiles();

	std::vector<String> userNames;
	DirectoryContentsList::FileInfo info;
	for (int i = 0; i < dirNumber; i++)
	{
		usersList.getFileInfo(i, info);
		if (info.isDirectory)
			userNames.push_back(info.filename);
	}

	usersListBox = new ComboBox();
	usersListBox->setEditableText(false);
	for (int i = 0; i < userNames.size(); i++)
	{
		usersListBox->addItem(userNames[i],i + 1);
	}
	addAndMakeVisible(usersListBox);

	Globals* globals = Globals::getInstance();
	for (int i = 0; i < userNames.size(); i++)
		if (userNames[i] == globals->getUser())
		{
			usersListBox->setSelectedId(i+1);
		}

	UserButtonListener* listener = new UserButtonListener(this);

	chooseUserButton = new TextButton();
	chooseUserButton->setButtonText("Choose");
	chooseUserButton->addListener(listener);
	addAndMakeVisible(chooseUserButton);

	addUserLabel = new Label(String::empty, L"or add a new user:");
	addAndMakeVisible(addUserLabel);

	newUserField = new TextEditor();
	addAndMakeVisible(newUserField);

	addUserButton = new TextButton();
	addUserButton->setButtonText("Add");
	addUserButton->addListener(listener);
	addAndMakeVisible(addUserButton);

	chooseUserLabel->setBounds(220,20,100,20);
	usersListBox->setBounds(180,60,100,20);
	chooseUserButton->setBounds(300, 60, 50, 20);
	addUserLabel->setBounds(200,100,150,20);
	newUserField->setBounds(180,140,100,20);
	addUserButton->setBounds(300, 140, 50, 20);
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