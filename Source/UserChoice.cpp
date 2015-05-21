#include "UserChoice.h"

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

	chooseUserLabel->setBounds(220,20,100,20);
	usersListBox->setBounds(180,60,100,20);

}

UserChoice::~UserChoice()
{
	deleteAndZero(chooseUserLabel);
	deleteAndZero(usersListBox);
}