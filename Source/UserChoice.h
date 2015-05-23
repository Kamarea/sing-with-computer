#ifndef USER_CHOICE_
#define USER_CHOICE_

#include "component_headers.h"

class UserChoice :	public Component
{
public:
	UserChoice();
	~UserChoice();

	ComboBox* usersListBox;
	std::vector<String> userNames;
	TextEditor* newUserField;

private:
	TextButton* chooseUserButton;
	TextButton* addUserButton;
	Label* chooseUserLabel;
	Label* addUserLabel;
	
    TimeSliceThread thread;
	DirectoryContentsList usersList;
};


class OkWind : public Component
{
public:
	OkWind();
	~OkWind();
	Label* label;
	TextButton* button;
};

#endif