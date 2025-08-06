#include "typename_base.h"

using namespace interbufc;

INTERBUFC_API TypeNameNode::TypeNameNode(TypeNameKind typeNameKind, peff::Alloc *selfAllocator, const AstNodePtr<Document> &document) : AstNode(AstNodeType::TypeName, selfAllocator, document), typeNameKind(typeNameKind) {
}

INTERBUFC_API TypeNameNode::TypeNameNode(const TypeNameNode &rhs, peff::Alloc *selfAllocator) : AstNode(rhs, selfAllocator), typeNameKind(rhs.typeNameKind) {
}

INTERBUFC_API TypeNameNode::~TypeNameNode() {
}
