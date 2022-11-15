#pragma once

#include <vector>
#include <string>
#include <memory>

#include <list>
#include <unordered_map>

#include "ActionPreferences.h"
#include "NeighborsData.h"


enum class NodeActions;


class Node : public std::enable_shared_from_this<Node>
{
	using WeakNodeIt = std::vector<std::weak_ptr<Node>>::const_iterator;
	using SharedNodeIt = std::vector<std::shared_ptr<Node>>::const_iterator;


public:
	template<typename... Ts>
	static std::shared_ptr<Node> Create(Ts&&... Args);


	friend bool operator== (const Node& Lhs, const Node& Rhs) {return Lhs == Rhs;}
	friend bool operator!= (const Node& Lhs, const Node& Rhs) {return Lhs != Rhs;}

	void Update();
	void CheckAndKill();

	// === Actions ===
	void GenerateEvent();
	void SubscribeToNeighbor();
	void SubscribeToNeighbor(const std::shared_ptr<Node>& Other);
	void UnsubscribeFromNeighbor();
	void UnsubscribeFromNeighbor(const std::shared_ptr<Node>& Other);
	std::shared_ptr<Node> GenerateNewNeighbor();



	std::string GetName() const {return mName;}
	ActionPreferences GetActionPreferences() const {return mActionPreferences;}
	const std::vector<std::weak_ptr<Node>>& GetNeighbors() const {return mNeighbors;}

	bool CheckIsNeighbors(const std::shared_ptr<Node>& Other) const;
	bool CheckIsSubscribedTo(const std::shared_ptr<Node>& Other) const;
	bool CheckIsSubscriber(const std::shared_ptr<Node>& Other) const;

	static std::list<std::shared_ptr<Node>> Nodes;
	size_t ThisNodeIndex;

	std::unordered_map<std::string, NeighborsData> NeighborsDataMap;

private:
	Node();

	template<typename T>
	Node(T&& Name, ActionPreferences Preferences, bool Active = true);

	


	std::shared_ptr<Node> SubscribeTo(const std::shared_ptr<Node>& Other);
	std::shared_ptr<Node> UnsubscribeFrom(const std::shared_ptr<Node>& Other);

	void AddSubscriber(const std::shared_ptr<Node>& Other);
	void RemoveSubscriber(const std::shared_ptr<Node>& Other);

	void BecomeNeighbors(const std::shared_ptr<Node>& Other);
	void CheckAndRemoveNeighbor(const std::shared_ptr<Node>& Other);


	void ReceiveEvent(int Value, const Node& Other);

	
	bool CheckIsNodeInContainer(const std::shared_ptr<Node>& NodePtr, const std::vector<std::weak_ptr<Node>>& Container) const;
	bool CheckIsNodeInContainer(const std::shared_ptr<Node>& NodePtr, const std::vector<std::shared_ptr<Node>>& Container) const;

	WeakNodeIt FindNodeInContainer(const std::shared_ptr<Node>& NodePtr, const std::vector<std::weak_ptr<Node>>& Container) const;
	SharedNodeIt FindNodeInContainer(const std::shared_ptr<Node>& NodePtr, const std::vector<std::shared_ptr<Node>>& Container) const;
	



	static int GenerateRandomNumber(const int Min = -1000, const int Max = 1000);


	//TODO: Refactor event handlers. Create event handler class and transfer all logic there
	void EventHandlerSum(const int Sum, const Node& Other);
	void EventHandlerNumberOfEvents(const Node& Other);


	static int mFactoryCounter;

	std::string mName;
	ActionPreferences mActionPreferences;


	std::vector<std::weak_ptr<Node>> mNeighbors;
	std::vector<std::weak_ptr<Node>> mSubscribers;
	std::vector<std::shared_ptr<Node>> mSubscribedTo;


	bool mActive;
	bool mPendingKill;


};
