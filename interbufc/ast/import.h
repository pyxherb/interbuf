#ifndef _INTERBUFC_AST_IMPORT_H_
#define _INTERBUFC_AST_IMPORT_H_

#include "module.h"

namespace interbufc {
	class ImportNode : public MemberNode {
	protected:
		INTERBUFC_API virtual AstNodePtr<AstNode> doDuplicate(peff::Alloc *newAllocator) const override;

	public:
		IdRefPtr idRef;

		INTERBUFC_API ImportNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document);
		INTERBUFC_API ImportNode(const ImportNode &rhs, peff::Alloc *allocator, bool &succeededOut);
		INTERBUFC_API virtual ~ImportNode();
	};
}

#endif
