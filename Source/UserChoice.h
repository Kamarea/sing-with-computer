#ifndef USER_CHOICE_
#define USER_CHOICE_

#include "component_headers.h"

class UserChoice :	public Component
{
public:
	UserChoice();
	~UserChoice();

private:
	Button* chooseUserButton;
	Button* addUserButton;
	ComboBox* usersListBox;
	TextEditor* newUserField;
	Label* chooseUserLabel;
	Label* addUserLabel;
	
    TimeSliceThread thread;
	DirectoryContentsList usersList;
};

#endif