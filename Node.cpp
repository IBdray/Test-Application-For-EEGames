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
	mName = "Node_" + std::to_string(mFactoryCounter++);
	mPreferences = Preferences();
	mActive = false;
	mPendingKill = false;
}

template<typename T>
Node::Node(T&&Name, Preferences Preferences)
	: mName(std::forward<T>(Name)), mPreferences(Preferences), mActive(false), mPendingKill(false)
{
}


std::shared_ptr<Node> Node::GenerateNewNeighbor()
{
	auto Temp = Create();
	SubscribeTo(Temp);
	return Temp;
}


void Node::Update()
{
	if (mActive)
	{
		GenerateEvent();
	}
	else
	{
		mActive = true;
	}
}

void Node::GenerateEvent()
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



std::shared_ptr<Node> Node::SubscribeToNeighbor(std::shared_ptr<Node> Other)
{
	return SubscribeTo(Other);
}

std::shared_ptr<Node> Node::UnsubscribeFromNeighbor(std::shared_ptr<Node> Other)
{

	return UnsubscribeFrom(Other);
}


std::shared_ptr<Node> Node::SubscribeTo(std::shared_ptr<Node> Other)
{
	if (Other != shared_from_this() && !CheckIsSubscribedTo(Other))
	{
		mSubscribedTo.emplace_back(Other);
		Other->AddSubscriber(shared_from_this());
		BecomeNeighbors(Other);
	}
	return Other;
}

std::shared_ptr<Node> Node::UnsubscribeFrom(std::shared_ptr<Node> Other)
{
	if (Other != shared_from_this() && CheckIsSubscribedTo(Other))
	{
		mSubscribedTo.erase(FindNodeInContainer(Other, mSubscribedTo));
		Other->RemoveSubscriber(shared_from_this());
		CheckAndRemoveNeighbor(Other);
	}
	return Other;
}

void Node::AddSubscriber(std::shared_ptr<Node> Other)
{
	if (Other != shared_from_this() && !CheckIsSubscriber(Other))
	{
		mSubscribers.emplace_back(Other);
	}
}

void Node::RemoveSubscriber(std::shared_ptr<Node> Other)
{
	if (Other != shared_from_this() && CheckIsSubscriber(Other))
	{
		mSubscribers.erase(FindNodeInContainer(Other, mSubscribers));
	}
}


void Node::BecomeNeighbors(std::shared_ptr<Node> Other)
{
	if (!CheckIsNeighbors(Other))
		mNeighbors.emplace_back(Other);
	if (!Other->CheckIsNeighbors(shared_from_this()))
		Other->BecomeNeighbors(shared_from_this());
}

void Node::CheckAndRemoveNeighbor(std::shared_ptr<Node> Other)
{
	if (!Other->CheckIsSubscribedTo(shared_from_this()) && CheckIsNeighbors(Other))
	{
		mNeighbors.erase(FindNodeInContainer(Other, mNeighbors));
		Other->CheckAndRemoveNeighbor(shared_from_this());
	}
}



bool Node::CheckIsNeighbors(const std::shared_ptr<Node>& Other) const
{
	return CheckIsNodeInContainer(Other, mNeighbors);
}

bool Node::CheckIsSubscribedTo(const std::shared_ptr<Node>& Other) const
{
	return CheckIsNodeInContainer(Other, mSubscribedTo);
}

bool Node::CheckIsSubscriber(const std::shared_ptr<Node>& Other) const
{
	return CheckIsNodeInContainer(Other, mSubscribers);
}



bool Node::CheckIsNodeInContainer(const std::shared_ptr<Node>& NodePtr,
                                  const std::vector<std::weak_ptr<Node>>& Container) const
{
	return FindNodeInContainer(NodePtr, Container) != Container.cend();
}

bool Node::CheckIsNodeInContainer(const std::shared_ptr<Node>& NodePtr,
								  const std::vector<std::shared_ptr<Node>>& Container) const
{
	return FindNodeInContainer(NodePtr, Container) != Container.cend();
}

Node::WeakNodeIt Node::FindNodeInContainer(const std::shared_ptr<Node>& NodePtr,
										   const std::vector<std::weak_ptr<Node>>& Container) const
{
	return std::find_if(Container.cbegin(), Container.cend(), 
		[NodePtr](const std::weak_ptr<Node>& ContainerNode)
		{
			return !ContainerNode.expired() ? ContainerNode.lock() == NodePtr : false;
		});
}

Node::SharedNodeIt Node::FindNodeInContainer(const std::shared_ptr<Node>& NodePtr,
											 const std::vector<std::shared_ptr<Node>>& Container) const
{
	return std::find(Container.cbegin(), Container.cend(), NodePtr);
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

