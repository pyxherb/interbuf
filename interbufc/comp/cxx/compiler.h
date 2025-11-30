#ifndef _INTERBUFC_COMP_CXX_COMPILER_H_
#define _INTERBUFC_COMP_CXX_COMPILER_H_

#include "../compiler.h"

namespace interbufc {
	class CXXCompiler : public Compiler {
	public:
		INTERBUFC_API virtual ~CXXCompiler();

		INTERBUFC_API virtual std::optional<CompilationError> compile(
			AstNodePtr<ModuleNode> mod) override;
	};
}

#endif
