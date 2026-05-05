#include "var.h"

using namespace interbufc;

INTERBUFC_API AstNodePtr<AstNode> VarNode::do_duplicate(peff::Alloc *new_allocator) const {
	bool succeeded = false;
	AstNodePtr<VarNode> duplicated_node(make_ast_node<VarNode>(new_allocator, *this, new_allocator, succeeded));
	if ((!duplicated_node) || (!succeeded)) {
		return {};
	}

	return duplicated_node.cast_to<AstNode>();
}

INTERBUFC_API VarNode::VarNode(
	peff::Alloc *self_allocator,
	const AstNodePtr<Document> &document)
	: MemberNode(AstNodeType::Var, self_allocator, document) {
}

INTERBUFC_API VarNode::VarNode(const VarNode &rhs, peff::Alloc *allocator, bool &succeeded_out) : MemberNode(rhs, allocator, succeeded_out) {
	if (!succeeded_out) {
		return;
	}

	if (!(type = rhs.type->duplicate<TypeNameNode>(allocator))) {
		succeeded_out = false;
		return;
	}

	succeeded_out = true;
}

INTERBUFC_API VarNode::~VarNode() {
}
