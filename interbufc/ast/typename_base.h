#ifndef _INTERBUFC_AST_TYPENAME_BASE_H_
#define _INTERBUFC_AST_TYPENAME_BASE_H_

#include "astnode.h"

namespace interbufc {
	enum class TypeNameKind : uint8_t {
		Void = 0,
		I8,
		I16,
		I32,
		I64,
		ISize,
		U8,
		U16,
		U32,
		U64,
		USize,
		F32,
		F64,
		String,
		Bool,
		Object,
		Any,
		Custom,
		Unpacking,

		Fn,
		Array,
		Ref,
		TempRef,
		ParamTypeList,
		UnpackedParams,
		UnpackedArgs,

		Bad
	};

	class TypeNameNode : public AstNode {
	public:
		TypeNameKind typeNameKind;

		INTERBUFC_API TypeNameNode(TypeNameKind typeNameKind, peff::Alloc *selfAllocator, const AstNodePtr<Document> &document);
		INTERBUFC_API TypeNameNode(const TypeNameNode &rhs, peff::Alloc *selfAllocator);
		INTERBUFC_API virtual ~TypeNameNode();
	};
}

#endif
