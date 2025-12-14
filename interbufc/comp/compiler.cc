#include "compiler.h"

using namespace interbufc;

std::string_view interbufc::g_language, interbufc::g_sourceFileName, interbufc::g_outputDirectoryPath, interbufc::g_outputFileName;

INTERBUFC_API Compiler::Compiler(peff::Alloc *allocator) : warnings(allocator), errors(allocator), allocator(allocator) {
}

INTERBUFC_API Compiler::~Compiler() {
}

std::optional<CompilationError> interbufc::resolveCustomTypeName(AstNodePtr<CustomTypeNameNode> typeName, AstNodePtr<MemberNode> &memberOut) {
	memberOut = {};

	AstNodePtr<MemberNode> m = typeName->contextNode.lock();

	if (typeName->cachedResolveResult.isValid()) {
		memberOut = typeName->cachedResolveResult.lock();
		return {};
	}

	for (size_t i = 0; i < typeName->idRefPtr->entries.size(); ++i) {
		IdRefEntry &entry = typeName->idRefPtr->entries.at(i);

		switch (m->astNodeType) {
			case AstNodeType::Module: {
				AstNodePtr<ModuleNode> member = m.castTo<ModuleNode>();

				if (auto it = member->memberIndices.find(entry.name); it != member->memberIndices.end()) {
					m = member->members.at(it.value());
				} else
					return {};
				break;
			}
			case AstNodeType::Class: {
				AstNodePtr<ClassNode> member = m.castTo<ClassNode>();

				if (auto it = member->memberIndices.find(entry.name); it != member->memberIndices.end()) {
					m = member->members.at(it.value());
				} else
					return {};
				break;
			}
			case AstNodeType::Struct: {
				AstNodePtr<StructNode> member = m.castTo<StructNode>();

				if (auto it = member->memberIndices.find(entry.name); it != member->memberIndices.end()) {
					m = member->members.at(it.value());
				} else
					return {};
				break;
			}
			default:
				return {};
		}
	}

	memberOut = m;
	typeName->cachedResolveResult = m;

	return {};
}
