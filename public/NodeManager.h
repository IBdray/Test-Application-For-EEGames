#pragma once
#include <list>
#include <memory>

class Node;

class NodeManager
{
	using NodePtr = std::shared_ptr<Node>;

	// I use list instead of map because list do not need continuous memory for it's items
	static std::list<NodePtr> mNodes;

public:
	static void AddNode(const NodePtr& NodePtr);
	static void RemoveNode(const NodePtr& NodePtr);
	static const auto& GetNodes() {return mNodes;}

};

