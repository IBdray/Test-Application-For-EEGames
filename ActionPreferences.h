#pragma once

#include <unordered_map>

enum class NodeActions;
enum class NodeHandlers;


struct ActionPreferences
{
public:
	void SetPreference(NodeActions Action, size_t Chance);
	void SetPreference(NodeHandlers Handler, size_t Chance);

	size_t GetPreference(NodeActions Action) const;
	size_t GetPreference(NodeHandlers Handler) const;

	size_t GetChancesSum() const {return mChancesSum;}

	NodeActions GetRandomAction() const;
	
	ActionPreferences(size_t EqualChance = 50);

private:
	size_t mChancesSum = 0;
	std::unordered_map<NodeActions, size_t> mActionsMap;
	std::unordered_map<NodeHandlers, size_t> mHandlersMap;
};

