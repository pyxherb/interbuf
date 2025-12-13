#ifndef _INTERBUFC_COMP_COMPILER_H_
#define _INTERBUFC_COMP_COMPILER_H_

#include <interbufc/ast/parser.h>
#include <interbufc/ast/typename.h>

namespace interbufc {
	extern std::string_view g_language, g_sourceFileName, g_outputDirectoryPath, g_outputFileName;

	class Compiler {
	public:
		peff::RcObjectPtr<peff::Alloc> allocator;
		peff::DynArray<CompilationError> errors;
		peff::DynArray<CompilationWarning> warnings;

		INTERBUFC_API Compiler(peff::Alloc *allocator);
		INTERBUFC_API virtual ~Compiler();

		[[nodiscard]] INTERBUFC_FORCEINLINE std::optional<CompilationError> pushError(CompilationError &&error) noexcept {
			if (!errors.pushBack(std::move(error)))
				return genOutOfMemoryCompError();

			return {};
		}

		[[nodiscard]] INTERBUFC_FORCEINLINE std::optional<CompilationError> pushWarning(CompilationWarning &&warning) noexcept {
			if (!warnings.pushBack(std::move(warning)))
				return genOutOfMemoryCompError();

			return {};
		}

		[[nodiscard]] virtual std::optional<CompilationError> compile(
			AstNodePtr<ModuleNode> mod) = 0;
	};
}

#define INTERBUFC_RETURN_EXCEPT_IF_WRITE_FAILED(allocator, e)         \
	if (!(e))                                                        \
		return interbufc::genIOCompError(); \
	else                                                             \
		;

#endif
