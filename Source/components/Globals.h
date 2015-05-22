#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "../component_headers.h"

const float log_10_2=0.30102999566398119521373889472449;

const float phi=0.95;

class Globals
{
public:
	static Globals* getInstance()
	{
		if (instance == NULL)
		{
			instance = new Globals();
		}
		return instance;
	};
	~Globals();

	String getUser();

private:
	static Globals* instance;
	String currentUser;

	std::vector<std::pair<String, String>> settings;

	void parseLine(String line);

	Globals();
    Globals (const Globals&);
    Globals& operator = (const Globals&);
};

#endif // GLOBALS_H