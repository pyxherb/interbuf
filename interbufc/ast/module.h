#ifndef _INTERBUFC_AST_MODULE_H_
#define _INTERBUFC_AST_MODULE_H_

#include "typename.h"
#include "idref.h"
#include <peff/containers/hashmap.h>

namespace interbufc {
	class MemberNode : public AstNode {
	public:
		MemberNode *parent = nullptr;  // DO NOT use WeakPtr because we want to set the parent during the copy constructor is executing.
		peff::String name;
		peff::DynArray<AstNodePtr<TypeNameNode>> genericArgs;

		INTERBUFC_API MemberNode(AstNodeType astNodeType, peff::Alloc *selfAllocator, const AstNodePtr<Document> &document);
		INTERBUFC_API MemberNode(const MemberNode &rhs, peff::Alloc *allocator, bool &succeededOut);
		INTERBUFC_API virtual ~MemberNode();

		INTERBUFC_FORCEINLINE void setParent(MemberNode *parent) noexcept {
			this->parent = parent;
		}
	};

	class ImportNode;
	class VarDefStmtNode;

	class Parser;

	class ModuleNode : public MemberNode {
	protected:
		INTERBUFC_API virtual AstNodePtr<AstNode> doDuplicate(peff::Alloc *newAllocator) const override;

	public:
		AstNodePtr<Parser> parser;
		peff::DynArray<AstNodePtr<MemberNode>> members;
		peff::HashMap<std::string_view, size_t> memberIndices;
		peff::DynArray<AstNodePtr<ImportNode>> anonymousImports;

		bool isVarDefStmtsNormalized = false;

		INTERBUFC_API ModuleNode(peff::Alloc *selfAllocator, const AstNodePtr<Document> &document, AstNodeType astNodeType = AstNodeType::Module);
		INTERBUFC_API ModuleNode(const ModuleNode &rhs, peff::Alloc *allocator, bool &succeededOut);
		INTERBUFC_API virtual ~ModuleNode();

		[[nodiscard]] INTERBUFC_API size_t pushMember(AstNodePtr<MemberNode> memberNode) noexcept;
		/// @brief Push and index a member.
		/// @param memberNode Member node to be added
		/// @return Whether the member is added successfully.
		[[nodiscard]] INTERBUFC_API bool addMember(AstNodePtr<MemberNode> memberNode) noexcept;
		[[nodiscard]] INTERBUFC_API bool indexMember(size_t indexInMemberArray) noexcept;
		/// @brief Remove a named member.
		/// @param name Name of the member to be removed.
		/// @return Whether the member is removed successfully.
		[[nodiscard]] INTERBUFC_API bool removeMember(const std::string_view &name) noexcept;

		INTERBUFC_API void setParser(AstNodePtr<Parser> parser);
	};
}

#endif
