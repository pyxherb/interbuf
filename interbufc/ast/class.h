#ifndef _INTERBUFC_AST_CLASS_H_
#define _INTERBUFC_AST_CLASS_H_

#include "module.h"
#include "expr.h"

namespace interbufc {
	class ClassNode : public ModuleNode {
	public:
		INTERBUFC_API ClassNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document);
		INTERBUFC_API virtual ~ClassNode();
	};

	class StructNode : public ModuleNode {
	public:
		INTERBUFC_API StructNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document);
		INTERBUFC_API virtual ~StructNode();
	};

	class EnumNode : public ModuleNode {
	public:
		AstNodePtr<TypeNameNode> baseType;

		INTERBUFC_API EnumNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document);
		INTERBUFC_API virtual ~EnumNode();
	};

	class EnumItemNode : public MemberNode {
	public:
		AstNodePtr<ExprNode> value;

		INTERBUFC_API EnumItemNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document);
		INTERBUFC_API virtual ~EnumItemNode();
	};
}

#endif
