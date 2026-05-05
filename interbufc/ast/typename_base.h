#ifndef _INTERBUFC_AST_TYPENAME_BASE_H_
#define _INTERBUFC_AST_TYPENAME_BASE_H_

#include "astnode.h"

namespace interbufc {
	enum class TypeNameKind : uint8_t {
		I8 = 0,
		I16,
		I32,
		I64,
		U8,
		U16,
		U32,
		U64,
		F32,
		F64,
		Object,
		String,
		Bool,
		Custom,

		Array,

		Bad
	};

	class TypeNameNode : public AstNode {
	public:
		TypeNameKind type_name_kind;

		INTERBUFC_API TypeNameNode(TypeNameKind type_name_kind, peff::Alloc *self_allocator, const AstNodePtr<Document> &document);
		INTERBUFC_API TypeNameNode(const TypeNameNode &rhs, peff::Alloc *self_allocator);
		INTERBUFC_API virtual ~TypeNameNode();
	};
}

#endif
