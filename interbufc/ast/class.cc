#include "class.h"

using namespace interbufc;

INTERBUFC_API ClassNode::ClassNode(
	peff::Alloc *selfAllocator,
	const AstNodePtr<Document> &document)
	: ModuleNode(selfAllocator, document, AstNodeType::Class) {
}

INTERBUFC_API ClassNode::~ClassNode() {
}

INTERBUFC_API StructNode::StructNode(
	peff::Alloc *selfAllocator,
	const AstNodePtr<Document> &document)
	: ModuleNode(selfAllocator, document, AstNodeType::Struct) {
}

INTERBUFC_API StructNode::~StructNode() {
}

INTERBUFC_API EnumNode::EnumNode(
	peff::Alloc *selfAllocator,
	const AstNodePtr<Document> &document)
	: ModuleNode(selfAllocator, document, AstNodeType::Enum) {
}

INTERBUFC_API EnumNode::~EnumNode() {
}

INTERBUFC_API EnumItemNode::EnumItemNode(
	peff::Alloc *selfAllocator,
	const AstNodePtr<Document> &document)
	: MemberNode(AstNodeType::EnumItem, selfAllocator, document) {
}

INTERBUFC_API EnumItemNode::~EnumItemNode() {
}
