#include "Menu.h"

#include "ActionPreferences.h"
#include "NodeEnums.h"

bool Menu::Confirm()
{
	char Input;
	std::cin >> Input;

	if (Input == 'Y' || Input == 'y')
		return true;
	if (Input == 'N' || Input == 'n')
		return false;

	std::cout << "Incorrect input" << std::endl << std::endl;
	return Confirm();
}

void Menu::PrintPreferences(const ActionPreferences& Preferences)
{
	std::cout << "\nYour preferences:\n\n";
	std::cout << "Chance to generate event: " << Preferences.GetPreference(NodeActions::GenerateEvent) << std::endl;
	std::cout << "Chance to subscribe to random neighbor of neighbor's neighbor: " << Preferences.GetPreference(NodeActions::SubscribeToNeighbor) << std::endl;
	std::cout << "Chance to unsubscribe from random neighbor: " << Preferences.GetPreference(NodeActions::UnsubscribeFromNeighbor) << std::endl;
	std::cout << "Chance to create new neighbor node: " << Preferences.GetPreference(NodeActions::GenerateNewNeighbor) << std::endl;
	std::cout << "Chance to skip update: " << Preferences.GetPreference(NodeActions::Sleep) << std::endl << std::endl;
}
