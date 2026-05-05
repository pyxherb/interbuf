#ifndef _INTERBUFC_AST_EXPR_H_
#define _INTERBUFC_AST_EXPR_H_

#include "typename_base.h"
#include "idref.h"
#include <peff/containers/hashmap.h>

namespace interbufc {
	enum class ExprKind {
		IdRef,	// Identifier reference

		I8,		 // i8 literal
		I16,	 // i16 literal
		I32,	 // i32 literal
		I64,	 // i64 literal
		U8,		 // u8 literal
		U16,	 // u16 literal
		U32,	 // u32 literal
		U64,	 // u64 literal
		F32,	 // f32 literal
		F64,	 // f64 literal
		String,	 // String literal
		Bool,	 // bool litera

		Bad,  // Bad expression
	};

	enum class UnaryOp {
		LNot,
		Not,
		Neg,
		Unpacking
	};

	class ExprNode : public AstNode {
	public:
		ExprKind expr_kind;

		INTERBUFC_API ExprNode(ExprKind expr_kind, peff::Alloc *self_allocator, const peff::SharedPtr<Document> &document);
		INTERBUFC_API virtual ~ExprNode();
	};

	class IdRefExprNode : public ExprNode {
	public:
		IdRefPtr id_ref_ptr;

		INTERBUFC_API IdRefExprNode(peff::Alloc *self_allocator, const peff::SharedPtr<Document> &document, IdRefPtr &&id_ref_ptr);
		INTERBUFC_API virtual ~IdRefExprNode();
	};

	class I8LiteralExprNode : public ExprNode {
	public:
		int8_t data;

		INTERBUFC_API I8LiteralExprNode(peff::Alloc *self_allocator, const peff::SharedPtr<Document> &document, int8_t data);
		INTERBUFC_API virtual ~I8LiteralExprNode();
	};

	class I16LiteralExprNode : public ExprNode {
	public:
		int16_t data;

		INTERBUFC_API I16LiteralExprNode(peff::Alloc *self_allocator, const peff::SharedPtr<Document> &document, int16_t data);
		INTERBUFC_API virtual ~I16LiteralExprNode();
	};

	class I32LiteralExprNode : public ExprNode {
	public:
		int32_t data;

		INTERBUFC_API I32LiteralExprNode(peff::Alloc *self_allocator, const peff::SharedPtr<Document> &document, int32_t data);
		INTERBUFC_API virtual ~I32LiteralExprNode();
	};

	class I64LiteralExprNode : public ExprNode {
	public:
		int64_t data;

		INTERBUFC_API I64LiteralExprNode(peff::Alloc *self_allocator, const peff::SharedPtr<Document> &document, int64_t data);
		INTERBUFC_API virtual ~I64LiteralExprNode();
	};

	class U8LiteralExprNode : public ExprNode {
	public:
		uint8_t data;

		INTERBUFC_API U8LiteralExprNode(peff::Alloc *self_allocator, const peff::SharedPtr<Document> &document, uint8_t data);
		INTERBUFC_API virtual ~U8LiteralExprNode();
	};

	class U16LiteralExprNode : public ExprNode {
	public:
		uint16_t data;

		INTERBUFC_API U16LiteralExprNode(peff::Alloc *self_allocator, const peff::SharedPtr<Document> &document, uint16_t data);
		INTERBUFC_API virtual ~U16LiteralExprNode();
	};

	class U32LiteralExprNode : public ExprNode {
	public:
		uint32_t data;

		INTERBUFC_API U32LiteralExprNode(peff::Alloc *self_allocator, const peff::SharedPtr<Document> &document, uint32_t data);
		INTERBUFC_API virtual ~U32LiteralExprNode();
	};

	class U64LiteralExprNode : public ExprNode {
	public:
		uint64_t data;

		INTERBUFC_API U64LiteralExprNode(peff::Alloc *self_allocator, const peff::SharedPtr<Document> &document, uint64_t data);
		INTERBUFC_API virtual ~U64LiteralExprNode();
	};

	class F32LiteralExprNode : public ExprNode {
	public:
		float data;

		INTERBUFC_API F32LiteralExprNode(peff::Alloc *self_allocator, const peff::SharedPtr<Document> &document, float data);
		INTERBUFC_API virtual ~F32LiteralExprNode();
	};

	class F64LiteralExprNode : public ExprNode {
	public:
		double data;

		INTERBUFC_API F64LiteralExprNode(peff::Alloc *self_allocator, const peff::SharedPtr<Document> &document, double data);
		INTERBUFC_API virtual ~F64LiteralExprNode();
	};

	class BoolLiteralExprNode : public ExprNode {
	public:
		bool data;

		INTERBUFC_API BoolLiteralExprNode(peff::Alloc *self_allocator, const peff::SharedPtr<Document> &document, bool data);
		INTERBUFC_API virtual ~BoolLiteralExprNode();
	};

	class StringLiteralExprNode : public ExprNode {
	public:
		peff::String data;

		INTERBUFC_API StringLiteralExprNode(peff::Alloc *self_allocator, const peff::SharedPtr<Document> &document, peff::String &&data);
		INTERBUFC_API virtual ~StringLiteralExprNode();
	};

	class BadExprNode : public ExprNode {
	public:
		AstNodePtr<ExprNode> incomplete_expr;

		INTERBUFC_API BadExprNode(peff::Alloc *self_allocator, const peff::SharedPtr<Document> &document, const AstNodePtr<ExprNode> &incomplete_expr);
		INTERBUFC_API virtual ~BadExprNode();
	};
}

#endif
