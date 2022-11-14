#include "Node.h"
#include "NodeEnums.h"

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
			std::cout << "Generate New Neighbor" << std::endl;

			GenerateNewNeighbor();
			break;

		case NodeActions::Sleep:
			std::cout << "Sleep" << std::endl;

			break;
		}
		
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


void Node::SubscribeToNeighbor()
{
	if (!mNeighbors.empty())
	{
		const auto NeighborIndex = GenerateRandomNumber() % mNeighbors.size();

		if (!mNeighbors[NeighborIndex].expired())
		{
			const auto Neighbor = mNeighbors[NeighborIndex].lock();

			if (!Neighbor->GetNeighbors().empty())
			{
				const auto NeighborsNeighborIndex = GenerateRandomNumber() % Neighbor->GetNeighbors().size();
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
		}
	}
}

void Node::SubscribeToNeighbor(std::shared_ptr<Node> Other)
{
	SubscribeTo(Other);
}

void Node::UnsubscribeFromNeighbor()
{
	if (!mSubscribedTo.empty())
	{
		const auto SubscribedIndex = GenerateRandomNumber() % mSubscribedTo.size();
		const auto Neighbor = mSubscribedTo[SubscribedIndex];
		
		UnsubscribeFrom(Neighbor);

		std::cout << GetName() << " unsubscribed from " << Neighbor->GetName() << std::endl;
	}
}

void Node::UnsubscribeFromNeighbor(std::shared_ptr<Node> Other)
{
	UnsubscribeFrom(Other);
}

std::shared_ptr<Node> Node::GenerateNewNeighbor()
{
	//TODO: fix memory limit issue for amount of nodes (maybe with exception handling)
	auto Temp = Create();
	SubscribeTo(Temp);
	return Temp;
}


std::shared_ptr<Node> Node::SubscribeTo(std::shared_ptr<Node> Other)
{
	if (Other && Other != shared_from_this() && !CheckIsSubscribedTo(Other))
	{
		mSubscribedTo.emplace_back(Other);
		Other->AddSubscriber(shared_from_this());
		BecomeNeighbors(Other);
	}
	return Other;
}

std::shared_ptr<Node> Node::UnsubscribeFrom(std::shared_ptr<Node> Other)
{
	if (Other && Other != shared_from_this() && CheckIsSubscribedTo(Other))
	{
		mSubscribedTo.erase(FindNodeInContainer(Other, mSubscribedTo));
		Other->RemoveSubscriber(shared_from_this());
		CheckAndRemoveNeighbor(Other);
	}
	return Other;
}

void Node::AddSubscriber(std::shared_ptr<Node> Other)
{
	if (Other && Other != shared_from_this() && !CheckIsSubscriber(Other))
	{
		mSubscribers.emplace_back(Other);
	}
}

void Node::RemoveSubscriber(std::shared_ptr<Node> Other)
{
	if (Other && Other != shared_from_this() && CheckIsSubscriber(Other))
	{
		mSubscribers.erase(FindNodeInContainer(Other, mSubscribers));
	}
}


void Node::BecomeNeighbors(std::shared_ptr<Node> Other)
{
	if (Other && !CheckIsNeighbors(Other))
		mNeighbors.emplace_back(Other);
	if (Other && !Other->CheckIsNeighbors(shared_from_this()))
		Other->BecomeNeighbors(shared_from_this());
}

void Node::CheckAndRemoveNeighbor(std::shared_ptr<Node> Other)
{
	if (!Other->CheckIsSubscribedTo(shared_from_this()) && CheckIsNeighbors(Other))
	{
		mNeighbors.erase(FindNodeInContainer(Other, mNeighbors));
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




int Node::GenerateRandomNumber()
{
	std::random_device RandomDevice;
	std::mt19937 Rand(RandomDevice());
	std::uniform_int_distribution<int> RandomDistribution(-1000, 1000);

	return RandomDistribution(Rand);
}

void Node::EventHandlerSum(int Sum, const Node& Other)
{
	static int SavedSum;
	SavedSum += Sum;
	std::cout << Other.mName << "->" << mName << ": " << SavedSum << std::endl;
}

void Node::EventHandlerNumberOfEvents(int Num, const Node& Other)
{
	static int SavedNum;
	SavedNum += Num;
	std::cout << Other.mName << "->" << mName << ": " << SavedNum << std::endl;
}

