#include "compiler.h"
#include <interbufc/comp/util.h>
#include <cctype>

using namespace interbufc;

INTERBUFC_API CXXCompiler::CXXCompiler(peff::Alloc *allocator) : Compiler(allocator) {
}

INTERBUFC_API CXXCompiler::~CXXCompiler() {
}

static std::optional<interbufc::CompilationError> _writeIndent(interbufc::File &file, size_t indent);
static std::optional<interbufc::CompilationError> _writeIdRef(interbufc::File &file, IdRef *idRef);
static std::optional<interbufc::CompilationError> _writeInternalStorageName(interbufc::File &file, const std::string_view &name);
static std::optional<interbufc::CompilationError> _writeInternalStorageTypeName(interbufc::File &file, AstNodePtr<TypeNameNode> typeName);

static std::optional<interbufc::CompilationError> _writeIndent(interbufc::File &file, size_t indent) {
	for (size_t i = 0; i < indent; ++i)
		INTERBUFC_RETURN_IF_COMP_ERROR(file.write("\t"));
	return {};
};

static std::optional<interbufc::CompilationError> _writeIdRef(interbufc::File &file, IdRef *idRef) {
	for (size_t i = 0; i < idRef->entries.size(); ++i) {
		if (i)
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("::"));
		INTERBUFC_RETURN_IF_COMP_ERROR(file.write(idRef->entries.at(i).name));
	}
	return {};
};

static std::optional<interbufc::CompilationError> _writeInternalStorageName(interbufc::File& file, const std::string_view& name) {
	INTERBUFC_RETURN_IF_COMP_ERROR(file.write("_generated_"));
	INTERBUFC_RETURN_IF_COMP_ERROR(file.write(name));
	return {};
}

static std::optional<interbufc::CompilationError> _writeMethodVarName(interbufc::File &file, const std::string_view &name) {
	INTERBUFC_RETURN_IF_COMP_ERROR(file.write(name));
	return {};
}

static std::optional<interbufc::CompilationError> _writeInternalStorageTypeName(interbufc::File &file, AstNodePtr<TypeNameNode> typeName) {
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
		case TypeNameKind::String:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("peff::String"));
			break;
		case TypeNameKind::Bool:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("bool"));
			break;
		case TypeNameKind::Custom: {
			AstNodePtr<MemberNode> m;

			INTERBUFC_RETURN_IF_COMP_ERROR(resolveCustomTypeName(typeName.castTo<CustomTypeNameNode>(), m));

			if (!m)
				return CompilationError(typeName->tokenRange, CompilationErrorKind::InvalidTypeName);

			switch (m->astNodeType) {
				case AstNodeType::Class:
					INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::ObjectPtr<interbuf::ClassBase>"));
					break;
				case AstNodeType::Struct:
					INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::ObjectPtr<interbuf::StructBase>"));
					break;
				case AstNodeType::Enum:
					INTERBUFC_RETURN_IF_COMP_ERROR(_writeIdRef(file, typeName.castTo<CustomTypeNameNode>()->idRefPtr.get()));
					break;
				default:
					std::terminate();
			}
			break;
		}
		default:
			std::terminate();
	}
	return {};
}

static std::optional<interbufc::CompilationError> _writeStorageTypeName(interbufc::File &file, AstNodePtr<TypeNameNode> typeName) {
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
		case TypeNameKind::String:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("peff::String"));
			break;
		case TypeNameKind::Bool:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("bool"));
			break;
		case TypeNameKind::Custom: {
			AstNodePtr<MemberNode> m;

			INTERBUFC_RETURN_IF_COMP_ERROR(resolveCustomTypeName(typeName.castTo<CustomTypeNameNode>(), m));

			if (!m)
				return CompilationError(typeName->tokenRange, CompilationErrorKind::InvalidTypeName);

			switch (m->astNodeType) {
				case AstNodeType::Class:
				case AstNodeType::Struct:
					INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::ObjectPtr<"));
					INTERBUFC_RETURN_IF_COMP_ERROR(_writeIdRef(file, typeName.castTo<CustomTypeNameNode>()->idRefPtr.get()));
					INTERBUFC_RETURN_IF_COMP_ERROR(file.write(">"));
					break;
				case AstNodeType::Enum:
					INTERBUFC_RETURN_IF_COMP_ERROR(_writeIdRef(file, typeName.castTo<CustomTypeNameNode>()->idRefPtr.get()));
					break;
				default:
					std::terminate();
			}
			break;
		}
		default:
			std::terminate();
	}
	return {};
}

static std::optional<interbufc::CompilationError> _writeDirectTypeName(interbufc::File &file, AstNodePtr<TypeNameNode> typeName) {
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
		case TypeNameKind::String:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("peff::String"));
			break;
		case TypeNameKind::Bool:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("bool"));
			break;
		case TypeNameKind::Custom: {
			AstNodePtr<MemberNode> m;

			INTERBUFC_RETURN_IF_COMP_ERROR(resolveCustomTypeName(typeName.castTo<CustomTypeNameNode>(), m));

			if (!m)
				return CompilationError(typeName->tokenRange, CompilationErrorKind::InvalidTypeName);

			switch (m->astNodeType) {
				case AstNodeType::Class:
				case AstNodeType::Struct:
				case AstNodeType::Enum:
					INTERBUFC_RETURN_IF_COMP_ERROR(_writeIdRef(file, typeName.castTo<CustomTypeNameNode>()->idRefPtr.get()));
					break;
				default:
					std::terminate();
			}
			break;
		}
		default:
			std::terminate();
	}
	return {};
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

	peff::DynArray<size_t> structs(allocator.get());
	peff::DynArray<size_t> classes(allocator.get());
	peff::DynArray<size_t> enums(allocator.get());

	for (size_t i = 0; i < mod->members.size(); ++i) {
		auto m = mod->members.at(i);
		switch (m->astNodeType) {
			case AstNodeType::Struct:
				if (!structs.pushBack(+i))
					return genOutOfMemoryCompError();
				break;
			case AstNodeType::Class:
				if (!classes.pushBack(+i))
					return genOutOfMemoryCompError();
				break;
			case AstNodeType::Enum:
				if (!enums.pushBack(+i))
					return genOutOfMemoryCompError();
				break;
			default:
				std::terminate();
		}
	}

	size_t namespaceScopes = mod->namespacePath ? mod->namespacePath->entries.size() : 0;
	size_t indent = namespaceScopes;

	//
	// Write the header file.
	//
	for (size_t i = 0; i < namespaceScopes; ++i) {
		INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, i));
		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("namespace "));
		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(mod->namespacePath->entries.at(i).name));
		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("{\n"));
	}

	for (size_t i = 0; i < enums.size(); ++i) {
		AstNodePtr<EnumNode> curEnum = mod->members.at(enums.at(i)).castTo<EnumNode>();

		switch (curEnum->baseType->typeNameKind) {
			case TypeNameKind::I8:
			case TypeNameKind::I16:
			case TypeNameKind::I32:
			case TypeNameKind::I64:
			case TypeNameKind::U8:
			case TypeNameKind::U16:
			case TypeNameKind::U32:
			case TypeNameKind::U64:
			case TypeNameKind::F32:
			case TypeNameKind::F64:
			case TypeNameKind::Bool: {
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent));

				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("enum "));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(curEnum->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(" {\n"));

				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent));

				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("};\n"));
				break;
			}
			case TypeNameKind::String: {
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("using "));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(curEnum->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(" = peff::String\n"));
				break;
			}
			case TypeNameKind::Custom:
				return CompilationError(curEnum->baseType->tokenRange, CompilationErrorKind::InvalidEnumBaseType);
			default:
				std::terminate();
		}
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("\n"));

	for (size_t i = 0; i < structs.size(); ++i) {
		AstNodePtr<StructNode> curStruct = mod->members.at(structs.at(i)).castTo<StructNode>();

		INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent));

		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("struct "));
		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(curStruct->name));
		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(";\n"));
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("\n"));

	for (size_t i = 0; i < classes.size(); ++i) {
		AstNodePtr<ClassNode> curClass = mod->members.at(classes.at(i)).castTo<ClassNode>();

		INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent));

		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("class "));
		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(curClass->name));
		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(";\n"));
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("\n"));

	for (size_t i = 0; i < structs.size(); ++i) {
		AstNodePtr<StructNode> curStruct = mod->members.at(structs.at(i)).castTo<StructNode>();

		INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent));

		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("struct "));
		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(curStruct->name));
		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(" {\n"));

		for (size_t j = 0; j < curStruct->members.size(); ++j) {
			AstNodePtr<VarNode> curVar = curStruct->members.at(j).castTo<VarNode>();

			assert(curVar->astNodeType == AstNodeType::Var);

			if (j)
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("\n"));

			INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent + 1));

			INTERBUFC_RETURN_IF_COMP_ERROR(_writeInternalStorageTypeName(headerFileOut, curVar->type));
			INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(" "));
			INTERBUFC_RETURN_IF_COMP_ERROR(_writeInternalStorageName(headerFileOut, curVar->name));
			INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(";\n"));

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent + 1));

				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("INTERBUF_FORCEINLINE "));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeStorageTypeName(headerFileOut, curVar->type));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(" get_"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeMethodVarName(headerFileOut, curVar->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("();\n"));
			}

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent + 1));

				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("INTERBUF_FORCEINLINE void set_"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeMethodVarName(headerFileOut, curVar->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("("));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeStorageTypeName(headerFileOut, curVar->type));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(" data);\n"));
			}
		}

		INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent));

		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("};\n"));
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("\n"));

	for (size_t i = 0; i < classes.size(); ++i) {
		AstNodePtr<ClassNode> curClass = mod->members.at(classes.at(i)).castTo<ClassNode>();

		INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent));

		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("class "));
		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(curClass->name));
		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(" {\n"));

		INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent));
		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("public:\n"));

		for (size_t j = 0; j < curClass->members.size(); ++j) {
			AstNodePtr<VarNode> curVar = curClass->members.at(j).castTo<VarNode>();

			assert(curVar->astNodeType == AstNodeType::Var);

			if (j)
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("\n"));

			INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent + 1));

			INTERBUFC_RETURN_IF_COMP_ERROR(_writeInternalStorageTypeName(headerFileOut, curVar->type));
			INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(" "));
			INTERBUFC_RETURN_IF_COMP_ERROR(_writeInternalStorageName(headerFileOut, curVar->name));
			INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(";\n"));

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent + 1));

				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("INTERBUF_FORCEINLINE "));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeStorageTypeName(headerFileOut, curVar->type));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(" get_"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeMethodVarName(headerFileOut, curVar->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("();\n"));
			}

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent + 1));

				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("INTERBUF_FORCEINLINE void set_"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeMethodVarName(headerFileOut, curVar->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("("));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeStorageTypeName(headerFileOut, curVar->type));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(" data);\n"));
			}
		}

		INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent));

		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("};\n"));
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("\n"));

	for (size_t i = 0; i < structs.size(); ++i) {
		AstNodePtr<StructNode> curStruct = mod->members.at(structs.at(i)).castTo<StructNode>();

		for (size_t j = 0; j < curStruct->members.size(); ++j) {
			AstNodePtr<VarNode> curVar = curStruct->members.at(j).castTo<VarNode>();

			assert(curVar->astNodeType == AstNodeType::Var);

			if (j)
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("\n"));

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent));

				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("INTERBUF_FORCEINLINE "));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeStorageTypeName(headerFileOut, curVar->type));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(" "));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(curStruct->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("::get_"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeMethodVarName(headerFileOut, curVar->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("() {\n"));

				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent + 1));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("return "));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeInternalStorageName(headerFileOut, curVar->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(";\n"));

				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("}\n"));
			}

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent));

				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("INTERBUF_FORCEINLINE void "));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(curStruct->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("::set_"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeMethodVarName(headerFileOut, curVar->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("("));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeStorageTypeName(headerFileOut, curVar->type));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(" data) {\n"));

				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent + 1));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("this->"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeInternalStorageName(headerFileOut, curVar->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(" = data;\n"));

				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("}\n"));
			}
		}
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("\n"));

	for (size_t i = 0; i < classes.size(); ++i) {
		AstNodePtr<ClassNode> curClass = mod->members.at(classes.at(i)).castTo<ClassNode>();

		for (size_t j = 0; j < curClass->members.size(); ++j) {
			AstNodePtr<VarNode> curVar = curClass->members.at(j).castTo<VarNode>();

			assert(curVar->astNodeType == AstNodeType::Var);

			if (j)
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("\n"));

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent));

				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("INTERBUF_FORCEINLINE "));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeStorageTypeName(headerFileOut, curVar->type));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(" "));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(curClass->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("::get_"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeMethodVarName(headerFileOut, curVar->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("() {\n"));

				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent + 1));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("return "));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeInternalStorageName(headerFileOut, curVar->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(";\n"));

				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("}\n"));
			}

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent));

				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("INTERBUF_FORCEINLINE void "));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(curClass->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("::set_"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeMethodVarName(headerFileOut, curVar->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("("));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeStorageTypeName(headerFileOut, curVar->type));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(" data) {\n"));

				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent + 1));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("this->"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeInternalStorageName(headerFileOut, curVar->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write(" = data;\n"));

				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, indent));
				INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("}\n"));
			}
		}
	}

	for (size_t i = 0; i < namespaceScopes; ++i) {
		INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(headerFileOut, namespaceScopes - i - 1));
		INTERBUFC_RETURN_IF_COMP_ERROR(headerFileOut.write("}\n"));
	}

	return {};
}
