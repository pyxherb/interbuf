#ifndef _INTERBUFC_AST_ASTNODE_H_
#define _INTERBUFC_AST_ASTNODE_H_

#include "lexer.h"
#include <peff/advutils/shared_ptr.h>

namespace interbufc {
	enum class AstNodeType : uint8_t {
		Struct = 0,
		Enum,
		EnumItem,
		AttributeDef,
		Attribute,
		FnSlot,
		Fn,
		Stmt,
		Expr,
		TypeName,
		Using,
		Var,
		GenericParam,
		Module,
		Class,
		Interface,
		Import,

		Root,
		This,

		Bad
	};

	struct TokenRange {
		size_t begin_index = SIZE_MAX, end_index = SIZE_MAX;

		inline TokenRange() = default;
		inline TokenRange(size_t index) : begin_index(index), end_index(index) {}
		inline TokenRange(size_t begin_index, size_t end_index) : begin_index(begin_index), end_index(end_index) {}

		INTERBUFC_FORCEINLINE operator bool() const {
			return begin_index != SIZE_MAX;
		}

		INTERBUFC_FORCEINLINE bool operator<(const TokenRange &rhs) const {
			return begin_index < rhs.begin_index;
		}

		INTERBUFC_FORCEINLINE bool operator>(const TokenRange &rhs) const {
			return begin_index < rhs.begin_index;
		}
	};

	constexpr static size_t ASTNODE_ALIGNMENT = sizeof(std::max_align_t);

	class AstNode;

	typedef void (*AstNodeDestructor)(AstNode *ast_node);

	class AstNode : public peff::SharedFromThis<AstNode> {
	protected:
		INTERBUFC_API virtual AstNodePtr<AstNode> do_duplicate(peff::Alloc *new_allocator) const;

	public:
		AstNodeType ast_node_type;
		peff::RcObjectPtr<peff::Alloc> self_allocator;
		Document *document;
		TokenRange token_range;

		AstNode *next_destructible = nullptr;
		AstNodeDestructor destructor = nullptr;

		INTERBUFC_API AstNode(AstNodeType ast_node_type, peff::Alloc *self_allocator, const AstNodePtr<Document> &document);
		INTERBUFC_API AstNode(const AstNode &other, peff::Alloc *new_allocator);
		INTERBUFC_API virtual ~AstNode();

		template <typename T>
		INTERBUFC_FORCEINLINE AstNodePtr<T> duplicate(peff::Alloc *new_allocator) const {
			return do_duplicate(new_allocator).cast_to<T>();
		}
	};

	INTERBUFC_API void add_ast_node_to_destructible_list(AstNode *ast_node, AstNodeDestructor destructor);

	template <typename T>
	struct AstNodeControlBlock : public AstNodePtr<T>::DefaultSharedPtrControlBlock {
		PEFF_FORCEINLINE AstNodeControlBlock(peff::Alloc *allocator, T *ptr) noexcept : AstNodePtr<T>::DefaultSharedPtrControlBlock(allocator, ptr) {}
		inline virtual ~AstNodeControlBlock() {}

		inline virtual void on_strong_ref_zero() noexcept override {
			add_ast_node_to_destructible_list(this->ptr, [](AstNode *ast_node) {
				peff::destroy_and_release<T>(ast_node->self_allocator.get(), static_cast<T *>(ast_node), alignof(T));
			});
		}

		inline virtual void on_ref_zero() noexcept override {
			peff::destroy_and_release<AstNodeControlBlock<T>>(this->allocator.get(), this, alignof(AstNodeControlBlock<T>));
		}
	};

	template <typename T, typename... Args>
	INTERBUFC_FORCEINLINE AstNodePtr<T> make_ast_node(peff::Alloc *allocator, Args &&...args) {
		return peff::make_shared_with_control_block<T, AstNodeControlBlock<T>>(allocator, std::forward<Args>(args)...);
	}
}

#endif
