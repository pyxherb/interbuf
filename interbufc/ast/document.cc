#include "document.h"

using namespace interbufc;

INTERBUFC_API Document::Document(peff::Alloc *allocator): allocator(allocator), externalModuleProviders(allocator) {
}

INTERBUFC_API Document::~Document() {
	rootModule.reset();
	_doClearDeferredDestructibleAstNodes();
}

INTERBUFC_API void Document::_doClearDeferredDestructibleAstNodes() {
	AstNode *i, *next;

	while ((i = destructibleAstNodeList)) {
		destructibleAstNodeList = nullptr;

		while (i) {
			next = i->nextDestructible;
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
	if ((storedError = typeNameListCmp(lhs, rhs, result))) {
		return false;
	}
	return result < 0;
}*/
