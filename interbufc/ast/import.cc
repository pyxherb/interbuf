#include "import.h"

using namespace interbufc;

INTERBUFC_API AstNodePtr<AstNode> ImportNode::doDuplicate(peff::Alloc *newAllocator) const {
	bool succeeded = false;
	AstNodePtr<ImportNode> duplicatedNode(makeAstNode<ImportNode>(newAllocator, *this, newAllocator, succeeded));
	if ((!duplicatedNode) || (!succeeded)) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API ImportNode::ImportNode(
	peff::Alloc *selfAllocator,
	const AstNodePtr<Document> &document)
	: MemberNode(AstNodeType::Import, selfAllocator, document) {
}

INTERBUFC_API ImportNode::ImportNode(const ImportNode &rhs, peff::Alloc *allocator, bool &succeededOut) : MemberNode(rhs, allocator, succeededOut) {
	if (!succeededOut) {
		return;
	}

	if (!(idRef = duplicateIdRef(allocator, rhs.idRef.get()))) {
		succeededOut = false;
		return;
	}

	succeededOut = true;
}

INTERBUFC_API ImportNode::~ImportNode() {
}
