#include "typename_base.h"

using namespace interbufc;

INTERBUFC_API TypeNameNode::TypeNameNode(TypeNameKind type_name_kind, peff::Alloc *self_allocator, const AstNodePtr<Document> &document) : AstNode(AstNodeType::TypeName, self_allocator, document), type_name_kind(type_name_kind) {
}

INTERBUFC_API TypeNameNode::TypeNameNode(const TypeNameNode &rhs, peff::Alloc *self_allocator) : AstNode(rhs, self_allocator), type_name_kind(rhs.type_name_kind) {
}

INTERBUFC_API TypeNameNode::~TypeNameNode() {
}
