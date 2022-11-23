#pragma once

#include <memory>
#include <string>
#include <vector>

#include <list>
#include <unordered_map>

#include "ActionPreferences.h"

template<typename T>
struct EventBase;
struct EventHandler;
class Node;

struct AuthorsData
{
	int Sum = 0;
	int Counter = 0;
	std::unique_ptr<EventHandler> HandlerPtr;
	std::weak_ptr<Node> AuthorPtr;

	AuthorsData() = default;
};


class Node : public std::enable_shared_from_this<Node>
{
	using NodePtr = std::shared_ptr<Node>;


	std::string mName;
	bool mActive;

	std::vector<std::weak_ptr<Node>> mNeighbors;
	std::vector<std::shared_ptr<Node>> mSubscribers;
	std::vector<std::weak_ptr<Node>> mAuthors;

	// Authors Data map will serve as additional data
	std::unordered_map<std::string, AuthorsData> mAuthorsData;
	static ActionPreferences mActionPreferences;

public:
	struct Factory
	{
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
	auto& GetNeighborsData() const {return mAuthorsData;}


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

	void SetEventHandler(const std::shared_ptr<Node>& Author);


	template<typename C>
	auto FindNodeInContainer(const Node& NodeRef, C& Container) const -> decltype(std::begin(Container));
	template<typename I, typename C>
	void ResetAndEraseNode(const I& It, C& Container);

};