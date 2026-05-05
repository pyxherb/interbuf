#ifndef _INTERBUFC_AST_VAR_H_
#define _INTERBUFC_AST_VAR_H_

#include "module.h"

namespace interbufc {
	class NamespaceNode;

	class VarNode : public MemberNode {
	protected:
		INTERBUFC_API virtual AstNodePtr<AstNode> do_duplicate(peff::Alloc *new_allocator) const override;

	public:
		AstNodePtr<TypeNameNode> type;

		INTERBUFC_API VarNode(peff::Alloc *self_allocator, const AstNodePtr<Document> &document);
		INTERBUFC_API VarNode(const VarNode &rhs, peff::Alloc *allocator, bool &succeeded_out);
		INTERBUFC_API virtual ~VarNode();
	};
}

#endif
