#include "compiler.h"
#include <interbufc/comp/util.h>

using namespace interbufc;

INTERBUFC_API CXXCompiler::~CXXCompiler() {
}

INTERBUFC_API std::optional<CompilationError> CXXCompiler::compile(
	AstNodePtr<ModuleNode> mod) {
	File headerFileOut, sourceFileOut;
	{
		peff::String path(allocator.get());

		if (!path.build(g_outputDirectoryPath.data()))
			return genOutOfMemoryCompError();

		if (!path.append("/"))
			return genOutOfMemoryCompError();

		if (!path.append(g_outputFileName))
			return genOutOfMemoryCompError();

		const size_t prefixLen = path.size();

		if (!path.append(".h"))
			return genOutOfMemoryCompError();

		FILE *fp;

		if (!(fp = fopen(path.data(), "wb"))) {
			ErrorOpeningFileError e(allocator.get());

			if (!e.name.build(path))
				return genOutOfMemoryCompError();

			return CompilationError({ 0, 0 }, std::move(e));
		}

		headerFileOut.setCFile(fp);

		if (!path.resize(prefixLen))
			return genOutOfMemoryCompError();

		if (!path.append(".cc"))
			return genOutOfMemoryCompError();

		if (!(fp = fopen(path.data(), "wb"))) {
			ErrorOpeningFileError e(allocator.get());

			if (!e.name.build(path))
				return genOutOfMemoryCompError();

			return CompilationError({ 0, 0 }, std::move(e));
		}

		sourceFileOut.setCFile(fp);
	}

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
