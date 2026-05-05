#include "typename.h"

using namespace interbufc;

INTERBUFC_API AstNodePtr<AstNode> I8TypeNameNode::do_duplicate(peff::Alloc *new_allocator) const {
	AstNodePtr<I8TypeNameNode> duplicated_node(make_ast_node<I8TypeNameNode>(new_allocator, *this, new_allocator));
	if(!duplicated_node) {
		return {};
	}

	return duplicated_node.cast_to<AstNode>();
}

INTERBUFC_API I8TypeNameNode::I8TypeNameNode(peff::Alloc *self_allocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::I8, self_allocator, document) {
}

INTERBUFC_API I8TypeNameNode::I8TypeNameNode(const I8TypeNameNode &rhs, peff::Alloc *self_allocator) : TypeNameNode(rhs, self_allocator) {
}

INTERBUFC_API I8TypeNameNode::~I8TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> I16TypeNameNode::do_duplicate(peff::Alloc *new_allocator) const {
	AstNodePtr<I16TypeNameNode> duplicated_node(make_ast_node<I16TypeNameNode>(new_allocator, *this, new_allocator));
	if(!duplicated_node) {
		return {};
	}

	return duplicated_node.cast_to<AstNode>();
}

INTERBUFC_API I16TypeNameNode::I16TypeNameNode(peff::Alloc *self_allocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::I16, self_allocator, document) {
}

INTERBUFC_API I16TypeNameNode::I16TypeNameNode(const I16TypeNameNode &rhs, peff::Alloc *self_allocator) : TypeNameNode(rhs, self_allocator) {
}

INTERBUFC_API I16TypeNameNode::~I16TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> I32TypeNameNode::do_duplicate(peff::Alloc *new_allocator) const {
	AstNodePtr<I32TypeNameNode> duplicated_node(make_ast_node<I32TypeNameNode>(new_allocator, *this, new_allocator));
	if(!duplicated_node) {
		return {};
	}

	return duplicated_node.cast_to<AstNode>();
}

INTERBUFC_API I32TypeNameNode::I32TypeNameNode(peff::Alloc *self_allocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::I32, self_allocator, document) {
}

INTERBUFC_API I32TypeNameNode::I32TypeNameNode(const I32TypeNameNode &rhs, peff::Alloc *self_allocator) : TypeNameNode(rhs, self_allocator) {
}

INTERBUFC_API I32TypeNameNode::~I32TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> I64TypeNameNode::do_duplicate(peff::Alloc *new_allocator) const {
	AstNodePtr<I64TypeNameNode> duplicated_node(make_ast_node<I64TypeNameNode>(new_allocator, *this, new_allocator));
	if(!duplicated_node) {
		return {};
	}

	return duplicated_node.cast_to<AstNode>();
}

INTERBUFC_API I64TypeNameNode::I64TypeNameNode(peff::Alloc *self_allocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::I64, self_allocator, document) {
}

INTERBUFC_API I64TypeNameNode::I64TypeNameNode(const I64TypeNameNode &rhs, peff::Alloc *self_allocator) : TypeNameNode(rhs, self_allocator) {
}

INTERBUFC_API I64TypeNameNode::~I64TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> U8TypeNameNode::do_duplicate(peff::Alloc *new_allocator) const {
	AstNodePtr<U8TypeNameNode> duplicated_node(make_ast_node<U8TypeNameNode>(new_allocator, *this, new_allocator));
	if(!duplicated_node) {
		return {};
	}

	return duplicated_node.cast_to<AstNode>();
}

INTERBUFC_API U8TypeNameNode::U8TypeNameNode(peff::Alloc *self_allocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::U8, self_allocator, document) {
}

INTERBUFC_API U8TypeNameNode::U8TypeNameNode(const U8TypeNameNode &rhs, peff::Alloc *self_allocator) : TypeNameNode(rhs, self_allocator) {
}

INTERBUFC_API U8TypeNameNode::~U8TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> U16TypeNameNode::do_duplicate(peff::Alloc *new_allocator) const {
	AstNodePtr<U16TypeNameNode> duplicated_node(make_ast_node<U16TypeNameNode>(new_allocator, *this, new_allocator));
	if(!duplicated_node) {
		return {};
	}

	return duplicated_node.cast_to<AstNode>();
}

INTERBUFC_API U16TypeNameNode::U16TypeNameNode(peff::Alloc *self_allocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::U16, self_allocator, document) {
}

INTERBUFC_API U16TypeNameNode::U16TypeNameNode(const U16TypeNameNode &rhs, peff::Alloc *self_allocator) : TypeNameNode(rhs, self_allocator) {
}

INTERBUFC_API U16TypeNameNode::~U16TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> U32TypeNameNode::do_duplicate(peff::Alloc *new_allocator) const {
	AstNodePtr<U32TypeNameNode> duplicated_node(make_ast_node<U32TypeNameNode>(new_allocator, *this, new_allocator));
	if(!duplicated_node) {
		return {};
	}

	return duplicated_node.cast_to<AstNode>();
}

INTERBUFC_API U32TypeNameNode::U32TypeNameNode(peff::Alloc *self_allocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::U32, self_allocator, document) {
}

INTERBUFC_API U32TypeNameNode::U32TypeNameNode(const U32TypeNameNode &rhs, peff::Alloc *self_allocator) : TypeNameNode(rhs, self_allocator) {
}

INTERBUFC_API U32TypeNameNode::~U32TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> U64TypeNameNode::do_duplicate(peff::Alloc *new_allocator) const {
	AstNodePtr<U64TypeNameNode> duplicated_node(make_ast_node<U64TypeNameNode>(new_allocator, *this, new_allocator));
	if(!duplicated_node) {
		return {};
	}

	return duplicated_node.cast_to<AstNode>();
}

INTERBUFC_API U64TypeNameNode::U64TypeNameNode(peff::Alloc *self_allocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::U64, self_allocator, document) {
}

INTERBUFC_API U64TypeNameNode::U64TypeNameNode(const U64TypeNameNode &rhs, peff::Alloc *self_allocator) : TypeNameNode(rhs, self_allocator) {
}

INTERBUFC_API U64TypeNameNode::~U64TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> F32TypeNameNode::do_duplicate(peff::Alloc *new_allocator) const {
	AstNodePtr<F32TypeNameNode> duplicated_node(make_ast_node<F32TypeNameNode>(new_allocator, *this, new_allocator));
	if(!duplicated_node) {
		return {};
	}

	return duplicated_node.cast_to<AstNode>();
}

INTERBUFC_API F32TypeNameNode::F32TypeNameNode(peff::Alloc *self_allocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::F32, self_allocator, document) {
}

INTERBUFC_API F32TypeNameNode::F32TypeNameNode(const F32TypeNameNode &rhs, peff::Alloc *self_allocator) : TypeNameNode(rhs, self_allocator) {
}

INTERBUFC_API F32TypeNameNode::~F32TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> F64TypeNameNode::do_duplicate(peff::Alloc *new_allocator) const {
	AstNodePtr<F64TypeNameNode> duplicated_node(make_ast_node<F64TypeNameNode>(new_allocator, *this, new_allocator));
	if(!duplicated_node) {
		return {};
	}

	return duplicated_node.cast_to<AstNode>();
}

INTERBUFC_API F64TypeNameNode::F64TypeNameNode(peff::Alloc *self_allocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::F64, self_allocator, document) {
}

INTERBUFC_API F64TypeNameNode::F64TypeNameNode(const F64TypeNameNode &rhs, peff::Alloc *self_allocator) : TypeNameNode(rhs, self_allocator) {
}

INTERBUFC_API F64TypeNameNode::~F64TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> StringTypeNameNode::do_duplicate(peff::Alloc *new_allocator) const {
	AstNodePtr<StringTypeNameNode> duplicated_node(make_ast_node<StringTypeNameNode>(new_allocator, *this, new_allocator));
	if(!duplicated_node) {
		return {};
	}

	return duplicated_node.cast_to<AstNode>();
}

INTERBUFC_API StringTypeNameNode::StringTypeNameNode(peff::Alloc *self_allocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::String, self_allocator, document) {
}

INTERBUFC_API StringTypeNameNode::StringTypeNameNode(const StringTypeNameNode &rhs, peff::Alloc *self_allocator) : TypeNameNode(rhs, self_allocator) {
}

INTERBUFC_API StringTypeNameNode::~StringTypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> BoolTypeNameNode::do_duplicate(peff::Alloc *new_allocator) const {
	AstNodePtr<BoolTypeNameNode> duplicated_node(make_ast_node<BoolTypeNameNode>(new_allocator, *this, new_allocator));
	if(!duplicated_node) {
		return {};
	}

	return duplicated_node.cast_to<AstNode>();
}

INTERBUFC_API BoolTypeNameNode::BoolTypeNameNode(peff::Alloc *self_allocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::Bool, self_allocator, document) {
}

INTERBUFC_API BoolTypeNameNode::BoolTypeNameNode(const BoolTypeNameNode &rhs, peff::Alloc *self_allocator) : TypeNameNode(rhs, self_allocator) {
}

INTERBUFC_API BoolTypeNameNode::~BoolTypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> ObjectTypeNameNode::do_duplicate(peff::Alloc *new_allocator) const {
	AstNodePtr<ObjectTypeNameNode> duplicated_node(make_ast_node<ObjectTypeNameNode>(new_allocator, *this, new_allocator));
	if (!duplicated_node) {
		return {};
	}

	return duplicated_node.cast_to<AstNode>();
}

INTERBUFC_API ObjectTypeNameNode::ObjectTypeNameNode(peff::Alloc *self_allocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::Object, self_allocator, document) {
}

INTERBUFC_API ObjectTypeNameNode::ObjectTypeNameNode(const ObjectTypeNameNode &rhs, peff::Alloc *self_allocator) : TypeNameNode(rhs, self_allocator) {
}

INTERBUFC_API ObjectTypeNameNode::~ObjectTypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> CustomTypeNameNode::do_duplicate(peff::Alloc *new_allocator) const {
	bool succeeded = false;
	AstNodePtr<CustomTypeNameNode> duplicated_node(make_ast_node<CustomTypeNameNode>(new_allocator, *this, new_allocator, succeeded));
	if((!duplicated_node) || (!succeeded)) {
		return {};
	}

	return duplicated_node.cast_to<AstNode>();
}

INTERBUFC_API CustomTypeNameNode::CustomTypeNameNode(peff::Alloc *self_allocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::Custom, self_allocator, document) {
}

INTERBUFC_API CustomTypeNameNode::CustomTypeNameNode(const CustomTypeNameNode &rhs, peff::Alloc *allocator, bool &succeeded_out) : TypeNameNode(rhs, allocator) {
	if(!(id_ref_ptr = duplicate_id_ref(allocator, rhs.id_ref_ptr.get()))) {
		succeeded_out = false;
		return;
	}

	context_node = rhs.context_node;

	succeeded_out = true;
}

INTERBUFC_API CustomTypeNameNode::~CustomTypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> ArrayTypeNameNode::do_duplicate(peff::Alloc *new_allocator) const {
	bool succeeded = false;
	AstNodePtr<ArrayTypeNameNode> duplicated_node(make_ast_node<ArrayTypeNameNode>(new_allocator, *this, new_allocator, succeeded));
	if((!duplicated_node) || (!succeeded)) {
		return {};
	}

	return duplicated_node.cast_to<AstNode>();
}

INTERBUFC_API ArrayTypeNameNode::ArrayTypeNameNode(peff::Alloc *self_allocator, const AstNodePtr<Document> &document, const AstNodePtr<TypeNameNode> &element_type) : TypeNameNode(TypeNameKind::Array, self_allocator, document), element_type(element_type) {
}

INTERBUFC_API ArrayTypeNameNode::ArrayTypeNameNode(const ArrayTypeNameNode &rhs, peff::Alloc *allocator, bool &succeeded_out) : TypeNameNode(rhs, allocator) {
	if(!(element_type = rhs.element_type->duplicate<TypeNameNode>(allocator))) {
		succeeded_out = false;
		return;
	}

	succeeded_out = true;
}

INTERBUFC_API ArrayTypeNameNode::~ArrayTypeNameNode() {
}

