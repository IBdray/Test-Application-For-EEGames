#pragma once
#include <iostream>


struct ActionPreferences;


class Menu
{

public:
	static bool Confirm();
	static void PrintPreferences(const ActionPreferences& Preferences);
};

