#include <random>

#include "ActionPreferences.h"
#include "NodeEnums.h"

#include <iostream>

void ActionPreferences::SetPreference(NodeActions Action, size_t Chance)
{
	mChancesSum += Chance - mActionsMap.at(Action);
	mActionsMap[Action] = Chance;
}
void ActionPreferences::SetPreference(NodeHandlers Handler, size_t Chance)
{
	mChancesSum += Chance - mHandlersMap.at(Handler);
	mHandlersMap[Handler] = Chance;
}

size_t ActionPreferences::GetPreference(NodeActions Action) const
{
	return mActionsMap.at(Action);
}

size_t ActionPreferences::GetPreference(NodeHandlers Handler) const
{
	return mHandlersMap.at(Handler);
}

NodeActions ActionPreferences::GetRandomAction() const
{
	std::random_device RandomDevice;
	std::mt19937 Rand(RandomDevice());
	std::uniform_int_distribution<size_t> RandomDistribution(0, mChancesSum);

	size_t RandomNumber = RandomDistribution(Rand);
	size_t Min = 0;
	for (auto& Action : mActionsMap)
	{
		if (RandomNumber >= Min && RandomNumber <= Action.second + Min)
			return Action.first;
		else
			Min += Action.second;
	}
	return NodeActions::GenerateEvent;
}

ActionPreferences::ActionPreferences(size_t EqualChance)
{
	mActionsMap[NodeActions::GenerateEvent] = EqualChance;
	mActionsMap[NodeActions::SubscribeToNeighbor] = EqualChance;
	mActionsMap[NodeActions::UnsubscribeFromNeighbor] = 2 * EqualChance;
	mActionsMap[NodeActions::GenerateNewNeighbor] = EqualChance;
	mActionsMap[NodeActions::Sleep] = EqualChance;

	mHandlersMap[NodeHandlers::SumHandler] = EqualChance;
	mHandlersMap[NodeHandlers::EventCounterHandler] = EqualChance;

	mChancesSum = mActionsMap.size() * EqualChance;
}
