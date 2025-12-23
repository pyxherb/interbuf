#include "compiler.h"
#include <interbufc/comp/util.h>
#include <cctype>

using namespace interbufc;

INTERBUFC_API TypeScriptCompiler::TypeScriptCompiler(peff::Alloc *allocator) : Compiler(allocator) {
}

INTERBUFC_API TypeScriptCompiler::~TypeScriptCompiler() {
}

static std::optional<interbufc::CompilationError> _writeIndent(interbufc::File &file, size_t indent);
static std::optional<interbufc::CompilationError> _writeIdRef(interbufc::File &file, IdRef *idRef);
static std::optional<interbufc::CompilationError> _writeInternalStorageName(interbufc::File &file, const std::string_view &name);
static std::optional<interbufc::CompilationError> _writeMethodVarName(interbufc::File &file, const std::string_view &name);
static std::optional<interbufc::CompilationError> _writeInternalStorageTypeName(interbufc::File &file, AstNodePtr<TypeNameNode> typeName);
static std::optional<interbufc::CompilationError> _writeTypeLayoutName(interbufc::File &file, const std::string_view &name);
static std::optional<interbufc::CompilationError> _writeDocumentVarName(interbufc::File &file);
static std::optional<interbufc::CompilationError> _writeInternalStorageTypeName(interbufc::File &file, AstNodePtr<TypeNameNode> typeName);
static std::optional<interbufc::CompilationError> _writeInterbufTypeNameInstanceInitValue(interbufc::File &file, AstNodePtr<TypeNameNode> typeName);
static std::optional<interbufc::CompilationError> _writeExpr(interbufc::File &file, AstNodePtr<ExprNode> expr);

static std::optional<interbufc::CompilationError> _writeIndent(interbufc::File &file, size_t indent) {
	assert(indent < UINT32_MAX);
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

static std::optional<interbufc::CompilationError> _writeInternalStorageName(interbufc::File &file, const std::string_view &name) {
	INTERBUFC_RETURN_IF_COMP_ERROR(file.write("_generated_"));
	INTERBUFC_RETURN_IF_COMP_ERROR(file.write(name));
	return {};
}

static std::optional<interbufc::CompilationError> _writeMethodVarName(interbufc::File &file, const std::string_view &name) {
	char s[2] = { name[0], '\0' };
	INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s, sizeof(s) - 1));
	INTERBUFC_RETURN_IF_COMP_ERROR(file.write(name.substr(1)));
	return {};
}

static std::optional<interbufc::CompilationError> _writeTypeLayoutName(interbufc::File &file, const std::string_view &name) {
	INTERBUFC_RETURN_IF_COMP_ERROR(file.write("generated_"));
	INTERBUFC_RETURN_IF_COMP_ERROR(file.write(name));
	INTERBUFC_RETURN_IF_COMP_ERROR(file.write("_layout"));
	return {};
}

static std::optional<interbufc::CompilationError> _writeDocumentVarName(interbufc::File &file) {
	INTERBUFC_RETURN_IF_COMP_ERROR(file.write("generated_document"));
	return {};
}

static std::optional<interbufc::CompilationError> _writeInternalStorageTypeName(interbufc::File &file, AstNodePtr<TypeNameNode> typeName) {
	switch (typeName->typeNameKind) {
		case TypeNameKind::I8:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("number"));
			break;
		case TypeNameKind::I16:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("number"));
			break;
		case TypeNameKind::I32:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("number"));
			break;
		case TypeNameKind::I64:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("bigint"));
			break;
		case TypeNameKind::U8:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("number"));
			break;
		case TypeNameKind::U16:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("number"));
			break;
		case TypeNameKind::U32:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("number"));
			break;
		case TypeNameKind::U64:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("bigint"));
			break;
		case TypeNameKind::F32:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("number"));
			break;
		case TypeNameKind::F64:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("number"));
			break;
		case TypeNameKind::String:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("string"));
			break;
		case TypeNameKind::Bool:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("boolean"));
			break;
		case TypeNameKind::Custom:
			INTERBUFC_RETURN_IF_COMP_ERROR(_writeIdRef(file, typeName.castTo<CustomTypeNameNode>()->idRefPtr.get()));
			break;
		default:
			std::terminate();
	}
	return {};
}

static std::optional<interbufc::CompilationError> _writeInterbufTypeNameInstanceInitValue(interbufc::File &file, AstNodePtr<TypeNameNode> typeName) {
redump:
	switch (typeName->typeNameKind) {
		case TypeNameKind::I8:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::DataType{interbuf::FieldTypeKind::I8}"));
			break;
		case TypeNameKind::I16:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::DataType{interbuf::FieldTypeKind::I16}"));
			break;
		case TypeNameKind::I32:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::DataType{interbuf::FieldTypeKind::I32}"));
			break;
		case TypeNameKind::I64:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::DataType{interbuf::FieldTypeKind::I64}"));
			break;
		case TypeNameKind::U8:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::DataType{interbuf::FieldTypeKind::U8}"));
			break;
		case TypeNameKind::U16:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::DataType{interbuf::FieldTypeKind::U16}"));
			break;
		case TypeNameKind::U32:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::DataType{interbuf::FieldTypeKind::U32}"));
			break;
		case TypeNameKind::U64:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::DataType{interbuf::FieldTypeKind::U64}"));
			break;
		case TypeNameKind::F32:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::DataType{interbuf::FieldTypeKind::F32}"));
			break;
		case TypeNameKind::F64:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::DataType{interbuf::FieldTypeKind::F64}"));
			break;
		case TypeNameKind::String:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::DataType{interbuf::FieldTypeKind::String}"));
			break;
		case TypeNameKind::Bool:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::DataType{interbuf::FieldTypeKind::Bool}"));
			break;
		case TypeNameKind::Custom: {
			AstNodePtr<MemberNode> m;

			INTERBUFC_RETURN_IF_COMP_ERROR(resolveCustomTypeName(typeName.castTo<CustomTypeNameNode>(), m));

			if (!m)
				return CompilationError(typeName->tokenRange, CompilationErrorKind::InvalidTypeName);

			switch (m->astNodeType) {
				case AstNodeType::Class:
					INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::DataType{interbuf::FieldTypeKind::Class}"));
					break;
				case AstNodeType::Struct:
					INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::DataType{interbuf::FieldTypeKind::Struct}"));
					break;
				case AstNodeType::Enum:
					typeName = m.castTo<EnumNode>()->baseType;
					goto redump;
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

static std::optional<interbufc::CompilationError> _writeExpr(interbufc::File &file, AstNodePtr<ExprNode> expr) {
redump:
	switch (expr->exprKind) {
		case ExprKind::I8: {
			AstNodePtr<I8LiteralExprNode> e = expr.castTo<I8LiteralExprNode>();

			char s[8];
			sprintf(s, "%hd", (int16_t)e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::I16: {
			AstNodePtr<I16LiteralExprNode> e = expr.castTo<I16LiteralExprNode>();

			char s[16];
			sprintf(s, "%hd", e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::I32: {
			AstNodePtr<I32LiteralExprNode> e = expr.castTo<I32LiteralExprNode>();

			char s[32];
			sprintf(s, "%d", e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::I64: {
			AstNodePtr<I64LiteralExprNode> e = expr.castTo<I64LiteralExprNode>();

			char s[48];
			sprintf(s, "%lld", e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::U8: {
			AstNodePtr<U8LiteralExprNode> e = expr.castTo<U8LiteralExprNode>();

			char s[4];
			sprintf(s, "%u", (uint16_t)e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::U16: {
			AstNodePtr<U16LiteralExprNode> e = expr.castTo<U16LiteralExprNode>();

			char s[8];
			sprintf(s, "%hu", e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::U32: {
			AstNodePtr<U32LiteralExprNode> e = expr.castTo<U32LiteralExprNode>();

			char s[16];
			sprintf(s, "%u", e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::U64: {
			AstNodePtr<U64LiteralExprNode> e = expr.castTo<U64LiteralExprNode>();

			char s[32];
			sprintf(s, "%llu", e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::F32: {
			AstNodePtr<F32LiteralExprNode> e = expr.castTo<F32LiteralExprNode>();

			char s[16];
			sprintf(s, "%f", e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::F64: {
			AstNodePtr<F64LiteralExprNode> e = expr.castTo<F64LiteralExprNode>();

			char s[32];
			sprintf(s, "%f", e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::Bool: {
			AstNodePtr<BoolLiteralExprNode> e = expr.castTo<BoolLiteralExprNode>();

			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(e->data ? "true" : "false"));
			break;
		}
		default:
			std::terminate();
	}

	return {};
}

INTERBUFC_API std::optional<CompilationError> TypeScriptCompiler::compile(
	AstNodePtr<ModuleNode> mod) {
	File sourceFileOut;
	{
		peff::String path(allocator.get());

		if (!path.build(g_outputDirectoryPath.data()))
			return genOutOfMemoryCompError();

		if (!path.append("/"))
			return genOutOfMemoryCompError();

		if (!path.append(g_outputFileName))
			return genOutOfMemoryCompError();

		const size_t prefixLen = path.size();

		if (!path.append(".ts"))
			return genOutOfMemoryCompError();

		FILE *fp;

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

	for (size_t i = 0; i < enums.size(); ++i) {
		AstNodePtr<EnumNode> curEnum = mod->members.at(enums.at(i)).castTo<EnumNode>();

		if (i)
			INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write("\n"));

		INTERBUFC_RETURN_IF_COMP_ERROR(fillEnum(*this, curEnum));

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
				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(sourceFileOut, +0));

				INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write("export const "));
				INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write(curEnum->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write(" = {\n"));

				for (size_t j = 0; j < curEnum->members.size(); ++j) {
					auto m = curEnum->members.at(j);
					assert(m->astNodeType == AstNodeType::EnumItem);

					INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(sourceFileOut, +1));

					INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write(m->name));
					INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write(": "));
					INTERBUFC_RETURN_IF_COMP_ERROR(_writeExpr(sourceFileOut, m.castTo<EnumItemNode>()->value));
					INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write(" as "));
					INTERBUFC_RETURN_IF_COMP_ERROR(_writeInternalStorageTypeName(sourceFileOut, curEnum->baseType));

					if (j + 1 < curEnum->members.size())
						INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write(","));

					INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write("\n"));
				}

				INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(sourceFileOut, 0));
				INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write("} as const;\n"));
				break;
			}
			case TypeNameKind::String:
				return CompilationError(curEnum->baseType->tokenRange, CompilationErrorKind::InvalidEnumBaseType);
			case TypeNameKind::Custom:
				return CompilationError(curEnum->baseType->tokenRange, CompilationErrorKind::InvalidEnumBaseType);
			default:
				std::terminate();
		}

		INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write("type "));
		INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write(curEnum->name));
		INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write(" = typeof "));
		INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write(curEnum->name));
		INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write("[keyof typeof "));
		INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write(curEnum->name));
		INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write("];\n"));
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write("\n"));

	for (size_t i = 0; i < structs.size(); ++i) {
		AstNodePtr<StructNode> curStruct = mod->members.at(structs.at(i)).castTo<StructNode>();

		INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(sourceFileOut, +0));

		INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write("interface "));
		INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write(curStruct->name));
		INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write(" {\n"));

		for (size_t j = 0; j < curStruct->members.size(); ++j) {
			AstNodePtr<VarNode> curVar = curStruct->members.at(j).castTo<VarNode>();

			assert(curVar->astNodeType == AstNodeType::Var);

			INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(sourceFileOut, +1));

			INTERBUFC_RETURN_IF_COMP_ERROR(_writeInternalStorageName(sourceFileOut, curVar->name));
			INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write(": "));
			INTERBUFC_RETURN_IF_COMP_ERROR(_writeInternalStorageTypeName(sourceFileOut, curVar->type));
			INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write(";\n"));
		}

		INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(sourceFileOut, +0));

		INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write("}\n"));
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write("\n"));

	for (size_t i = 0; i < classes.size(); ++i) {
		AstNodePtr<ClassNode> curClass = mod->members.at(classes.at(i)).castTo<ClassNode>();

		INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(sourceFileOut, +0));

		INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write("interface "));
		INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write(curClass->name));
		INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write(" {\n"));

		for (size_t j = 0; j < curClass->members.size(); ++j) {
			AstNodePtr<VarNode> curVar = curClass->members.at(j).castTo<VarNode>();

			assert(curVar->astNodeType == AstNodeType::Var);

			INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(sourceFileOut, +1));

			INTERBUFC_RETURN_IF_COMP_ERROR(_writeInternalStorageName(sourceFileOut, curVar->name));
			INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write(": "));
			INTERBUFC_RETURN_IF_COMP_ERROR(_writeInternalStorageTypeName(sourceFileOut, curVar->type));
			INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write(";\n"));
		}

		INTERBUFC_RETURN_IF_COMP_ERROR(_writeIndent(sourceFileOut, +0));

		INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write("}\n"));
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(sourceFileOut.write("\n"));

	return {};
}
