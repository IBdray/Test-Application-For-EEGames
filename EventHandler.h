#pragma once
#include "Node.h"


template<typename T>
struct EventHandler
{
	virtual void Handle(const T& Author, const T& Subscriber) = 0;
	virtual ~EventHandler() = default;
};

struct SumHandler : EventHandler<Node>
{

	SumHandler() = default;
	virtual void Handle(const Node& Author, const Node& Subscriber) override;
};

struct CountHandler : EventHandler<Node>
{
	CountHandler() = default;
	virtual void Handle(const Node& Author, const Node& Subscriber) override;
};
