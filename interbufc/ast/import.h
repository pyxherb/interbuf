#ifndef _INTERBUFC_AST_IMPORT_H_
#define _INTERBUFC_AST_IMPORT_H_

#include "module.h"

namespace interbufc {
	class ImportNode : public MemberNode {
	protected:
		INTERBUFC_API virtual AstNodePtr<AstNode> do_duplicate(peff::Alloc *new_allocator) const override;

	public:
		IdRefPtr id_ref;

		INTERBUFC_API ImportNode(peff::Alloc *self_allocator, const AstNodePtr<Document> &document);
		INTERBUFC_API ImportNode(const ImportNode &rhs, peff::Alloc *allocator, bool &succeeded_out);
		INTERBUFC_API virtual ~ImportNode();
	};
}

#endif
