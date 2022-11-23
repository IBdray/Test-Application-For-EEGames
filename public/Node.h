#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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
};


class Node : public std::enable_shared_from_this<Node>
{
	using NodePtr = std::shared_ptr<Node>;


	std::string mName;
	bool mActive;

	std::vector<std::weak_ptr<Node>> mNeighbors;
	std::vector<NodePtr> mSubscribers;
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

	friend bool operator==(const Node& Lhs, const Node& Rhs);
	friend bool operator!=(const Node& Lhs, const Node& Rhs);


	void Update();
	bool CheckAndDestroy();

	void GenerateEvent() const;
	template<typename T>
	void ReceiveEvent(const EventBase<T>&, const Node& Other);

	void SubscribeNeighbor(const NodePtr& Subscriber = nullptr);
	void UnsubscribeNeighbor(Node* Neighbor = nullptr);


	static ActionPreferences GetPreferences() {return mActionPreferences;}
	static void SetPreferences(ActionPreferences&& Preferences);

	std::string GetName() const {return mName;}
	auto& GetNeighbors() const {return mNeighbors;}
	auto& GetSubscribers() const {return mSubscribers;}
	auto& GetAuthors() const {return mAuthors;}
	auto& GetAuthorsData() const {return mAuthorsData;}


	bool IsNeighbors(const Node& Neighbor) const;
	bool IsAuthor(const Node& Author) const;
	bool IsSubscriber(const Node& Subscriber) const;	

private:
	Node();
	template<typename T>
	Node(T&& Name, bool Active = true);


	void Subscribe(const NodePtr& Subscriber);
	void AddAuthor(const NodePtr& Author);
	void AddNeighbor(const NodePtr& Neighbor);

	void Unsubscribe(Node& Subscriber);
	void RemoveAuthor(const Node& Author);
	void RemoveNeighbor(const Node& Neighbor);

	void SetEventHandler(const NodePtr& Author);


	template<typename C>
	auto FindNodeInContainer(const Node& NodeRef, C& Container) const -> decltype(std::begin(Container));
	template<typename I, typename C>
	void ResetAndEraseNode(const I& It, C& Container);

};


inline bool operator==(const Node& Lhs, const Node& Rhs)
{
	return Lhs.GetName() == Rhs.GetName();
}

inline bool operator!=(const Node& Lhs, const Node& Rhs)
{
	return !(Lhs == Rhs);
}
