#include "NodeFactory.h"
#include "Node.h"


//std::list<std::shared_ptr<Node>> NodeFactory::mNodesList {};
//
//template<typename ... Ts>
//std::shared_ptr<Node> NodeFactory::Create(Ts&&... Args)
//{
//	std::shared_ptr<Node> NewNode(new Node(std::forward<Ts>(Args)...));
//	mNodesList.emplace_back(NewNode);
//	NewNode->mThisNodeIndex = mNodesList.size() - 1;
//
//	return NewNode;
//}
////std::shared_ptr<Node> NodeFactory::Create()
////{
////	auto NewNode = std::make_shared<Node>();
////	mNodesList.emplace_back(NewNode);
////	NewNode->mThisNodeIndex = mNodesList.size() - 1;
////
////	return NewNode;
////}
//
//void NodeFactory::RemoveNode(size_t NodeIndex)
//{
//	auto NodeIt = mNodesList.begin();
//	std::advance(NodeIt, NodeIndex);
//	NodeIt->reset();
//}
