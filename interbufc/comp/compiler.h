#ifndef _INTERBUFC_COMP_COMPILER_H_
#define _INTERBUFC_COMP_COMPILER_H_

#include <interbufc/ast/parser.h>

namespace interbufc {
	class CompilerWriter {
	public:
		INTERBUFC_API virtual ~CompilerWriter();

		[[nodisacrd]] virtual bool write(const char *s, size_t len) noexcept = 0;

		[[nodiscard]] bool write(const std::string_view& sv) noexcept {
			return write(sv.data(), sv.size());
		}
	};

	class Compiler {
	public:
		peff::DynArray<CompilationError> errors;

		INTERBUFC_API virtual ~Compiler();

		[[nodiscard]] INTERBUFC_FORCEINLINE std::optional<CompilationError> pushError(CompilationError &&error) noexcept {
			if (!errors.pushBack(std::move(error)))
				return genOutOfMemoryCompError();

			return {};
		}

		[[nodiscard]] virtual std::optional<CompilationError> compile(
			CompilerWriter *writer,
			AstNodePtr<ModuleNode> mod) = 0;
	};
}

#endif
