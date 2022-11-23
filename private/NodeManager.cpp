#include "../public/NodeManager.h"
#include "../public/Node.h"

std::list<std::shared_ptr<Node>> NodeManager::mNodes {};

void NodeManager::AddNode(const NodePtr& NodePtr)
{
	if (NodePtr && std::find(mNodes.cbegin(),mNodes.cend(), NodePtr) == mNodes.cend())
		mNodes.emplace_back(NodePtr);
}

void NodeManager::RemoveNode(const NodePtr& NodePtr)
{
	if (NodePtr)
		std::find(mNodes.begin(),mNodes.end(), NodePtr)->reset();
}
