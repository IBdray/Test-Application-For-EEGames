#pragma once

enum class NodeActions
{
	GenerateEvent,
	SubscribeToNeighbor,
	UnsubscribeFromNeighbor,
	GenerateNewNeighbor,
	Sleep,
};

enum class NodeHandlers
{
	SumHandler,
	EventCounterHandler,
};

