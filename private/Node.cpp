#include "../public/Node.h"
#include "../public/NodeEnums.h"
#include "../public/NodeManager.h"
#include "../public/RandomGenerator.h"
#include "../public/EventBase.h"
#include "../public/EventHandler.h"

#include <algorithm>

ActionPreferences Node::mActionPreferences {};


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
	NodeManager::AddNode(NewNode);
	return NewNode;
}

template<typename ... Ts>
Node::NodePtr Node::Factory::CreateNeighborTo(const NodePtr& ParentNode, Ts... Args)
{
	auto NewNode = CreateNode(std::forward<Ts>(Args)...);
	NewNode->SubscribeNeighbor(ParentNode);
	return NewNode;
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
			SubscribeNeighbor();
			break;
		case NodeActions::UnsubscribeFromNeighbor:
			UnsubscribeNeighbor();
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

	CheckAndDestroy();
}

bool Node::CheckAndDestroy()
{
	if (mNeighbors.empty())
	{
		NodeManager::RemoveNode(shared_from_this());
		return true;
	}
	return false;
}

void Node::GenerateEvent() const
{
	for (const auto& Subscriber : mSubscribers)
		Subscriber->ReceiveEvent(NumberEvent(), *this);
}

template<typename T>
void Node::ReceiveEvent(const EventBase<T>& EventData, const Node& Other)
{
	mAuthorsData[Other.GetName()].Sum += EventData.GetData();
	mAuthorsData[Other.GetName()].Counter += 1;
	mAuthorsData[Other.GetName()].HandlerPtr->Handle(Other, *this);
}


void Node::SubscribeNeighbor(const NodePtr& Subscriber)
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
		if (!mAuthors[Author].expired()) 
			Unsubscribe(*mAuthors[Author].lock());
	}
	else if (Neighbor)
		Unsubscribe(*Neighbor);
}


void Node::Subscribe(const NodePtr& Subscriber)
{
	if (Subscriber && Subscriber != shared_from_this() && !IsSubscriber(*Subscriber))
	{
		mSubscribers.emplace_back(Subscriber);
		Subscriber->AddAuthor(shared_from_this());
		AddNeighbor(Subscriber);
	}
}

void Node::AddAuthor(const NodePtr& Author)
{
	if (Author && Author != shared_from_this() && !IsAuthor(*Author))
	{
		mAuthors.emplace_back(Author);
		SetEventHandler(Author);
		AddNeighbor(Author);

	}
}

void Node::AddNeighbor(const NodePtr& Neighbor)
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

void Node::SetEventHandler(const NodePtr& Author)
{
	if (RandomGenerator::GenerateNumber(0, 1) == 0)
		mAuthorsData[Author->GetName()].HandlerPtr = std::make_unique<SumHandler>();
	else
		mAuthorsData[Author->GetName()].HandlerPtr = std::make_unique<CountHandler>();
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


void Node::SetPreferences(ActionPreferences&& Preferences)
{
	mActionPreferences = std::move(Preferences);
}
