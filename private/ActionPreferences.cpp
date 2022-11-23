#include "../public/ActionPreferences.h"
#include "../public/NodeEnums.h"
#include "../public/RandomGenerator.h"


void ActionPreferences::SetPreference(const NodeActions& Action, const int Chance)
{
	mChancesSum += Chance - mActionsMap.at(Action);
	mActionsMap[Action] = Chance;
}

int ActionPreferences::GetPreference(const NodeActions& Action) const
{
	return mActionsMap.at(Action);
}

NodeActions ActionPreferences::GetRandomAction() const
{
	const int RandomNumber = RandomGenerator::GenerateNumber(0, mChancesSum);
	int Min = 0;
	for (auto& Action : mActionsMap)
	{
		if (RandomNumber >= Min && RandomNumber <= Action.second + Min)
			return Action.first;
		else
			Min += Action.second;
	}
	return NodeActions::GenerateEvent;
}

ActionPreferences::ActionPreferences(const int EqualChance)
{
	mActionsMap[NodeActions::GenerateEvent] = EqualChance;
	mActionsMap[NodeActions::SubscribeNeighbor] = EqualChance;
	mActionsMap[NodeActions::UnsubscribeNeighbor] = EqualChance;
	mActionsMap[NodeActions::GenerateNewNeighbor] = EqualChance;
	mActionsMap[NodeActions::Sleep] = EqualChance;

	mChancesSum = static_cast<int>(mActionsMap.size()) * EqualChance;
}
