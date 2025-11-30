#include "expr.h"

using namespace interbufc;

INTERBUFC_API ExprNode::ExprNode(ExprKind exprKind, peff::Alloc *selfAllocator, const peff::SharedPtr<Document> &document) : AstNode(AstNodeType::TypeName, selfAllocator, document), exprKind(exprKind) {
}

INTERBUFC_API ExprNode::~ExprNode() {
}

INTERBUFC_API IdRefExprNode::IdRefExprNode(
	peff::Alloc *selfAllocator,
	const peff::SharedPtr<Document> &document,
	IdRefPtr &&idRefPtr)
	: ExprNode(ExprKind::IdRef, selfAllocator, document),
	  idRefPtr(std::move(idRefPtr)) {
}
INTERBUFC_API IdRefExprNode::~IdRefExprNode() {
}

INTERBUFC_API I8LiteralExprNode::I8LiteralExprNode(
	peff::Alloc *selfAllocator,
	const peff::SharedPtr<Document> &document,
	int8_t data)
	: ExprNode(ExprKind::I8, selfAllocator, document),
	  data(data) {
}
INTERBUFC_API I8LiteralExprNode::~I8LiteralExprNode() {
}

INTERBUFC_API I16LiteralExprNode::I16LiteralExprNode(
	peff::Alloc *selfAllocator,
	const peff::SharedPtr<Document> &document,
	int16_t data)
	: ExprNode(ExprKind::I16, selfAllocator, document),
	  data(data) {
}
INTERBUFC_API I16LiteralExprNode::~I16LiteralExprNode() {
}

INTERBUFC_API I32LiteralExprNode::I32LiteralExprNode(
	peff::Alloc *selfAllocator,
	const peff::SharedPtr<Document> &document,
	int32_t data)
	: ExprNode(ExprKind::I32, selfAllocator, document),
	  data(data) {
}
INTERBUFC_API I32LiteralExprNode::~I32LiteralExprNode() {
}

INTERBUFC_API I64LiteralExprNode::I64LiteralExprNode(
	peff::Alloc *selfAllocator,
	const peff::SharedPtr<Document> &document,
	int64_t data)
	: ExprNode(ExprKind::I64, selfAllocator, document),
	  data(data) {
}
INTERBUFC_API I64LiteralExprNode::~I64LiteralExprNode() {
}

INTERBUFC_API U8LiteralExprNode::U8LiteralExprNode(
	peff::Alloc *selfAllocator,
	const peff::SharedPtr<Document> &document,
	uint8_t data)
	: ExprNode(ExprKind::U8, selfAllocator, document),
	  data(data) {
}
INTERBUFC_API U8LiteralExprNode::~U8LiteralExprNode() {
}

INTERBUFC_API U16LiteralExprNode::U16LiteralExprNode(
	peff::Alloc *selfAllocator,
	const peff::SharedPtr<Document> &document,
	uint16_t data)
	: ExprNode(ExprKind::U16, selfAllocator, document),
	  data(data) {
}
INTERBUFC_API U16LiteralExprNode::~U16LiteralExprNode() {
}

INTERBUFC_API U32LiteralExprNode::U32LiteralExprNode(
	peff::Alloc *selfAllocator,
	const peff::SharedPtr<Document> &document,
	uint32_t data)
	: ExprNode(ExprKind::U32, selfAllocator, document),
	  data(data) {
}
INTERBUFC_API U32LiteralExprNode::~U32LiteralExprNode() {
}

INTERBUFC_API U64LiteralExprNode::U64LiteralExprNode(
	peff::Alloc *selfAllocator,
	const peff::SharedPtr<Document> &document,
	uint64_t data)
	: ExprNode(ExprKind::U64, selfAllocator, document),
	  data(data) {
}
INTERBUFC_API U64LiteralExprNode::~U64LiteralExprNode() {
}

INTERBUFC_API F32LiteralExprNode::F32LiteralExprNode(
	peff::Alloc *selfAllocator,
	const peff::SharedPtr<Document> &document,
	float data)
	: ExprNode(ExprKind::F32, selfAllocator, document),
	  data(data) {
}
INTERBUFC_API F32LiteralExprNode::~F32LiteralExprNode() {
}

INTERBUFC_API F64LiteralExprNode::F64LiteralExprNode(
	peff::Alloc *selfAllocator,
	const peff::SharedPtr<Document> &document,
	double data)
	: ExprNode(ExprKind::F64, selfAllocator, document),
	  data(data) {
}
INTERBUFC_API F64LiteralExprNode::~F64LiteralExprNode() {
}

INTERBUFC_API BoolLiteralExprNode::BoolLiteralExprNode(
	peff::Alloc *selfAllocator,
	const peff::SharedPtr<Document> &document,
	bool data)
	: ExprNode(ExprKind::Bool, selfAllocator, document),
	  data(data) {
}
INTERBUFC_API BoolLiteralExprNode::~BoolLiteralExprNode() {
}

INTERBUFC_API StringLiteralExprNode::StringLiteralExprNode(
	peff::Alloc *selfAllocator,
	const peff::SharedPtr<Document> &document,
	peff::String &&data)
	: ExprNode(ExprKind::String, selfAllocator, document),
	  data(std::move(data)) {
}
INTERBUFC_API StringLiteralExprNode::~StringLiteralExprNode() {
}

INTERBUFC_API BadExprNode::BadExprNode(
	peff::Alloc *selfAllocator,
	const peff::SharedPtr<Document> &document,
	const AstNodePtr<ExprNode> &incompleteExpr)
	: ExprNode(ExprKind::Bad, selfAllocator, document),
	  incompleteExpr(incompleteExpr) {
}
INTERBUFC_API BadExprNode::~BadExprNode() {
}
