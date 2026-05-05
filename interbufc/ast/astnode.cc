#include "astnode.h"
#include "document.h"

using namespace interbufc;

INTERBUFC_API AstNode::AstNode(AstNodeType ast_node_type, peff::Alloc *self_allocator, const AstNodePtr<Document> &document) : ast_node_type(ast_node_type), self_allocator(self_allocator), document(document.get()) {
	assert(document);
	document->clear_deferred_destructible_ast_nodes();
}

INTERBUFC_API AstNode::AstNode(const AstNode &other, peff::Alloc *new_allocator) {
	other.document->clear_deferred_destructible_ast_nodes();
	document = other.document;
	self_allocator = new_allocator;
	ast_node_type = other.ast_node_type;
	token_range = other.token_range;
}

INTERBUFC_API AstNode::~AstNode() {
}

INTERBUFC_API AstNodePtr<AstNode> AstNode::do_duplicate(peff::Alloc *new_allocator) const {
	std::terminate();
}

INTERBUFC_API void interbufc::add_ast_node_to_destructible_list(AstNode *ast_node, AstNodeDestructor destructor) {
	ast_node->next_destructible = ast_node->document->destructible_ast_node_list;
	ast_node->destructor = destructor;
	ast_node->document->destructible_ast_node_list = ast_node;
}
