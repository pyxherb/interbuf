#ifndef _INTERBUFC_AST_CLASS_H_
#define _INTERBUFC_AST_CLASS_H_

#include "module.h"

namespace interbufc {
	class ClassNode : public ModuleNode {
	protected:
		INTERBUFC_API virtual AstNodePtr<AstNode> doDuplicate(peff::Alloc *newAllocator) const override;

	public:
		peff::HashMap<std::string_view, size_t> genericParamIndices;
		peff::DynArray<size_t> idxGenericParamCommaTokens;
		size_t idxLAngleBracketToken = SIZE_MAX, idxRAngleBracketToken = SIZE_MAX;

		bool isGenericParamsIndexed = false;

		INTERBUFC_API ClassNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document);
		INTERBUFC_API ClassNode(const ClassNode &rhs, peff::Alloc *allocator, bool &succeededOut);
		INTERBUFC_API virtual ~ClassNode();
	};
}

#endif
