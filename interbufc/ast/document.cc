#include "document.h"

using namespace interbufc;

INTERBUFC_API Document::Document(peff::Alloc *allocator): allocator(allocator), external_module_providers(allocator) {
}

INTERBUFC_API Document::~Document() {
	root_module.reset();
	_do_clear_deferred_destructible_ast_nodes();
}

INTERBUFC_API void Document::_do_clear_deferred_destructible_ast_nodes() {
	AstNode *i, *next;

	while ((i = destructible_ast_node_list)) {
		destructible_ast_node_list = nullptr;

		while (i) {
			next = i->next_destructible;
			i->destructor(i);
			i = next;
		};
	}
}

/*
INTERBUFC_API bool TypeNameListCmp::operator()(const peff::DynArray<AstNodePtr<TypeNameNode>> &lhs, const peff::DynArray<AstNodePtr<TypeNameNode>> &rhs) const noexcept {
	int result;
	// Note that we just need one critical error to notify the compiler
	// that we have encountered errors that will force the compilation
	// to be interrupted.
	if ((stored_error = type_name_list_cmp(lhs, rhs, result))) {
		return false;
	}
	return result < 0;
}*/
