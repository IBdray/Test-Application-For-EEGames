#pragma once

#include <memory>
#include <string>
#include <vector>

#include <list>
#include <unordered_map>

#include "ActionPreferences.h"
#include "NeighborsData.h"

enum class NodeActions;

template<typename T>
struct EventBase;

// main reason of this class is to generate events and receive other node's events
// secondary is to be able to subscribe/unsubscribe to events of neighboring node
class Node : public std::enable_shared_from_this<Node>
{
	using NodePtr = std::shared_ptr<Node>;


	std::string mName;
	bool mActive;

	std::vector<std::weak_ptr<Node>> mNeighbors;
	std::vector<std::shared_ptr<Node>> mSubscribers;
	std::vector<std::weak_ptr<Node>> mAuthors;

	// TODO: maybe unite with subscribed to array (add pointer to creator to NeighborsData struct)
	std::unordered_map<std::string, NeighborsData> mNeighborsDataMap;
	static ActionPreferences mActionPreferences;

public:
	struct Factory
	{
		// TODO: fix memory limit issue for amount of nodes (maybe with exception handling)

		template<typename... Ts>
		static NodePtr CreateNode(Ts... Args);
		template<typename... Ts>
		static NodePtr CreateNeighborTo(const NodePtr& ParentNode, Ts... Args);
	};

	struct Manager
	{
		static const auto& GetNodes() {return mNodesList;}
		static void AddNode(const NodePtr& NodePtr);
		static void RemoveNode(const NodePtr& NodePtr);

	private:
		// I use list instead of map because list do not need continuous memory for it's items
		static std::list<NodePtr> mNodesList;

	};

	// TODO: move update method to CycleManager class as SRP recommends (void Update(const Node& NodeRef) - to update any and all nodes in single class)
	void Update(const bool Active = true);

	// === Actions ===
	void GenerateEvent() const;

	template<typename T>
	void ReceiveEvent(const EventBase<T>&, const Node& Other);


	void SubscribeNeighbor(const std::shared_ptr<Node>& Subscriber = nullptr);
	void UnsubscribeNeighbor(Node* Neighbor = nullptr);

	// TODO: maybe it is better to move it to cycle manager, otherwise it is important part of the node
	static void SetPreferences(ActionPreferences Preferences);

	std::string GetName() const {return mName;}
	static ActionPreferences GetPreferences() {return mActionPreferences;}
	auto& GetNeighbors() const {return mNeighbors;}
	auto& GetNeighborsData() const {return mNeighborsDataMap;}


	bool IsNeighbors(const Node& Neighbor) const;
	bool IsAuthor(const Node& Author) const;
	bool IsSubscriber(const Node& Subscriber) const;


	friend bool operator==(const Node& Lhs, const Node& Rhs)
	{
		return Lhs.GetName() == Rhs.GetName();
	}
	friend bool operator!=(const Node& Lhs, const Node& Rhs)
	{
		return !(Lhs == Rhs);
	}

private:
	Node();
	template<typename T>
	Node(T&& Name, bool Active = true);
	void Destroy();


	void Subscribe(const std::shared_ptr<Node>& Subscriber);
	void AddAuthor(const std::shared_ptr<Node>& Author);
	void AddNeighbor(const std::shared_ptr<Node>& Neighbor);

	void Unsubscribe(Node& Subscriber);
	void RemoveAuthor(const Node& Author);
	void RemoveNeighbor(const Node& Neighbor);


	template<typename C>
	auto FindNodeInContainer(const Node& NodeRef, C& Container) const -> decltype(std::begin(Container));
	template<typename I, typename C>
	void ResetAndEraseNode(const I& It, C& Container);

};