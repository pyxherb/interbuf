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
	};

	class TypeNameNode;
	class ModuleNode;
	class FnOverloadingNode;

	struct CompilationError {
		TokenRange tokenRange;
		CompilationErrorKind errorKind;
		std::variant<std::monostate> exData;

		INTERBUFC_FORCEINLINE CompilationError(
			const TokenRange &tokenRange,
			CompilationErrorKind errorKind)
			: tokenRange(tokenRange),
			  errorKind(errorKind) {
			assert(tokenRange);
		}

		INTERBUFC_FORCEINLINE bool operator<(const CompilationError &rhs) const noexcept {
			return tokenRange < rhs.tokenRange;
		}

		INTERBUFC_FORCEINLINE bool operator>(const CompilationError &rhs) const noexcept {
			return tokenRange > rhs.tokenRange;
		}
	};

#define INTERBUFC_RETURN_IF_COMP_ERROR(...) \
	if (std::optional<interbufc::CompilationError> _ = (__VA_ARGS__); _) return _

	enum class CompilationWarningKind : int {
		UnusedExprResult = 0,
	};

	INTERBUFC_FORCEINLINE CompilationError genOutOfMemoryCompError() {
		return CompilationError(TokenRange{ 0, 0 }, CompilationErrorKind::OutOfMemory);
	}

	struct CompilationWarning {
		TokenRange tokenRange;
		CompilationWarningKind warningKind;
		std::variant<std::monostate> exData;

		INTERBUFC_FORCEINLINE CompilationWarning(
			const TokenRange &tokenRange,
			CompilationWarningKind warningKind)
			: tokenRange(tokenRange),
			  warningKind(warningKind) {
		}
	};

	class MemberNode;

	class ExternalModuleProvider;

	class Document : public peff::SharedFromThis<Document> {
	private:
		INTERBUFC_API void _doClearDeferredDestructibleAstNodes();

	public:
		peff::RcObjectPtr<peff::Alloc> allocator;
		AstNodePtr<ModuleNode> rootModule;
		peff::DynArray<AstNodePtr<ExternalModuleProvider>> externalModuleProviders;

		AstNode *destructibleAstNodeList = nullptr;

		INTERBUFC_API Document(peff::Alloc *allocator);
		INTERBUFC_API virtual ~Document();

		INTERBUFC_FORCEINLINE void clearDeferredDestructibleAstNodes() {
			if (destructibleAstNodeList) {
				_doClearDeferredDestructibleAstNodes();
			}
		}
	};
}

#endif
