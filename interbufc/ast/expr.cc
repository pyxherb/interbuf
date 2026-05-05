#include "expr.h"

using namespace interbufc;

INTERBUFC_API ExprNode::ExprNode(ExprKind expr_kind, peff::Alloc *self_allocator, const peff::SharedPtr<Document> &document) : AstNode(AstNodeType::TypeName, self_allocator, document), expr_kind(expr_kind) {
}

INTERBUFC_API ExprNode::~ExprNode() {
}

INTERBUFC_API IdRefExprNode::IdRefExprNode(
	peff::Alloc *self_allocator,
	const peff::SharedPtr<Document> &document,
	IdRefPtr &&id_ref_ptr)
	: ExprNode(ExprKind::IdRef, self_allocator, document),
	  id_ref_ptr(std::move(id_ref_ptr)) {
}
INTERBUFC_API IdRefExprNode::~IdRefExprNode() {
}

INTERBUFC_API I8LiteralExprNode::I8LiteralExprNode(
	peff::Alloc *self_allocator,
	const peff::SharedPtr<Document> &document,
	int8_t data)
	: ExprNode(ExprKind::I8, self_allocator, document),
	  data(data) {
}
INTERBUFC_API I8LiteralExprNode::~I8LiteralExprNode() {
}

INTERBUFC_API I16LiteralExprNode::I16LiteralExprNode(
	peff::Alloc *self_allocator,
	const peff::SharedPtr<Document> &document,
	int16_t data)
	: ExprNode(ExprKind::I16, self_allocator, document),
	  data(data) {
}
INTERBUFC_API I16LiteralExprNode::~I16LiteralExprNode() {
}

INTERBUFC_API I32LiteralExprNode::I32LiteralExprNode(
	peff::Alloc *self_allocator,
	const peff::SharedPtr<Document> &document,
	int32_t data)
	: ExprNode(ExprKind::I32, self_allocator, document),
	  data(data) {
}
INTERBUFC_API I32LiteralExprNode::~I32LiteralExprNode() {
}

INTERBUFC_API I64LiteralExprNode::I64LiteralExprNode(
	peff::Alloc *self_allocator,
	const peff::SharedPtr<Document> &document,
	int64_t data)
	: ExprNode(ExprKind::I64, self_allocator, document),
	  data(data) {
}
INTERBUFC_API I64LiteralExprNode::~I64LiteralExprNode() {
}

INTERBUFC_API U8LiteralExprNode::U8LiteralExprNode(
	peff::Alloc *self_allocator,
	const peff::SharedPtr<Document> &document,
	uint8_t data)
	: ExprNode(ExprKind::U8, self_allocator, document),
	  data(data) {
}
INTERBUFC_API U8LiteralExprNode::~U8LiteralExprNode() {
}

INTERBUFC_API U16LiteralExprNode::U16LiteralExprNode(
	peff::Alloc *self_allocator,
	const peff::SharedPtr<Document> &document,
	uint16_t data)
	: ExprNode(ExprKind::U16, self_allocator, document),
	  data(data) {
}
INTERBUFC_API U16LiteralExprNode::~U16LiteralExprNode() {
}

INTERBUFC_API U32LiteralExprNode::U32LiteralExprNode(
	peff::Alloc *self_allocator,
	const peff::SharedPtr<Document> &document,
	uint32_t data)
	: ExprNode(ExprKind::U32, self_allocator, document),
	  data(data) {
}
INTERBUFC_API U32LiteralExprNode::~U32LiteralExprNode() {
}

INTERBUFC_API U64LiteralExprNode::U64LiteralExprNode(
	peff::Alloc *self_allocator,
	const peff::SharedPtr<Document> &document,
	uint64_t data)
	: ExprNode(ExprKind::U64, self_allocator, document),
	  data(data) {
}
INTERBUFC_API U64LiteralExprNode::~U64LiteralExprNode() {
}

INTERBUFC_API F32LiteralExprNode::F32LiteralExprNode(
	peff::Alloc *self_allocator,
	const peff::SharedPtr<Document> &document,
	float data)
	: ExprNode(ExprKind::F32, self_allocator, document),
	  data(data) {
}
INTERBUFC_API F32LiteralExprNode::~F32LiteralExprNode() {
}

INTERBUFC_API F64LiteralExprNode::F64LiteralExprNode(
	peff::Alloc *self_allocator,
	const peff::SharedPtr<Document> &document,
	double data)
	: ExprNode(ExprKind::F64, self_allocator, document),
	  data(data) {
}
INTERBUFC_API F64LiteralExprNode::~F64LiteralExprNode() {
}

INTERBUFC_API BoolLiteralExprNode::BoolLiteralExprNode(
	peff::Alloc *self_allocator,
	const peff::SharedPtr<Document> &document,
	bool data)
	: ExprNode(ExprKind::Bool, self_allocator, document),
	  data(data) {
}
INTERBUFC_API BoolLiteralExprNode::~BoolLiteralExprNode() {
}

INTERBUFC_API StringLiteralExprNode::StringLiteralExprNode(
	peff::Alloc *self_allocator,
	const peff::SharedPtr<Document> &document,
	peff::String &&data)
	: ExprNode(ExprKind::String, self_allocator, document),
	  data(std::move(data)) {
}
INTERBUFC_API StringLiteralExprNode::~StringLiteralExprNode() {
}

INTERBUFC_API BadExprNode::BadExprNode(
	peff::Alloc *self_allocator,
	const peff::SharedPtr<Document> &document,
	const AstNodePtr<ExprNode> &incomplete_expr)
	: ExprNode(ExprKind::Bad, self_allocator, document),
	  incomplete_expr(incomplete_expr) {
}
INTERBUFC_API BadExprNode::~BadExprNode() {
}
