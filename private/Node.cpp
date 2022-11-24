#include "../public/Node.h"
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
	// Exception handling in case if not enough free memory
	try
	{
		// std::make_shared not working with private constructors
		auto NewNode = NodePtr(new Node(std::forward<Ts>(Args)...));
		NodeManager::AddNode(NewNode);
		return NewNode;
	}
	catch (...)
	{
		return nullptr;
	}
}

template<typename ... Ts>
Node::NodePtr Node::Factory::CreateNeighborTo(const NodePtr& ParentNode, Ts... Args)
{
	auto NewNode = CreateNode(std::forward<Ts>(Args)...);
	NewNode->Subscribe(ParentNode);
	return NewNode;
}


void Node::Update(const bool Force, const NodeActions& Action)
{
	if (mActive || Force)
	{
		switch (Action == NodeActions::Default ? mActionPreferences.GetRandomAction() : Action)
		{
		case NodeActions::GenerateEvent:
			GenerateEvent();
			break;
		case NodeActions::SubscribeNeighbor:
			mUpdateBuffer = std::make_pair(SubscribeNeighbor(), NodeActions::SubscribeNeighbor);
			break;
		case NodeActions::UnsubscribeNeighbor:
			mUpdateBuffer = std::make_pair(UnsubscribeNeighbor(), NodeActions::UnsubscribeNeighbor);
			break;
		case NodeActions::GenerateNewNeighbor:
			mUpdateBuffer = std::make_pair(Factory::CreateNode(), NodeActions::GenerateNewNeighbor);
			break;
		case NodeActions::Sleep:
			mUpdateBuffer = std::make_pair(nullptr, NodeActions::Sleep);
			break;
		case NodeActions::Default:
			break;
		}
	}

	mActive = true;
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

void Node::SubmitUpdate()
{
	if (mUpdateBuffer.first)
	{
		switch (mUpdateBuffer.second)
		{
		case NodeActions::GenerateNewNeighbor:
			mUpdateBuffer.first->Subscribe(shared_from_this());
			break;
		case NodeActions::SubscribeNeighbor:
			Subscribe(mUpdateBuffer.first);
			break;
		case NodeActions::UnsubscribeNeighbor:
			Unsubscribe(*mUpdateBuffer.first);
			break;
		case NodeActions::GenerateEvent:
		case NodeActions::Sleep:
		case NodeActions::Default:
			break;
		}

		mUpdateBuffer.first.reset();
		mUpdateBuffer = {};
	}
	CheckAndDestroy();
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


Node::NodePtr Node::SubscribeNeighbor(const NodePtr& Subscriber)
{
	if (Subscriber && Subscriber != shared_from_this())
		return Subscriber;
	return FindRandomNeighbor(mNeighbors, 1);
}

Node::NodePtr Node::UnsubscribeNeighbor(Node* Author) const
{
	if (Author)
		return NodePtr(Author);

	if (!mAuthors.empty())
	{
		const auto AuthorIndex = RandomGenerator::GenerateNumber(0, static_cast<int>(mAuthors.size()) - 1);
		if (!mAuthors[AuthorIndex].expired())
			return mAuthors[AuthorIndex].lock();
	}
	return nullptr;
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

template<typename C>
Node::NodePtr Node::FindRandomNeighbor(const C& Array, int Deep) const
{
	if (Array.empty() || Deep <= 0)
		return nullptr;
	{
		const auto NeighborIndex = RandomGenerator::GenerateNumber(0, static_cast<int>(Array.size()) - 1);
		const std::weak_ptr<Node> NeighborTemp = Array[NeighborIndex];
		if (!NeighborTemp.expired())
		{
			auto Neighbor = NeighborTemp.lock();
			auto NeighborsNeighbor = FindRandomNeighbor(Neighbor->GetNeighbors(), --Deep);
			if (!NeighborsNeighbor) 
				if (NeighborsNeighbor != shared_from_this())
					return NeighborsNeighbor;
			if (NeighborsNeighbor != shared_from_this())
				return Neighbor;
		}
		return nullptr;
	}
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

bool Node::IsBufferEmpty() const
{
	return mUpdateBuffer.first == nullptr;
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
