#include "import.h"

using namespace interbufc;

INTERBUFC_API AstNodePtr<AstNode> ImportNode::do_duplicate(peff::Alloc *new_allocator) const {
	bool succeeded = false;
	AstNodePtr<ImportNode> duplicated_node(make_ast_node<ImportNode>(new_allocator, *this, new_allocator, succeeded));
	if ((!duplicated_node) || (!succeeded)) {
		return {};
	}

	return duplicated_node.cast_to<AstNode>();
}

INTERBUFC_API ImportNode::ImportNode(
	peff::Alloc *self_allocator,
	const AstNodePtr<Document> &document)
	: MemberNode(AstNodeType::Import, self_allocator, document) {
}

INTERBUFC_API ImportNode::ImportNode(const ImportNode &rhs, peff::Alloc *allocator, bool &succeeded_out) : MemberNode(rhs, allocator, succeeded_out) {
	if (!succeeded_out) {
		return;
	}

	if (!(id_ref = duplicate_id_ref(allocator, rhs.id_ref.get()))) {
		succeeded_out = false;
		return;
	}

	succeeded_out = true;
}

INTERBUFC_API ImportNode::~ImportNode() {
}
