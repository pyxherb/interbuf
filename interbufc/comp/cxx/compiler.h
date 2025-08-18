#ifndef _INTERBUFC_COMP_CXX_COMPILER_H_
#define _INTERBUFC_COMP_CXX_COMPILER_H_

#include "../compiler.h"

namespace interbufc {
	class CXXCompiler {
	public:
		INTERBUFC_API virtual ~CXXCompiler();

		INTERBUFC_API virtual std::optional<CompilationError> compile(
			CompilerWriter *writer,
			AstNodePtr<ModuleNode> mod) = 0;
	};
}

#endif
