#ifndef _INTERBUFC_AST_VAR_H_
#define _INTERBUFC_AST_VAR_H_

#include "module.h"

namespace interbufc {
	class NamespaceNode;

	class VarNode : public MemberNode {
	protected:
		INTERBUFC_API virtual AstNodePtr<AstNode> doDuplicate(peff::Alloc *newAllocator) const override;

	public:
		bool isTypeDeducedFromInitialValue = false;
		AstNodePtr<TypeNameNode> type;
		bool isTypeSealed = false;
		uint32_t idxReg = UINT32_MAX;

		INTERBUFC_API VarNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document);
		INTERBUFC_API VarNode(const VarNode &rhs, peff::Alloc *allocator, bool &succeededOut);
		INTERBUFC_API virtual ~VarNode();
	};
}

#endif
