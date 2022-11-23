// Copyright 2022 Ivan Babanov, IB Dray. No Rights Reserved.

#include <iostream>
#include <thread>
#include <chrono>

#include <Windows.h>

#include "../public/Node.h"
#include "../public/NodeEnums.h"
#include "../public/NodeManager.h"
#include "../public/ActionPreferences.h"
#include "../public/RandomGenerator.h"
#include "../public/Menu.h"


ActionPreferences SetupPreferences();
ActionPreferences SetupEqualPreferences();
ActionPreferences SetupDetailedPreferences();

void GenerateNodeNet(const int Size, const bool Random)
{
	const int FirstLineSize = Random ? RandomGenerator::GenerateNumber(0, Size / 4) : Size / 2;
	const int SecondLineSize = Random ? RandomGenerator::GenerateNumber(0, Size / 8) : 1;
	const int ThirdLineSize = Random ? RandomGenerator::GenerateNumber(0, Size / 16) : 0;

	for (int i = 0; i < FirstLineSize; ++i)
    {
	    const auto FirstLine = Node::Factory::CreateNode();
		for (int j = 0; j < SecondLineSize; ++j)
		{
			const auto SecondLine = Node::Factory::CreateNode();
			SecondLine->Subscribe(FirstLine);
			for (int k = 0; k < ThirdLineSize; ++k)
			{
				const auto ThirdLine = Node::Factory::CreateNode();
				ThirdLine->Subscribe(SecondLine);
			}
		}
    }
}

void Cycle()
{
	int CycleCounter = 0;

	while (!GetAsyncKeyState(VK_ESCAPE))
	{
		NodeManager::UpdateNodes();

		std::cout << "\n ======= Cycle " << ++CycleCounter << " =======\n"
				  << "Number of remaining nodes: " <<  NodeManager::GetNodes().size() << std::endl << std::endl;

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

ActionPreferences SetupPreferences()
{
	std::cout << "You can make all action equally possible or enter specific chance to each." << std::endl;
	std::cout << "Make all actions equally possible? [Y/N]" << std::endl;

	return Menu::Confirm() ? SetupEqualPreferences() : SetupDetailedPreferences();
}

ActionPreferences SetupEqualPreferences()
{
	std::cout << "\n=====================================" << std::endl;
	std::cout << "Enter preferences for all actions: ";

	int Val;
	std::cin >> Val;

	const auto Preferences = ActionPreferences{Val};

	Menu::PrintPreferences(Preferences);
	std::cout << "Confirm [Y/N]" << std::endl;
	return Menu::Confirm() ? Preferences : SetupEqualPreferences();
}

ActionPreferences SetupDetailedPreferences()
{
	ActionPreferences Preferences {0};
	int Val;

	std::cout << "\n=====================================" << std::endl;
	std::cout << "Enter action preferences:\n\n";

	std::cout << "Chance to generate event: ";
	std::cin >> Val;
	Preferences.SetPreference(NodeActions::GenerateEvent, Val);
	
	std::cout << "Chance to subscribe to random neighbor of neighbor's neighbor: ";
	std::cin >> Val;
	Preferences.SetPreference(NodeActions::SubscribeNeighbor, Val);

	std::cout << "Chance to unsubscribe from random neighbor: ";
	std::cin >> Val;
	Preferences.SetPreference(NodeActions::UnsubscribeNeighbor, Val);
	
	std::cout << "Chance to create new neighbor node: ";
	std::cin >> Val;
	Preferences.SetPreference(NodeActions::GenerateNewNeighbor, Val);
	
	std::cout << "Chance to skip update: ";
	std::cin >> Val;
	Preferences.SetPreference(NodeActions::Sleep, Val);
	
	std::cout << "\nConfirm [Y/N]" << std::endl;
	return Menu::Confirm() ? Preferences : SetupDetailedPreferences();
}

void SetupInitialNet()
{
	bool Random = true;
	int Size = 50;

	std::cout << "\n=====================================" << std::endl;
	std::cout << "Use default settings for initial node net? [Y/N]" << std::endl;

	if (!Menu::Confirm())
	{
		std::cout << "\nMake initial node net random? [Y/N]" << std::endl;
		Random = Menu::Confirm();

		std::cout << "\nSize of initial net: ";
		std::cin >> Size;
	}
	GenerateNodeNet(Size, Random);
}

void AppLoop()
{
	Node::SetPreferences(SetupPreferences());
	SetupInitialNet();
	Cycle();

	std::cout << "\n=====================================" << std::endl;
	std::cout << "Restart application? [Y/N]" << std::endl;

	if (Menu::Confirm())
		AppLoop();
}

int main()
{
	std::cout << "App created by Ivan Babanov, IB Dray for EEGames" << std::endl;
	std::cout << "Press ESC to stop execution." << std::endl << std::endl;

	AppLoop();	

    std::cin.get();
}
