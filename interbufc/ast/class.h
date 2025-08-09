#ifndef _INTERBUFC_AST_CLASS_H_
#define _INTERBUFC_AST_CLASS_H_

#include "module.h"

namespace interbufc {
	class ClassNode : public ModuleNode {
	protected:
		INTERBUFC_API virtual AstNodePtr<AstNode> doDuplicate(peff::Alloc *newAllocator) const override;

	public:
		INTERBUFC_API ClassNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document);
		INTERBUFC_API ClassNode(const ClassNode &rhs, peff::Alloc *allocator, bool &succeededOut);
		INTERBUFC_API virtual ~ClassNode();
	};

	class StructNode : public ModuleNode {
	protected:
		INTERBUFC_API virtual AstNodePtr<AstNode> doDuplicate(peff::Alloc *newAllocator) const override;

	public:
		INTERBUFC_API StructNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document);
		INTERBUFC_API StructNode(const StructNode &rhs, peff::Alloc *allocator, bool &succeededOut);
		INTERBUFC_API virtual ~StructNode();
	};
}

#endif
