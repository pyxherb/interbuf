#ifndef _INTERBUFC_DOCUMENT_H_
#define _INTERBUFC_DOCUMENT_H_

#include "astnode.h"
#include <peff/containers/dynarray.h>
#include <peff/containers/map.h>
#include <peff/advutils/shared_ptr.h>

namespace interbufc {
	class Document;

	enum class CompilationErrorKind : int {
		OutOfMemory = 0,
		ErrorOpeningFile,
		IO,
		InvalidTypeName,
		InvalidEnumBaseType,
		EnumTypeNotIncrementable
	};

	class TypeNameNode;
	class ModuleNode;
	class FnOverloadingNode;

	struct ErrorOpeningFileError {
		peff::String name;

		INTERBUFC_FORCEINLINE ErrorOpeningFileError(peff::Alloc *allocator) : name(allocator) {}
		INTERBUFC_FORCEINLINE ErrorOpeningFileError(ErrorOpeningFileError &&rhs) : name(std::move(rhs.name)) {}
		~ErrorOpeningFileError() = default;
	};

	struct CompilationError {
		TokenRange token_range;
		CompilationErrorKind error_kind;
		std::variant<std::monostate, ErrorOpeningFileError> ex_data;

		INTERBUFC_FORCEINLINE CompilationError(
			const TokenRange &token_range,
			CompilationErrorKind error_kind)
			: token_range(token_range),
			  error_kind(error_kind) {
			assert(token_range);
		}

		INTERBUFC_FORCEINLINE CompilationError(
			const TokenRange &token_range,
			ErrorOpeningFileError &&error)
			: token_range(token_range),
			  error_kind(CompilationErrorKind::ErrorOpeningFile),
			  ex_data(std::move(error)) {
			assert(token_range);
		}

		INTERBUFC_FORCEINLINE bool operator<(const CompilationError &rhs) const noexcept {
			return token_range < rhs.token_range;
		}

		INTERBUFC_FORCEINLINE bool operator>(const CompilationError &rhs) const noexcept {
			return token_range > rhs.token_range;
		}
	};

#define INTERBUFC_RETURN_IF_COMP_ERROR(...)                                \
	if (std::optional<interbufc::CompilationError> _ = (__VA_ARGS__); _) { \
		return _;                                                          \
	} else

	enum class CompilationWarningKind : int {
		UnusedExprResult = 0,
	};

	INTERBUFC_FORCEINLINE CompilationError gen_out_of_memory_comp_error() {
		return CompilationError(TokenRange{ 0, 0 }, CompilationErrorKind::OutOfMemory);
	}

	INTERBUFC_FORCEINLINE CompilationError gen_io_comp_error() {
		return CompilationError(TokenRange{ 0, 0 }, CompilationErrorKind::IO);
	}

	struct CompilationWarning {
		TokenRange token_range;
		CompilationWarningKind warning_kind;
		std::variant<std::monostate> ex_data;

		INTERBUFC_FORCEINLINE CompilationWarning(
			const TokenRange &token_range,
			CompilationWarningKind warning_kind)
			: token_range(token_range),
			  warning_kind(warning_kind) {
		}
	};

	class MemberNode;

	class ExternalModuleProvider;

	class Document : public peff::SharedFromThis<Document> {
	private:
		INTERBUFC_API void _do_clear_deferred_destructible_ast_nodes();

	public:
		peff::RcObjectPtr<peff::Alloc> allocator;
		AstNodePtr<ModuleNode> root_module;
		peff::DynArray<AstNodePtr<ExternalModuleProvider>> external_module_providers;

		AstNode *destructible_ast_node_list = nullptr;

		INTERBUFC_API Document(peff::Alloc *allocator);
		INTERBUFC_API virtual ~Document();

		INTERBUFC_FORCEINLINE void clear_deferred_destructible_ast_nodes() {
			if (destructible_ast_node_list) {
				_do_clear_deferred_destructible_ast_nodes();
			}
		}
	};
}

#endif
