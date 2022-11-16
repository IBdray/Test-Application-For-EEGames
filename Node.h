#pragma once

#include <vector>
#include <string>
#include <memory>

#include <list>
#include <unordered_map>

#include "ActionPreferences.h"
#include "NeighborsData.h"

enum class NodeActions;

// main reason of this class is to generate events and receive other node's events
// secondary is to be able to subscribe/unsubscribe to events of neighboring node
class Node : public std::enable_shared_from_this<Node>
{
	using WeakNodeIt = std::vector<std::weak_ptr<Node>>::const_iterator;
	using SharedNodeIt = std::vector<std::shared_ptr<Node>>::const_iterator;


	std::string mName;
	size_t mThisNodeIndex;
	bool mActive;
	bool mPendingKill;

	// TODO: refactor containers. maybe use unordered map or list instead of vector
	std::vector<std::weak_ptr<Node>> mNeighbors;
	std::vector<std::weak_ptr<Node>> mSubscribers;
	std::vector<std::shared_ptr<Node>> mSubscribedTo;

public:
	friend class NodeFactory;

	// Comparison operators
	friend bool operator== (const Node& Lhs, const Node& Rhs) {return Lhs == Rhs;}
	friend bool operator!= (const Node& Lhs, const Node& Rhs) {return Lhs != Rhs;}

	// TODO: move update method to CycleManager class as SRP recommends (void Update(const Node& NodeRef) - to update any and all nodes in single class)
	void Update(const bool Active = true);

	// Destructor, sort of
	void CheckAndKill();


	// === Actions ===

	// TODO: maybe create event class that would contain all subscribers (not sure it goes well) and event generation logic
	void GenerateEvent();

	// TODO: should i move subscription logic to separate class of no?
	void SubscribeToNeighbor();
	void SubscribeToNeighbor(const std::shared_ptr<Node>& Other);
	void UnsubscribeFromNeighbor();
	void UnsubscribeFromNeighbor(const std::shared_ptr<Node>& Other);

	// TODO: move it to factory class. Nodes shouldn't generate new nodes (?)
	std::shared_ptr<Node> GenerateNewNeighbor();

	// TODO: maybe it is better to move it to cycle manager, otherwise it is important part of the node
	static void SetPreferences(ActionPreferences Preferences);

	std::string GetName() const {return mName;}
	static ActionPreferences GetPreferences() {return mActionPreferences;}
	const std::vector<std::weak_ptr<Node>>& GetNeighbors() const {return mNeighbors;}

	bool CheckIsNeighbors(const std::shared_ptr<Node>& Other) const;
	bool CheckIsSubscribedTo(const std::shared_ptr<Node>& Other) const;
	bool CheckIsSubscriber(const std::shared_ptr<Node>& Other) const;
	

	// TODO: move default preferences to factory class and make unique to each node?
	static ActionPreferences mActionPreferences;

	// TODO: maybe unite with subscribed to array (add pointer to creator to NeighborsData struct)
	std::unordered_map<std::string, NeighborsData> NeighborsDataMap;

private:
	Node();

	template<typename T>
	Node(T&& Name, bool Active = true);



	std::shared_ptr<Node> SubscribeTo(const std::shared_ptr<Node>& Other);
	std::shared_ptr<Node> UnsubscribeFrom(const std::shared_ptr<Node>& Other);

	void AddSubscriber(const std::shared_ptr<Node>& Other);
	void RemoveSubscriber(const std::shared_ptr<Node>& Other);

	void BecomeNeighbors(const std::shared_ptr<Node>& Other);
	void CheckAndRemoveNeighbor(const std::shared_ptr<Node>& Other);


	void ReceiveEvent(int Value, const Node& Other);


	// TODO: refactor this
	bool CheckIsNodeInContainer(const std::shared_ptr<Node>& NodePtr, const std::vector<std::weak_ptr<Node>>& Container) const;
	bool CheckIsNodeInContainer(const std::shared_ptr<Node>& NodePtr, const std::vector<std::shared_ptr<Node>>& Container) const;

	WeakNodeIt FindNodeInContainer(const std::shared_ptr<Node>& NodePtr, const std::vector<std::weak_ptr<Node>>& Container) const;
	SharedNodeIt FindNodeInContainer(const std::shared_ptr<Node>& NodePtr, const std::vector<std::shared_ptr<Node>>& Container) const;
	


	// TODO: create random number generator class
	static int GenerateRandomNumber(const int Min = -1000, const int Max = 1000);


	// TODO: Refactor event handlers. Create event handler class and transfer all logic there
	void EventHandlerSum(const int Sum, const Node& Other);
	void EventHandlerNumberOfEvents(const Node& Other);

	// Counter for name generation
	static int mFactoryCounter;
};


class NodeFactory
{

	static std::list<std::shared_ptr<Node>> mNodesList;

public:
	template<typename... Ts>
	static std::shared_ptr<Node> Create(Ts&& ... Args)
	{
		std::shared_ptr<Node> NewNode(new Node(std::forward<Ts>(Args)...));
		mNodesList.emplace_back(NewNode);
		NewNode->mThisNodeIndex = mNodesList.size() - 1;

		return NewNode;
	}


	static const auto& GetNodes() {return mNodesList;}

	static void RemoveNode(size_t NodeIndex)
	{
		auto NodeIt = mNodesList.begin();
		std::advance(NodeIt, NodeIndex);
		NodeIt->reset();
	}

};