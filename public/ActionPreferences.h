#pragma once

#include <unordered_map>

enum class NodeActions;


struct ActionPreferences
{
	ActionPreferences(const int EqualChance = 50);

	void SetPreference(const NodeActions& Action, const int Chance);
	int GetPreference(const NodeActions& Action) const;

	int GetChancesSum() const {return mChancesSum;}

	NodeActions GetRandomAction() const;
	

private:
	int mChancesSum = 0;
	std::unordered_map<NodeActions, int> mActionsMap;
};

