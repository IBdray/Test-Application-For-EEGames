#pragma once
#include "Node.h"


struct EventHandler
{
	virtual void Handle(const Node& Author, const Node& Subscriber) = 0;
	virtual ~EventHandler() = default;
};

struct SumHandler final : EventHandler
{

	SumHandler() = default;
	virtual void Handle(const Node& Author, const Node& Subscriber) override;
};

struct CountHandler final : EventHandler
{
	CountHandler() = default;
	virtual void Handle(const Node& Author, const Node& Subscriber) override;
};
