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

		INTERBUFC_API MemberNode(AstNodeType ast_node_type, peff::Alloc *self_allocator, const AstNodePtr<Document> &document);
		INTERBUFC_API MemberNode(const MemberNode &rhs, peff::Alloc *allocator, bool &succeeded_out);
		INTERBUFC_API virtual ~MemberNode();

		INTERBUFC_FORCEINLINE void set_parent(MemberNode *parent) noexcept {
			this->parent = parent;
		}
	};

	class ImportNode;
	class VarDefStmtNode;

	class Parser;

	class ModuleNode : public MemberNode {
	protected:
		INTERBUFC_API virtual AstNodePtr<AstNode> do_duplicate(peff::Alloc *new_allocator) const override;

	public:
		AstNodePtr<Parser> parser;
		IdRefPtr namespace_path;
		peff::DynArray<AstNodePtr<MemberNode>> members;
		peff::HashMap<std::string_view, size_t> member_indices;
		peff::DynArray<AstNodePtr<ImportNode>> anonymous_imports;

		bool is_var_def_stmts_normalized = false;

		INTERBUFC_API ModuleNode(peff::Alloc *self_allocator, const AstNodePtr<Document> &document, AstNodeType ast_node_type = AstNodeType::Module);
		INTERBUFC_API ModuleNode(const ModuleNode &rhs, peff::Alloc *allocator, bool &succeeded_out);
		INTERBUFC_API virtual ~ModuleNode();

		[[nodiscard]] INTERBUFC_API size_t push_member(AstNodePtr<MemberNode> member_node) noexcept;
		/// @brief Push and index a member.
		/// @param member_node Member node to be added
		/// @return Whether the member is added successfully.
		[[nodiscard]] INTERBUFC_API bool add_member(AstNodePtr<MemberNode> member_node) noexcept;
		[[nodiscard]] INTERBUFC_API bool index_member(size_t index_in_member_array) noexcept;
		/// @brief Remove a named member.
		/// @param name Name of the member to be removed.
		/// @return Whether the member is removed successfully.
		INTERBUFC_API void remove_member(const std::string_view &name) noexcept;

		INTERBUFC_API void set_parser(AstNodePtr<Parser> parser);
	};
}

#endif
