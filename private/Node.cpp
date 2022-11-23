#include "../public/Node.h"
#include "../public/NodeEnums.h"
#include "../public/RandomGenerator.h"
#include "../public/EventBase.h"
#include "../public/EventHandler.h"

#include <algorithm>
#include <iostream>



ActionPreferences Node::mActionPreferences {}; 
std::list<std::shared_ptr<Node>> Node::Manager::mNodesList {};


Node::Node()
{
	static int FactoryCounter;
	mName = "Node_" + std::to_string(FactoryCounter++);
	mActive = false;
}

template<typename T>
Node::Node(T&&Name, bool Active)
	: mName(std::forward<T>(Name)), mActive(Active)
{
}


template<typename ... Ts>
Node::NodePtr Node::Factory::CreateNode(Ts... Args)
{
	// std::make_shared not working with private constructors
	auto NewNode = NodePtr(new Node(std::forward<Ts>(Args)...));
	Manager::AddNode(NewNode);
	return NewNode;
}

template<typename ... Ts>
Node::NodePtr Node::Factory::CreateNeighborTo(const NodePtr& ParentNode, Ts... Args)
{
	auto NewNode = CreateNode(std::forward<Ts>(Args)...);
	NewNode->SubscribeNeighbor(ParentNode);
	return NewNode;
}


void Node::Manager::AddNode(const NodePtr& NodePtr)
{
	if (NodePtr)
		mNodesList.emplace_back(NodePtr);
}

void Node::Manager::RemoveNode(const NodePtr& NodePtr)
{
	if (NodePtr)
		std::find(mNodesList.begin(),mNodesList.end(), NodePtr)->reset();
}


void Node::Update(const bool Active)
{
	if (mActive && Active)
	{
		switch (mActionPreferences.GetRandomAction())
		{
		case NodeActions::GenerateEvent:
			GenerateEvent();
			break;
		case NodeActions::SubscribeToNeighbor:
			SubscribeNeighbor();
			break;
		case NodeActions::UnsubscribeFromNeighbor:
			UnsubscribeNeighbor(nullptr);
			break;
		case NodeActions::GenerateNewNeighbor:
			Factory::CreateNeighborTo(shared_from_this());
			break;
		case NodeActions::Sleep:
			break;
		}
	}
	else
		mActive = true;

	if (mNeighbors.empty())
		Destroy();
}

void Node::Destroy()
{
	Manager::RemoveNode(shared_from_this());
}

void Node::GenerateEvent() const
{
	for (const auto& Subscriber : mSubscribers)
		Subscriber->ReceiveEvent(NumberEvent(), *this);
}

template<typename T>
void Node::ReceiveEvent(const EventBase<T>& EventData, const Node& Other)
{
	mNeighborsDataMap[Other.GetName()].Sum += EventData.GetData();
	mNeighborsDataMap[Other.GetName()].Counter += 1;
	//mNeighborsDataMap[Other.GetName()].Handler.target<>();

	const bool UseSumHandler = RandomGenerator::GenerateNumber(0, 1) == 0;
	UseSumHandler ? SumHandler().Handle(Other, *this) : CountHandler().Handle(Other, *this);
}


void Node::SubscribeNeighbor(const std::shared_ptr<Node>& Subscriber)
{
	if (!mNeighbors.empty() && !Subscriber)
	{
		const auto NeighborIndex = RandomGenerator::GenerateNumber(0, static_cast<int>(mNeighbors.size()) - 1);
		if (!mNeighbors[NeighborIndex].expired())
		{
			const auto Neighbor = mNeighbors[NeighborIndex].lock();
			if (!Neighbor->GetNeighbors().empty())
			{
				const auto NeighborsNeighborIndex = RandomGenerator::GenerateNumber(0, static_cast<int>(Neighbor->GetNeighbors().size()) - 1);
				if (!Neighbor->GetNeighbors()[NeighborsNeighborIndex].expired())
				{
					const auto NeighborsNeighbor = Neighbor->GetNeighbors()[NeighborsNeighborIndex].lock();
					if (NeighborsNeighbor != shared_from_this())
						Subscribe(NeighborsNeighbor);
				}
			}
			else
				Subscribe(Neighbor);
		}
	}
	else if (Subscriber)
		Subscribe(Subscriber);
}

void Node::UnsubscribeNeighbor(Node* Neighbor)
{
	if (!mAuthors.empty() && !Neighbor)
	{
		const auto Author = RandomGenerator::GenerateNumber(0, static_cast<int>(mAuthors.size()) - 1);
		Unsubscribe(*mAuthors[Author].lock());
	}
	else if (Neighbor)
		Unsubscribe(*Neighbor);
}


void Node::Subscribe(const std::shared_ptr<Node>& Subscriber)
{
	if (Subscriber && Subscriber != shared_from_this() && !IsSubscriber(*Subscriber))
	{
		mSubscribers.emplace_back(Subscriber);
		Subscriber->AddAuthor(shared_from_this());
		AddNeighbor(Subscriber);
	}
}

void Node::AddAuthor(const std::shared_ptr<Node>& Author)
{
	if (Author && Author != shared_from_this() && !IsAuthor(*Author))
	{
		mAuthors.emplace_back(Author);
		AddNeighbor(Author);
	}
}

void Node::AddNeighbor(const std::shared_ptr<Node>& Neighbor)
{
	if (Neighbor && Neighbor != shared_from_this() && !IsNeighbors(*Neighbor))
		mNeighbors.emplace_back(Neighbor);
}


void Node::Unsubscribe(Node& Subscriber)
{
	const auto NodeIt = FindNodeInContainer(Subscriber, mSubscribers);
	if (NodeIt != mSubscribers.end())
	{
		ResetAndEraseNode(NodeIt, mSubscribers);
		Subscriber.RemoveAuthor(*this);
		RemoveNeighbor(Subscriber);
	}
}

void Node::RemoveAuthor(const Node& Author)
{
	const auto NodeIt = FindNodeInContainer(Author, mAuthors);
	if (NodeIt != mAuthors.end())
	{
		ResetAndEraseNode(NodeIt, mAuthors);
		RemoveNeighbor(Author);
	}
}

void Node::RemoveNeighbor(const Node& Neighbor)
{
	const auto NodeIt = FindNodeInContainer(Neighbor, mNeighbors);
	if (NodeIt != mNeighbors.end())
		ResetAndEraseNode(NodeIt, mNeighbors);
}


bool Node::IsNeighbors(const Node& Neighbor) const
{
	return FindNodeInContainer(Neighbor, mNeighbors) != mNeighbors.cend();
}
bool Node::IsAuthor(const Node& Author) const
{
	return FindNodeInContainer(Author, mAuthors) != mAuthors.cend();
}
bool Node::IsSubscriber(const Node& Subscriber) const
{
	return FindNodeInContainer(Subscriber, mSubscribers) != mSubscribers.cend();
}

template<typename T>
auto Node::FindNodeInContainer(const Node& NodeRef, T& Container) const -> decltype(std::begin(Container))
{
	return std::find_if(std::begin(Container),std::end(Container), 
		[&NodeRef](const std::weak_ptr<Node>& ContainerNode) 
		{
			if (!ContainerNode.expired())
				return NodeRef == *ContainerNode.lock();
			return false;
		});
}

template<typename I, typename C>
void Node::ResetAndEraseNode(const I& It, C& Container)
{
	auto& TempNode = *It;
	TempNode.reset();
	Container.erase(It);
}


void Node::SetPreferences(ActionPreferences Preferences)
{
	mActionPreferences = Preferences;
}
