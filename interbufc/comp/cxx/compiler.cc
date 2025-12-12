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

	size_t indent = mod->namespacePath->entries.size();

	auto writeIndent = [](interbufc::File &file, size_t indent) -> std::optional<interbufc::CompilationError> {
		for (size_t i = 0; i < indent; ++i)
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("\t"));
		return {};
	};

	auto writeTypeName = [](interbufc::File &file, AstNodePtr<TypeNameNode> typeName) -> std::optional<interbufc::CompilationError> {
		switch (typeName->typeNameKind) {
			case TypeNameKind::I8:
				INTERBUFC_RETURN_IF_COMP_ERROR(file.write("int8_t"));
				break;
			case TypeNameKind::I16:
				INTERBUFC_RETURN_IF_COMP_ERROR(file.write("int16_t"));
				break;
			case TypeNameKind::I32:
				INTERBUFC_RETURN_IF_COMP_ERROR(file.write("int32_t"));
				break;
			case TypeNameKind::I64:
				INTERBUFC_RETURN_IF_COMP_ERROR(file.write("int64_t"));
				break;
			case TypeNameKind::U8:
				INTERBUFC_RETURN_IF_COMP_ERROR(file.write("uint8_t"));
				break;
			case TypeNameKind::U16:
				INTERBUFC_RETURN_IF_COMP_ERROR(file.write("uint16_t"));
				break;
			case TypeNameKind::U32:
				INTERBUFC_RETURN_IF_COMP_ERROR(file.write("uint32_t"));
				break;
			case TypeNameKind::U64:
				INTERBUFC_RETURN_IF_COMP_ERROR(file.write("uint64_t"));
				break;
			case TypeNameKind::F32:
				INTERBUFC_RETURN_IF_COMP_ERROR(file.write("float"));
				break;
			case TypeNameKind::F64:
				INTERBUFC_RETURN_IF_COMP_ERROR(file.write("double"));
				break;
			case TypeNameKind::Object:
				INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::ObjectPtr<interbuf::Object>"));
				break;
			case TypeNameKind::String:
				INTERBUFC_RETURN_IF_COMP_ERROR(file.write("peff::String"));
				break;
			case TypeNameKind::Bool:
				INTERBUFC_RETURN_IF_COMP_ERROR(file.write("bool"));
				break;
			default:
				std::terminate();
		}
		return {};
	};

	//
	// Write the header file.
	//
	for (size_t i = 0; i < mod->namespacePath->entries.size(); ++i) {
		INTERBUFC_RETURN_IF_COMP_ERROR(writeIndent(headerFileOut, i));
		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("namespace {"));
		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(mod->namespacePath->entries.at(i).name));
	}

	for (size_t i = 0; i < structs.size(); ++i) {
		AstNodePtr<StructNode> curStruct = mod->members.at(structs.at(i)).castTo<StructNode>();

		for (size_t j = 0; j < curStruct->members.size(); ++j) {
			AstNodePtr<VarNode> curVar = curStruct->members.at(structs.at(i)).castTo<VarNode>();

			assert(curVar->astNodeType == AstNodeType::Var);

			switch (curVar->type->typeNameKind) {

			}
		}
	}

	for (size_t i = 0; i < classes.size(); ++i) {
		AstNodePtr<ClassNode> curClass = mod->members.at(structs.at(i)).castTo<ClassNode>();
	}

	for (size_t i = 0; i < enums.size(); ++i) {
		AstNodePtr<EnumNode> curEnum = mod->members.at(structs.at(i)).castTo<EnumNode>();
	}

	for (size_t i = 0; i < mod->namespacePath->entries.size(); ++i) {
		INTERBUFC_RETURN_IF_COMP_ERROR(writeIndent(headerFileOut, i));
		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("}"));
	}
}
