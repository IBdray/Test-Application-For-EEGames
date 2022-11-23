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

void NodeManager::UpdateNodes()
{
	auto It = mNodes.cbegin();
	const auto End = mNodes.cend();

	while (It != End)
	{
		auto& Item = *It;
		if (Item)
		{
			Item->Update();
			++It;
		}
		else
			It = mNodes.erase(It);
	}

	for (const auto& Item : mNodes)
		if (Item)
			Item->SubmitUpdate();
}
