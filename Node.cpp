#include "Node.h"

#include <algorithm>
#include <iostream>
#include <random>


int Node::mFactoryCounter = 0;


template<typename ... Ts>
std::shared_ptr<Node> Node::Create(Ts&&... Args)
{
	//std::make_shared throws errors so this is a work around
	return std::shared_ptr<Node>(new Node(std::forward<Ts>(Args)...));
}


Node::Node()
{
	this->mName = "Node_" + std::to_string(mFactoryCounter++);
	this->mPreferences = Preferences();
}

template<typename T>
Node::Node(T&&Name, Preferences Preferences)
	: mName(std::forward<T>(Name)), mPreferences(Preferences)
{
}


std::shared_ptr<Node> Node::GenerateNewNeighbor()
{
	auto Temp = Create();
	SubscribeTo(Temp);
	return Temp;
}



void Node::SubscribeTo(std::shared_ptr<Node> Other)
{
	if (Other != shared_from_this() && std::find(mSubscribedTo.cbegin(), mSubscribedTo.cend(), Other) == mSubscribedTo.cend())
	{
		mSubscribedTo.emplace_back(Other);
		Other->AddSubscriber(shared_from_this());

		if (CheckIsNodeInContainer(Other, mNeighbors))
			mNeighbors.emplace_back(Other);

	}
}

void Node::AddSubscriber(std::shared_ptr<Node> Other)
{
	if (Other != shared_from_this() && CheckIsNodeInContainer(Other, mSubscribers))
	{
		mSubscribers.emplace_back(Other);

		if (CheckIsNodeInContainer(Other, mNeighbors))
			mNeighbors.emplace_back(Other);
	}
}




void Node::Update()
{
	
}

void Node::CreateEvent()
{
	auto EventValue = GenerateRandomNumber();
	for (auto Subscriber : mSubscribers)
	{
		if (!Subscriber.expired())
			Subscriber.lock()->ReceiveEvent(EventValue, *this);
	}
}

void Node::ReceiveEvent(int Value, const Node& Other)
{
	// TODO: Save value to member variable
	// maybe create a struct with ptr to subscribed node, sum of event values and number of received events
	EventHandlerSum(Value, Other);
}


bool Node::CheckIsNodeInContainer(const std::shared_ptr<Node>& NodePtr,
	const std::vector<std::weak_ptr<Node>>& Container) const
{
	return std::find_if(Container.cbegin(), Container.cend(), 
		[NodePtr](const std::weak_ptr<Node>& ContainerNode)
		{
			return !ContainerNode.expired() ? ContainerNode.lock() == NodePtr : false;
		}) == Container.cend();
}

int Node::GenerateRandomNumber()
{
	std::random_device RandomDevice;
	std::mt19937 Rand(RandomDevice());
	std::uniform_int_distribution<int> RandomDistribution;

	return RandomDistribution(Rand);
}

void Node::EventHandlerSum(int Sum, const Node& Other)
{
	std::cout << Other.mName << "->" << mName << ": " << Sum;
}

void Node::EventHandlerNumberOfEvents(int Num, const Node& Other)
{
	std::cout << Other.mName << "->" << mName << ": " << Num;
}
