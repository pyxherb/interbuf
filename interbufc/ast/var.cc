#include "var.h"

using namespace interbufc;

INTERBUFC_API AstNodePtr<AstNode> VarNode::doDuplicate(peff::Alloc *newAllocator) const {
	bool succeeded = false;
	AstNodePtr<VarNode> duplicatedNode(makeAstNode<VarNode>(newAllocator, *this, newAllocator, succeeded));
	if ((!duplicatedNode) || (!succeeded)) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API VarNode::VarNode(
	peff::Alloc *selfAllocator,
	const AstNodePtr<Document> &document)
	: MemberNode(AstNodeType::Var, selfAllocator, document) {
}

INTERBUFC_API VarNode::VarNode(const VarNode &rhs, peff::Alloc *allocator, bool &succeededOut) : MemberNode(rhs, allocator, succeededOut) {
	if (!succeededOut) {
		return;
	}

	if (!(type = rhs.type->duplicate<TypeNameNode>(allocator))) {
		succeededOut = false;
		return;
	}

	isTypeDeducedFromInitialValue = rhs.isTypeDeducedFromInitialValue;
	isTypeSealed = rhs.isTypeSealed;
	idxReg = rhs.idxReg;

	succeededOut = true;
}

INTERBUFC_API VarNode::~VarNode() {
}
