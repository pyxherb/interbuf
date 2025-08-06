#include "astnode.h"
#include "document.h"

using namespace interbufc;

INTERBUFC_API AstNode::AstNode(AstNodeType astNodeType, peff::Alloc *selfAllocator, const AstNodePtr<Document> &document) : astNodeType(astNodeType), selfAllocator(selfAllocator), document(document) {
	assert(document);
	document->clearDeferredDestructibleAstNodes();
}

INTERBUFC_API AstNode::AstNode(const AstNode &other, peff::Alloc *newAllocator) {
	other.document->clearDeferredDestructibleAstNodes();
	document = other.document;
	selfAllocator = newAllocator;
	astNodeType = other.astNodeType;
	tokenRange = other.tokenRange;
}

INTERBUFC_API AstNode::~AstNode() {
}

INTERBUFC_API AstNodePtr<AstNode> AstNode::doDuplicate(peff::Alloc *newAllocator) const {
	std::terminate();
}

INTERBUFC_API void interbufc::addAstNodeToDestructibleList(AstNode *astNode, AstNodeDestructor destructor) {
	astNode->nextDestructible = astNode->document->destructibleAstNodeList;
	astNode->destructor = destructor;
	astNode->document->destructibleAstNodeList = astNode;
}
