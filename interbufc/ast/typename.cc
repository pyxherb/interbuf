#include "typename.h"

using namespace interbufc;

INTERBUFC_API AstNodePtr<AstNode> I8TypeNameNode::doDuplicate(peff::Alloc *newAllocator) const {
	AstNodePtr<I8TypeNameNode> duplicatedNode(makeAstNode<I8TypeNameNode>(newAllocator, *this, newAllocator));
	if(!duplicatedNode) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API I8TypeNameNode::I8TypeNameNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::I8, selfAllocator, document) {
}

INTERBUFC_API I8TypeNameNode::I8TypeNameNode(const I8TypeNameNode &rhs, peff::Alloc *selfAllocator) : TypeNameNode(rhs, selfAllocator) {
}

INTERBUFC_API I8TypeNameNode::~I8TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> I16TypeNameNode::doDuplicate(peff::Alloc *newAllocator) const {
	AstNodePtr<I16TypeNameNode> duplicatedNode(makeAstNode<I16TypeNameNode>(newAllocator, *this, newAllocator));
	if(!duplicatedNode) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API I16TypeNameNode::I16TypeNameNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::I16, selfAllocator, document) {
}

INTERBUFC_API I16TypeNameNode::I16TypeNameNode(const I16TypeNameNode &rhs, peff::Alloc *selfAllocator) : TypeNameNode(rhs, selfAllocator) {
}

INTERBUFC_API I16TypeNameNode::~I16TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> I32TypeNameNode::doDuplicate(peff::Alloc *newAllocator) const {
	AstNodePtr<I32TypeNameNode> duplicatedNode(makeAstNode<I32TypeNameNode>(newAllocator, *this, newAllocator));
	if(!duplicatedNode) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API I32TypeNameNode::I32TypeNameNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::I32, selfAllocator, document) {
}

INTERBUFC_API I32TypeNameNode::I32TypeNameNode(const I32TypeNameNode &rhs, peff::Alloc *selfAllocator) : TypeNameNode(rhs, selfAllocator) {
}

INTERBUFC_API I32TypeNameNode::~I32TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> I64TypeNameNode::doDuplicate(peff::Alloc *newAllocator) const {
	AstNodePtr<I64TypeNameNode> duplicatedNode(makeAstNode<I64TypeNameNode>(newAllocator, *this, newAllocator));
	if(!duplicatedNode) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API I64TypeNameNode::I64TypeNameNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::I64, selfAllocator, document) {
}

INTERBUFC_API I64TypeNameNode::I64TypeNameNode(const I64TypeNameNode &rhs, peff::Alloc *selfAllocator) : TypeNameNode(rhs, selfAllocator) {
}

INTERBUFC_API I64TypeNameNode::~I64TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> U8TypeNameNode::doDuplicate(peff::Alloc *newAllocator) const {
	AstNodePtr<U8TypeNameNode> duplicatedNode(makeAstNode<U8TypeNameNode>(newAllocator, *this, newAllocator));
	if(!duplicatedNode) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API U8TypeNameNode::U8TypeNameNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::U8, selfAllocator, document) {
}

INTERBUFC_API U8TypeNameNode::U8TypeNameNode(const U8TypeNameNode &rhs, peff::Alloc *selfAllocator) : TypeNameNode(rhs, selfAllocator) {
}

INTERBUFC_API U8TypeNameNode::~U8TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> U16TypeNameNode::doDuplicate(peff::Alloc *newAllocator) const {
	AstNodePtr<U16TypeNameNode> duplicatedNode(makeAstNode<U16TypeNameNode>(newAllocator, *this, newAllocator));
	if(!duplicatedNode) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API U16TypeNameNode::U16TypeNameNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::U16, selfAllocator, document) {
}

INTERBUFC_API U16TypeNameNode::U16TypeNameNode(const U16TypeNameNode &rhs, peff::Alloc *selfAllocator) : TypeNameNode(rhs, selfAllocator) {
}

INTERBUFC_API U16TypeNameNode::~U16TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> U32TypeNameNode::doDuplicate(peff::Alloc *newAllocator) const {
	AstNodePtr<U32TypeNameNode> duplicatedNode(makeAstNode<U32TypeNameNode>(newAllocator, *this, newAllocator));
	if(!duplicatedNode) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API U32TypeNameNode::U32TypeNameNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::U32, selfAllocator, document) {
}

INTERBUFC_API U32TypeNameNode::U32TypeNameNode(const U32TypeNameNode &rhs, peff::Alloc *selfAllocator) : TypeNameNode(rhs, selfAllocator) {
}

INTERBUFC_API U32TypeNameNode::~U32TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> U64TypeNameNode::doDuplicate(peff::Alloc *newAllocator) const {
	AstNodePtr<U64TypeNameNode> duplicatedNode(makeAstNode<U64TypeNameNode>(newAllocator, *this, newAllocator));
	if(!duplicatedNode) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API U64TypeNameNode::U64TypeNameNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::U64, selfAllocator, document) {
}

INTERBUFC_API U64TypeNameNode::U64TypeNameNode(const U64TypeNameNode &rhs, peff::Alloc *selfAllocator) : TypeNameNode(rhs, selfAllocator) {
}

INTERBUFC_API U64TypeNameNode::~U64TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> F32TypeNameNode::doDuplicate(peff::Alloc *newAllocator) const {
	AstNodePtr<F32TypeNameNode> duplicatedNode(makeAstNode<F32TypeNameNode>(newAllocator, *this, newAllocator));
	if(!duplicatedNode) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API F32TypeNameNode::F32TypeNameNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::F32, selfAllocator, document) {
}

INTERBUFC_API F32TypeNameNode::F32TypeNameNode(const F32TypeNameNode &rhs, peff::Alloc *selfAllocator) : TypeNameNode(rhs, selfAllocator) {
}

INTERBUFC_API F32TypeNameNode::~F32TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> F64TypeNameNode::doDuplicate(peff::Alloc *newAllocator) const {
	AstNodePtr<F64TypeNameNode> duplicatedNode(makeAstNode<F64TypeNameNode>(newAllocator, *this, newAllocator));
	if(!duplicatedNode) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API F64TypeNameNode::F64TypeNameNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::F64, selfAllocator, document) {
}

INTERBUFC_API F64TypeNameNode::F64TypeNameNode(const F64TypeNameNode &rhs, peff::Alloc *selfAllocator) : TypeNameNode(rhs, selfAllocator) {
}

INTERBUFC_API F64TypeNameNode::~F64TypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> StringTypeNameNode::doDuplicate(peff::Alloc *newAllocator) const {
	AstNodePtr<StringTypeNameNode> duplicatedNode(makeAstNode<StringTypeNameNode>(newAllocator, *this, newAllocator));
	if(!duplicatedNode) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API StringTypeNameNode::StringTypeNameNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::String, selfAllocator, document) {
}

INTERBUFC_API StringTypeNameNode::StringTypeNameNode(const StringTypeNameNode &rhs, peff::Alloc *selfAllocator) : TypeNameNode(rhs, selfAllocator) {
}

INTERBUFC_API StringTypeNameNode::~StringTypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> BoolTypeNameNode::doDuplicate(peff::Alloc *newAllocator) const {
	AstNodePtr<BoolTypeNameNode> duplicatedNode(makeAstNode<BoolTypeNameNode>(newAllocator, *this, newAllocator));
	if(!duplicatedNode) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API BoolTypeNameNode::BoolTypeNameNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::Bool, selfAllocator, document) {
}

INTERBUFC_API BoolTypeNameNode::BoolTypeNameNode(const BoolTypeNameNode &rhs, peff::Alloc *selfAllocator) : TypeNameNode(rhs, selfAllocator) {
}

INTERBUFC_API BoolTypeNameNode::~BoolTypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> ObjectTypeNameNode::doDuplicate(peff::Alloc *newAllocator) const {
	AstNodePtr<ObjectTypeNameNode> duplicatedNode(makeAstNode<ObjectTypeNameNode>(newAllocator, *this, newAllocator));
	if (!duplicatedNode) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API ObjectTypeNameNode::ObjectTypeNameNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::Object, selfAllocator, document) {
}

INTERBUFC_API ObjectTypeNameNode::ObjectTypeNameNode(const ObjectTypeNameNode &rhs, peff::Alloc *selfAllocator) : TypeNameNode(rhs, selfAllocator) {
}

INTERBUFC_API ObjectTypeNameNode::~ObjectTypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> CustomTypeNameNode::doDuplicate(peff::Alloc *newAllocator) const {
	bool succeeded = false;
	AstNodePtr<CustomTypeNameNode> duplicatedNode(makeAstNode<CustomTypeNameNode>(newAllocator, *this, newAllocator, succeeded));
	if((!duplicatedNode) || (!succeeded)) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API CustomTypeNameNode::CustomTypeNameNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document) : TypeNameNode(TypeNameKind::Custom, selfAllocator, document) {
}

INTERBUFC_API CustomTypeNameNode::CustomTypeNameNode(const CustomTypeNameNode &rhs, peff::Alloc *allocator, bool &succeededOut) : TypeNameNode(rhs, allocator) {
	if(!(idRefPtr = duplicateIdRef(allocator, rhs.idRefPtr.get()))) {
		succeededOut = false;
		return;
	}

	contextNode = rhs.contextNode;

	succeededOut = true;
}

INTERBUFC_API CustomTypeNameNode::~CustomTypeNameNode() {
}

INTERBUFC_API AstNodePtr<AstNode> ArrayTypeNameNode::doDuplicate(peff::Alloc *newAllocator) const {
	bool succeeded = false;
	AstNodePtr<ArrayTypeNameNode> duplicatedNode(makeAstNode<ArrayTypeNameNode>(newAllocator, *this, newAllocator, succeeded));
	if((!duplicatedNode) || (!succeeded)) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API ArrayTypeNameNode::ArrayTypeNameNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document, const AstNodePtr<TypeNameNode> &elementType) : TypeNameNode(TypeNameKind::Array, selfAllocator, document), elementType(elementType) {
}

INTERBUFC_API ArrayTypeNameNode::ArrayTypeNameNode(const ArrayTypeNameNode &rhs, peff::Alloc *allocator, bool &succeededOut) : TypeNameNode(rhs, allocator) {
	if(!(elementType = rhs.elementType->duplicate<TypeNameNode>(allocator))) {
		succeededOut = false;
		return;
	}

	succeededOut = true;
}

INTERBUFC_API ArrayTypeNameNode::~ArrayTypeNameNode() {
}

