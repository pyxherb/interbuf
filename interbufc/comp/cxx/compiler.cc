#include "compiler.h"

using namespace interbufc;

INTERBUFC_API CXXCompiler::~CXXCompiler() {
}

INTERBUFC_API std::optional<CompilationError> CXXCompiler::compile(
	AstNodePtr<ModuleNode> mod) {
	peff::Set<size_t> structs(allocator.get());
	peff::Set<size_t> classes(allocator.get());
	peff::Set<size_t> enums(allocator.get());

	for (size_t i = 0; i < mod->members.size(); ++i) {
		switch (mod->members.at(i)->astNodeType) {
			case AstNodeType::Struct:
				if (!structs.insert(+i))
					return genOutOfMemoryCompError();
				break;
			case AstNodeType::Class:
				if (!classes.insert(+i))
					return genOutOfMemoryCompError();
				break;
			case AstNodeType::Enum:
				if (!enums.insert(+i))
					return genOutOfMemoryCompError();
				break;
			default:
				std::terminate();
		}
	}
}
