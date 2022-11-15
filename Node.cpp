#include "Node.h"
#include "NodeEnums.h"

#include <algorithm>
#include <iostream>
#include <random>


int Node::mFactoryCounter = 0;
std::list<std::shared_ptr<Node>> Node::Nodes {};


template<typename ... Ts>
std::shared_ptr<Node> Node::Create(Ts&&... Args)
{
	std::shared_ptr<Node> NewNode(new Node(std::forward<Ts>(Args)...));
	Nodes.emplace_back(NewNode);
	NewNode->ThisNodeIndex = Nodes.size() - 1;

	return NewNode;


	//std::make_shared throws errors so this is a work around
	//return std::shared_ptr<Node>(new Node(std::forward<Ts>(Args)...));
}


Node::Node()
{
	mName = "Node_" + std::to_string(mFactoryCounter++);
	mActionPreferences = ActionPreferences(50);
	mActive = false;
	mPendingKill = false;
}

template<typename T>
Node::Node(T&&Name, ActionPreferences Preferences, bool Active)
	: mName(std::forward<T>(Name)), mActionPreferences(Preferences), mActive(Active), mPendingKill(false)
{
}



void Node::Update()
{
	if (mActive)
	{
		switch (mActionPreferences.GetRandomAction())
		{
		case NodeActions::GenerateEvent:
			GenerateEvent();
			break;

		case NodeActions::SubscribeToNeighbor:
			SubscribeToNeighbor();
			break;

		case NodeActions::UnsubscribeFromNeighbor:
			UnsubscribeFromNeighbor();
			break;

		case NodeActions::GenerateNewNeighbor:
			GenerateNewNeighbor();
			break;

		case NodeActions::Sleep:
			std::cout << GetName() << " sleeping" << std::endl;

			break;
		}
		
	}
	else
	{
		mActive = true;
		std::cout << GetName() << " activated" << std::endl;
	}

	CheckAndKill();
}

void Node::CheckAndKill()
{
	if (mPendingKill && mNeighbors.empty())
	{
		mActive = false;

		std::cout << "\nDeleting " << GetName() << "\t\t| neighbors - "
				  << mNeighbors.size() << ", subsTo - " << mSubscribedTo.size()
				  << ", subs - " << mSubscribers.size() << std::endl;
		
		auto ThisNode = Nodes.begin();
		std::advance(ThisNode, ThisNodeIndex);
		ThisNode->reset();
	}
	else
		mPendingKill = false;
}

void Node::GenerateEvent()
{
	std::cout << GetName() << " generated event" << std::endl;
	const auto EventValue = GenerateRandomNumber();
	for (const auto& Subscriber : mSubscribers)
	{
		if (!Subscriber.expired())
			Subscriber.lock()->ReceiveEvent(EventValue, *this);
	}
}


void Node::ReceiveEvent(int Value, const Node& Other)
{
	NeighborsDataMap[Other.GetName()].Sum += Value;
	NeighborsDataMap[Other.GetName()].EventCounter += 1;

	GenerateRandomNumber(0, 1) == 0 ? EventHandlerSum(Value, Other) : EventHandlerNumberOfEvents(Other);
}


void Node::SubscribeToNeighbor()
{
	if (!mNeighbors.empty())
	{
		const auto NeighborIndex = GenerateRandomNumber(0, static_cast<int>(mNeighbors.size()) - 1);

		if (!mNeighbors[NeighborIndex].expired())
		{
			const auto Neighbor = mNeighbors[NeighborIndex].lock();

			if (!Neighbor->GetNeighbors().empty())
			{
				const auto NeighborsNeighborIndex = GenerateRandomNumber(0, static_cast<int>(Neighbor->GetNeighbors().size()) - 1);
				if (!Neighbor->GetNeighbors()[NeighborsNeighborIndex].expired())
				{
					const auto NeighborsNeighbor = Neighbor->GetNeighbors()[NeighborsNeighborIndex].lock();
					if (NeighborsNeighbor != shared_from_this())
					{
						SubscribeTo(NeighborsNeighbor);

						std::cout << GetName() << " subscribed to " << NeighborsNeighbor->GetName() << std::endl;
					}
				}
			}
			else
			{
				SubscribeTo(Neighbor);

				std::cout << GetName() << " subscribed to " << Neighbor->GetName() << std::endl;
			}
		}
	}
	else
	{
		std::cout << GetName() << " subscription failed" << std:: endl;
	}
}

void Node::SubscribeToNeighbor(const std::shared_ptr<Node>& Other)
{
	SubscribeTo(Other);
}

void Node::UnsubscribeFromNeighbor()
{
	if (!mSubscribedTo.empty())
	{
		const auto SubscribedIndex = GenerateRandomNumber(0, static_cast<int>(mSubscribedTo.size()) - 1);
		const auto Neighbor = mSubscribedTo[SubscribedIndex];
		UnsubscribeFrom(Neighbor);

		std::cout << GetName() << " unsubscribed from " << Neighbor->GetName() << std::endl;
	}
	else
	{
		std::cout << GetName() << " is not subscribed to anyone" << std::endl;
	}
}

void Node::UnsubscribeFromNeighbor(const std::shared_ptr<Node>& Other)
{
	UnsubscribeFrom(Other);
}

std::shared_ptr<Node> Node::GenerateNewNeighbor()
{
	//TODO: fix memory limit issue for amount of nodes (maybe with exception handling)
	auto Temp = Create();
	SubscribeTo(Temp);

	std::cout << Temp->GetName() << " was created by " << GetName() << std::endl;
	return Temp;
}


std::shared_ptr<Node> Node::SubscribeTo(const std::shared_ptr<Node>& Other)
{
	if (Other && Other != shared_from_this() && !CheckIsSubscribedTo(Other))
	{
		mSubscribedTo.emplace_back(Other);
		Other->AddSubscriber(shared_from_this());
		BecomeNeighbors(Other);
	}
	return Other;
}

std::shared_ptr<Node> Node::UnsubscribeFrom(const std::shared_ptr<Node>& Other)
{
	if (Other && Other != shared_from_this() && CheckIsSubscribedTo(Other))
	{
		const auto CreatorIt = FindNodeInContainer(Other, mSubscribedTo);
		auto Creator = *CreatorIt;
		Creator.reset();

		mSubscribedTo.erase(CreatorIt);
		Other->RemoveSubscriber(shared_from_this());
		CheckAndRemoveNeighbor(Other);
	}
	return Other;
}

void Node::AddSubscriber(const std::shared_ptr<Node>& Other)
{
	if (Other && Other != shared_from_this() && !CheckIsSubscriber(Other))
	{
		mSubscribers.emplace_back(Other);
	}
}

void Node::RemoveSubscriber(const std::shared_ptr<Node>& Other)
{
	if (Other && Other != shared_from_this() && CheckIsSubscriber(Other))
	{
		const auto SubscriberIt = FindNodeInContainer(Other, mSubscribers);
		auto Subscriber = *SubscriberIt;

		Subscriber.reset();
		mSubscribers.erase(SubscriberIt);
	}
}


void Node::BecomeNeighbors(const std::shared_ptr<Node>& Other)
{
	if (Other && !CheckIsNeighbors(Other))
		mNeighbors.emplace_back(Other);
	if (Other && !Other->CheckIsNeighbors(shared_from_this()))
		Other->BecomeNeighbors(shared_from_this());
}

void Node::CheckAndRemoveNeighbor(const std::shared_ptr<Node>& Other)
{
	if (!Other->CheckIsSubscribedTo(shared_from_this()) && CheckIsNeighbors(Other))
	{
		const auto NeighborIt = FindNodeInContainer(Other, mNeighbors);
		auto Neighbor = *NeighborIt;

		Neighbor.reset();
		mNeighbors.erase(NeighborIt);
		Other->CheckAndRemoveNeighbor(shared_from_this());
	}

	if (mNeighbors.empty())
		mPendingKill = true;
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




int Node::GenerateRandomNumber(const int Min, const int Max)
{
	std::random_device RandomDevice;
	std::mt19937 Rand(RandomDevice());
	std::uniform_int_distribution<int> RandomDistribution(Min, Max);

	return RandomDistribution(Rand);
}

void Node::EventHandlerSum(const int Sum, const Node& Other)
{	
	std::cout << Other.mName << "->" << mName << ": S = " << NeighborsDataMap[Other.GetName()].Sum << std::endl;
}

void Node::EventHandlerNumberOfEvents(const Node& Other)
{
	std::cout << Other.mName << "->" << mName << ": N = " << NeighborsDataMap[Other.GetName()].EventCounter << std::endl;
}

