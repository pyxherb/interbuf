#include "class.h"

using namespace interbufc;

INTERBUFC_API ClassNode::ClassNode(
	peff::Alloc *self_allocator,
	const AstNodePtr<Document> &document)
	: ModuleNode(self_allocator, document, AstNodeType::Class) {
}

INTERBUFC_API ClassNode::~ClassNode() {
}

INTERBUFC_API StructNode::StructNode(
	peff::Alloc *self_allocator,
	const AstNodePtr<Document> &document)
	: ModuleNode(self_allocator, document, AstNodeType::Struct) {
}

INTERBUFC_API StructNode::~StructNode() {
}

INTERBUFC_API EnumNode::EnumNode(
	peff::Alloc *self_allocator,
	const AstNodePtr<Document> &document)
	: ModuleNode(self_allocator, document, AstNodeType::Enum) {
}

INTERBUFC_API EnumNode::~EnumNode() {
}

INTERBUFC_API EnumItemNode::EnumItemNode(
	peff::Alloc *self_allocator,
	const AstNodePtr<Document> &document)
	: MemberNode(AstNodeType::EnumItem, self_allocator, document) {
}

INTERBUFC_API EnumItemNode::~EnumItemNode() {
}
