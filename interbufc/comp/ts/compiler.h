#ifndef _INTERBUFC_COMP_TS_COMPILER_H_
#define _INTERBUFC_COMP_TS_COMPILER_H_

#include "../compiler.h"

namespace interbufc {
	class TypeScriptCompiler : public Compiler {
	public:
		INTERBUFC_API TypeScriptCompiler(peff::Alloc *allocator);
		INTERBUFC_API virtual ~TypeScriptCompiler();

		INTERBUFC_API virtual std::optional<CompilationError> compile(
			AstNodePtr<ModuleNode> mod) override;
	};
}

#endif
