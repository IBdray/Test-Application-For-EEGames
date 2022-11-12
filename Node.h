#pragma once

#include "vector"
#include "string"
#include "memory"

#include "Preferences.h"

class Node : public std::enable_shared_from_this<Node>
{
public:
	template<typename... Ts>
	static std::shared_ptr<Node> Create(Ts&&... Args);

	/*friend bool operator== (const Node& Lhs, const Node& Rhs);
	friend bool operator!= (const Node& Lhs, const Node& Rhs);*/

	void Update();

	void CreateEvent();

	void SubscribeTo(std::shared_ptr<Node> Other);
	void AddSubscriber(std::shared_ptr<Node> Other);

	void ReceiveEvent(int Value, const Node& Other);

	std::shared_ptr<Node> GenerateNewNeighbor();


	std::string GetName() const {return mName;}
	const std::vector<std::shared_ptr<Node>>& GetSubscribedToArray() const {return mSubscribedTo;}
	const std::vector<std::weak_ptr<Node>>& GetSubscribersArray() const {return mSubscribers;}

	

private:
	Node();

	template<typename T>
	Node(T&& Name, Preferences Preferences);



	int GenerateRandomNumber();


	//TODO: Refactor event handlers. Create event handler class and transfer all logic there
	void EventHandlerSum(const Node& Other);
	void EventHandlerNumberOfEvents(const Node& Other);


	static int mFactoryCounter;

	std::string mName;
	Preferences mPreferences;

	std::vector<std::weak_ptr<Node>> mNeighbors;

	std::vector<std::weak_ptr<Node>> mSubscribers;
	std::vector<std::shared_ptr<Node>> mSubscribedTo;

};

