#include "Node.h"

#include "iostream"

int Node::mFactoryCounter = 0;


template<typename ... Ts>
std::shared_ptr<Node> Node::Create(Ts&&... Args)
{
	//std::make_shared throws errors so this is a work around
	return std::shared_ptr<Node>(new Node(std::forward<Ts>(Args)...));
}


Node::Node()
{
	this->mName = "Node_" + std::to_string(mFactoryCounter++);
	this->mPreferences = Preferences();
}

template<typename T>
Node::Node(T&&Name, Preferences Preferences)
	: mName(std::forward<T>(Name)), mPreferences(Preferences)
{
}


std::shared_ptr<Node> Node::GenerateNewNeighbor()
{
	auto Temp = Create();
	SubscribeTo(Temp);
	return Temp;
}



void Node::SubscribeTo(std::shared_ptr<Node> Other)
{
	if (Other != shared_from_this() && std::find(mSubscribedTo.cbegin(), mSubscribedTo.cend(), Other) == mSubscribedTo.cend())
	{
		mSubscribedTo.emplace_back(Other);
		Other->AddSubscriber(shared_from_this());

		//if (std::find(mNeighbors.cbegin(), mNeighbors.cend(), Other) == mNeighbors.cend())
			//mNeighbors.emplace_back(Other);

	}
}

void Node::AddSubscriber(std::shared_ptr<Node> Other)
{
	if (Other != shared_from_this() )//&& std::find(mSubscribers.cbegin(), mSubscribers.cend(), Other) == mSubscribers.cend())
	{
		mSubscribers.emplace_back(Other);

		//if (std::find(mNeighbors.cbegin(), mNeighbors.cend(), Other) == mNeighbors.cend())
			//mNeighbors.emplace_back(Other);
	}
}




void Node::Update()
{
	
}

void Node::CreateEvent()
{
	/*auto EventValue = GenerateRandomNumber();
	for (auto Subscriber : mSubscribers)
	{
		Subscriber.ReceiveEvent(EventValue);
	}*/
}

void Node::ReceiveEvent(int Value, const Node& Other)
{
	//// TODO: Save value to member variable
	//EventHandlerSum(Other);
}





int Node::GenerateRandomNumber()
{
	return 0;//std::rand();
}

void Node::EventHandlerSum(const Node& Other)
{
	std::cout<<mName<<"->"<<Other.mName<<": "<<"Sum";
}

void Node::EventHandlerNumberOfEvents(const Node& Other)
{
	std::cout<<mName<<"->"<<Other.mName<<": "<<"Num";
}
