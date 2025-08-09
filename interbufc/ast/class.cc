#include "class.h"

using namespace interbufc;

INTERBUFC_API AstNodePtr<AstNode> ClassNode::doDuplicate(peff::Alloc *newAllocator) const {
	bool succeeded = false;
	AstNodePtr<ClassNode> duplicatedNode(makeAstNode<ClassNode>(newAllocator, *this, newAllocator, succeeded));
	if ((!duplicatedNode) || (!succeeded)) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API ClassNode::ClassNode(
	peff::Alloc *selfAllocator,
	const AstNodePtr<Document> &document)
	: ModuleNode(selfAllocator, document, AstNodeType::Class) {
}

INTERBUFC_API ClassNode::ClassNode(const ClassNode &rhs, peff::Alloc *allocator, bool &succeededOut) : ModuleNode(rhs, allocator, succeededOut) {
	if (!succeededOut) {
		return;
	}

	succeededOut = true;
}

INTERBUFC_API ClassNode::~ClassNode() {
}

INTERBUFC_API AstNodePtr<AstNode> StructNode::doDuplicate(peff::Alloc *newAllocator) const {
	bool succeeded = false;
	AstNodePtr<StructNode> duplicatedNode(makeAstNode<StructNode>(newAllocator, *this, newAllocator, succeeded));
	if ((!duplicatedNode) || (!succeeded)) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API StructNode::StructNode(
	peff::Alloc *selfAllocator,
	const AstNodePtr<Document> &document)
	: ModuleNode(selfAllocator, document, AstNodeType::Struct) {
}

INTERBUFC_API StructNode::StructNode(const StructNode &rhs, peff::Alloc *allocator, bool &succeededOut) : ModuleNode(rhs, allocator, succeededOut) {
	if (!succeededOut) {
		return;
	}

	succeededOut = true;
}

INTERBUFC_API StructNode::~StructNode() {
}
