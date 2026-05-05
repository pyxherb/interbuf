#include "compiler.h"
#include <interbufc/comp/util.h>
#include <cctype>

using namespace interbufc;

INTERBUFC_API CXXCompiler::CXXCompiler(peff::Alloc *allocator) : Compiler(allocator) {
}

INTERBUFC_API CXXCompiler::~CXXCompiler() {
}

static std::optional<interbufc::CompilationError> _write_indent(interbufc::File &file, size_t indent);
static std::optional<interbufc::CompilationError> _write_id_ref(interbufc::File &file, IdRef *id_ref);
static std::optional<interbufc::CompilationError> _write_internal_storage_name(interbufc::File &file, const std::string_view &name);
static std::optional<interbufc::CompilationError> _write_method_var_name(interbufc::File &file, const std::string_view &name);
static std::optional<interbufc::CompilationError> _write_internal_storage_type_name(interbufc::File &file, AstNodePtr<TypeNameNode> type_name);
static std::optional<interbufc::CompilationError> _write_type_layout_name(interbufc::File &file, const std::string_view &name);
static std::optional<interbufc::CompilationError> _write_document_var_name(interbufc::File &file);
static std::optional<interbufc::CompilationError> _write_internal_storage_type_name(interbufc::File &file, AstNodePtr<TypeNameNode> type_name);
static std::optional<interbufc::CompilationError> _write_storage_type_name(interbufc::File &file, AstNodePtr<TypeNameNode> type_name);
static std::optional<interbufc::CompilationError> _write_direct_type_name(interbufc::File &file, AstNodePtr<TypeNameNode> type_name);
static std::optional<interbufc::CompilationError> _write_interbuf_type_name_instance_init_value(interbufc::File &file, AstNodePtr<TypeNameNode> type_name);
static std::optional<interbufc::CompilationError> _write_expr(interbufc::File &file, AstNodePtr<ExprNode> expr);

static std::optional<interbufc::CompilationError> _write_indent(interbufc::File &file, size_t indent) {
	assert(indent < UINT32_MAX);
	for (size_t i = 0; i < indent; ++i)
		INTERBUFC_RETURN_IF_COMP_ERROR(file.write("\t"));
	return {};
};

static std::optional<interbufc::CompilationError> _write_id_ref(interbufc::File &file, IdRef *id_ref) {
	for (size_t i = 0; i < id_ref->entries.size(); ++i) {
		if (i)
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("::"));
		INTERBUFC_RETURN_IF_COMP_ERROR(file.write(id_ref->entries.at(i).name));
	}
	return {};
};

static std::optional<interbufc::CompilationError> _write_internal_storage_name(interbufc::File &file, const std::string_view &name) {
	INTERBUFC_RETURN_IF_COMP_ERROR(file.write("_generated_"));
	INTERBUFC_RETURN_IF_COMP_ERROR(file.write(name));
	return {};
}

static std::optional<interbufc::CompilationError> _write_method_var_name(interbufc::File &file, const std::string_view &name) {
	INTERBUFC_RETURN_IF_COMP_ERROR(file.write(name));
	return {};
}

static std::optional<interbufc::CompilationError> _write_type_layout_name(interbufc::File &file, const std::string_view &name) {
	INTERBUFC_RETURN_IF_COMP_ERROR(file.write("generated_"));
	INTERBUFC_RETURN_IF_COMP_ERROR(file.write(name));
	INTERBUFC_RETURN_IF_COMP_ERROR(file.write("_layout"));
	return {};
}

static std::optional<interbufc::CompilationError> _write_document_var_name(interbufc::File &file) {
	INTERBUFC_RETURN_IF_COMP_ERROR(file.write("generated_document"));
	return {};
}

static std::optional<interbufc::CompilationError> _write_internal_storage_type_name(interbufc::File &file, AstNodePtr<TypeNameNode> type_name) {
	switch (type_name->type_name_kind) {
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
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("std::string_view"));
			break;
		case TypeNameKind::Bool:
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write("bool"));
			break;
		case TypeNameKind::Custom: {
			AstNodePtr<MemberNode> m;

			INTERBUFC_RETURN_IF_COMP_ERROR(resolve_custom_type_name(type_name.cast_to<CustomTypeNameNode>(), m));

			if (!m)
				return CompilationError(type_name->token_range, CompilationErrorKind::InvalidTypeName);

			switch (m->ast_node_type) {
				case AstNodeType::Class:
					INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::ObjectPtr<interbuf::ClassBase>"));
					break;
				case AstNodeType::Struct:
					INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::ObjectPtr<interbuf::StructBase>"));
					break;
				case AstNodeType::Enum:
					INTERBUFC_RETURN_IF_COMP_ERROR(_write_id_ref(file, type_name.cast_to<CustomTypeNameNode>()->id_ref_ptr.get()));
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

static std::optional<interbufc::CompilationError> _write_storage_type_name(interbufc::File &file, AstNodePtr<TypeNameNode> type_name) {
	switch (type_name->type_name_kind) {
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

			INTERBUFC_RETURN_IF_COMP_ERROR(resolve_custom_type_name(type_name.cast_to<CustomTypeNameNode>(), m));

			if (!m)
				return CompilationError(type_name->token_range, CompilationErrorKind::InvalidTypeName);

			switch (m->ast_node_type) {
				case AstNodeType::Class:
				case AstNodeType::Struct:
					INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::ObjectPtr<"));
					INTERBUFC_RETURN_IF_COMP_ERROR(_write_id_ref(file, type_name.cast_to<CustomTypeNameNode>()->id_ref_ptr.get()));
					INTERBUFC_RETURN_IF_COMP_ERROR(file.write(">"));
					break;
				case AstNodeType::Enum:
					INTERBUFC_RETURN_IF_COMP_ERROR(_write_id_ref(file, type_name.cast_to<CustomTypeNameNode>()->id_ref_ptr.get()));
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

static std::optional<interbufc::CompilationError> _write_direct_type_name(interbufc::File &file, AstNodePtr<TypeNameNode> type_name) {
	switch (type_name->type_name_kind) {
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

			INTERBUFC_RETURN_IF_COMP_ERROR(resolve_custom_type_name(type_name.cast_to<CustomTypeNameNode>(), m));

			if (!m)
				return CompilationError(type_name->token_range, CompilationErrorKind::InvalidTypeName);

			switch (m->ast_node_type) {
				case AstNodeType::Class:
				case AstNodeType::Struct:
				case AstNodeType::Enum:
					INTERBUFC_RETURN_IF_COMP_ERROR(_write_id_ref(file, type_name.cast_to<CustomTypeNameNode>()->id_ref_ptr.get()));
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

static std::optional<interbufc::CompilationError> _write_interbuf_type_name_instance_init_value(interbufc::File &file, AstNodePtr<TypeNameNode> type_name) {
redump:
	switch (type_name->type_name_kind) {
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

			INTERBUFC_RETURN_IF_COMP_ERROR(resolve_custom_type_name(type_name.cast_to<CustomTypeNameNode>(), m));

			if (!m)
				return CompilationError(type_name->token_range, CompilationErrorKind::InvalidTypeName);

			switch (m->ast_node_type) {
				case AstNodeType::Class:
					INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::DataType{interbuf::FieldTypeKind::Class}"));
					break;
				case AstNodeType::Struct:
					INTERBUFC_RETURN_IF_COMP_ERROR(file.write("interbuf::DataType{interbuf::FieldTypeKind::Struct}"));
					break;
				case AstNodeType::Enum:
					type_name = m.cast_to<EnumNode>()->base_type;
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

static std::optional<interbufc::CompilationError> _write_expr(interbufc::File &file, AstNodePtr<ExprNode> expr) {
redump:
	switch (expr->expr_kind) {
		case ExprKind::I8: {
			AstNodePtr<I8LiteralExprNode> e = expr.cast_to<I8LiteralExprNode>();

			char s[8];
			sprintf(s, "%hd", (int16_t)e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::I16: {
			AstNodePtr<I16LiteralExprNode> e = expr.cast_to<I16LiteralExprNode>();

			char s[16];
			sprintf(s, "%hd", e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::I32: {
			AstNodePtr<I32LiteralExprNode> e = expr.cast_to<I32LiteralExprNode>();

			char s[32];
			sprintf(s, "%d", e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::I64: {
			AstNodePtr<I64LiteralExprNode> e = expr.cast_to<I64LiteralExprNode>();

			char s[48];
			sprintf(s, "%lld", e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::U8: {
			AstNodePtr<U8LiteralExprNode> e = expr.cast_to<U8LiteralExprNode>();

			char s[4];
			sprintf(s, "%u", (uint16_t)e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::U16: {
			AstNodePtr<U16LiteralExprNode> e = expr.cast_to<U16LiteralExprNode>();

			char s[8];
			sprintf(s, "%hu", e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::U32: {
			AstNodePtr<U32LiteralExprNode> e = expr.cast_to<U32LiteralExprNode>();

			char s[16];
			sprintf(s, "%u", e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::U64: {
			AstNodePtr<U64LiteralExprNode> e = expr.cast_to<U64LiteralExprNode>();

			char s[32];
			sprintf(s, "%llu", e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::F32: {
			AstNodePtr<F32LiteralExprNode> e = expr.cast_to<F32LiteralExprNode>();

			char s[16];
			sprintf(s, "%f", e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::F64: {
			AstNodePtr<F64LiteralExprNode> e = expr.cast_to<F64LiteralExprNode>();

			char s[32];
			sprintf(s, "%f", e->data);
			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(s));
			break;
		}
		case ExprKind::Bool: {
			AstNodePtr<BoolLiteralExprNode> e = expr.cast_to<BoolLiteralExprNode>();

			INTERBUFC_RETURN_IF_COMP_ERROR(file.write(e->data ? "true" : "false"));
			break;
		}
		default:
			std::terminate();
	}

	return {};
}

INTERBUFC_API std::optional<CompilationError> CXXCompiler::compile(
	AstNodePtr<ModuleNode> mod) {
	File header_file_out, source_file_out;
	{
		peff::String path(allocator.get());

		if (!path.build(g_output_directory_path.data()))
			return gen_out_of_memory_comp_error();

		if (!path.append("/"))
			return gen_out_of_memory_comp_error();

		if (!path.append(g_output_file_name))
			return gen_out_of_memory_comp_error();

		const size_t prefix_len = path.size();

		if (!path.append(".h"))
			return gen_out_of_memory_comp_error();

		FILE *fp;

		if (!(fp = fopen(path.data(), "wb"))) {
			ErrorOpeningFileError e(allocator.get());

			if (!e.name.build(path))
				return gen_out_of_memory_comp_error();

			return CompilationError({ 0, 0 }, std::move(e));
		}

		header_file_out.set_c_file(fp);

		if (!path.resize(prefix_len))
			return gen_out_of_memory_comp_error();

		if (!path.append(".cc"))
			return gen_out_of_memory_comp_error();

		if (!(fp = fopen(path.data(), "wb"))) {
			ErrorOpeningFileError e(allocator.get());

			if (!e.name.build(path))
				return gen_out_of_memory_comp_error();

			return CompilationError({ 0, 0 }, std::move(e));
		}

		source_file_out.set_c_file(fp);
	}

	peff::DynArray<size_t> structs(allocator.get());
	peff::DynArray<size_t> classes(allocator.get());
	peff::DynArray<size_t> enums(allocator.get());

	for (size_t i = 0; i < mod->members.size(); ++i) {
		auto m = mod->members.at(i);
		switch (m->ast_node_type) {
			case AstNodeType::Struct:
				if (!structs.push_back(+i))
					return gen_out_of_memory_comp_error();
				break;
			case AstNodeType::Class:
				if (!classes.push_back(+i))
					return gen_out_of_memory_comp_error();
				break;
			case AstNodeType::Enum:
				if (!enums.push_back(+i))
					return gen_out_of_memory_comp_error();
				break;
			default:
				std::terminate();
		}
	}

	size_t namespace_scopes = mod->namespace_path ? mod->namespace_path->entries.size() : 0;
	size_t indent = namespace_scopes;

	//
	// Write the header file.
	//
	for (size_t i = 0; i < namespace_scopes; ++i) {
		INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, i));
		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("namespace "));
		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(mod->namespace_path->entries.at(i).name));
		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("{\n"));
	}

	for (size_t i = 0; i < enums.size(); ++i) {
		AstNodePtr<EnumNode> cur_enum = mod->members.at(enums.at(i)).cast_to<EnumNode>();

		INTERBUFC_RETURN_IF_COMP_ERROR(fill_enum(*this, cur_enum));

		switch (cur_enum->base_type->type_name_kind) {
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
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));

				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("enum "));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(cur_enum->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(" : "));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_direct_type_name(header_file_out, cur_enum->base_type));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(" {\n"));

				for (size_t j = 0; j < cur_enum->members.size(); ++j) {
					auto m = cur_enum->members.at(j);
					assert(m->ast_node_type == AstNodeType::EnumItem);

					INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent + 1));

					INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(m->name));
					INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(" = "));
					INTERBUFC_RETURN_IF_COMP_ERROR(_write_expr(header_file_out, m.cast_to<EnumItemNode>()->value));

					if (j + 1 < cur_enum->members.size())
						INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(","));

					INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("\n"));
				}

				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("};\n"));
				break;
			}
			case TypeNameKind::String:
				return CompilationError(cur_enum->base_type->token_range, CompilationErrorKind::InvalidEnumBaseType);
			case TypeNameKind::Custom:
				return CompilationError(cur_enum->base_type->token_range, CompilationErrorKind::InvalidEnumBaseType);
			default:
				std::terminate();
		}
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("\n"));

	for (size_t i = 0; i < structs.size(); ++i) {
		AstNodePtr<StructNode> cur_struct = mod->members.at(structs.at(i)).cast_to<StructNode>();

		INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));

		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("struct "));
		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(cur_struct->name));
		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(";\n"));
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("\n"));

	for (size_t i = 0; i < classes.size(); ++i) {
		AstNodePtr<ClassNode> cur_class = mod->members.at(classes.at(i)).cast_to<ClassNode>();

		INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));

		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("class "));
		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(cur_class->name));
		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(";\n"));
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("\n"));

	for (size_t i = 0; i < structs.size(); ++i) {
		AstNodePtr<StructNode> cur_struct = mod->members.at(structs.at(i)).cast_to<StructNode>();

		INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));

		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("struct "));
		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(cur_struct->name));
		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(" {\n"));

		for (size_t j = 0; j < cur_struct->members.size(); ++j) {
			AstNodePtr<VarNode> cur_var = cur_struct->members.at(j).cast_to<VarNode>();

			assert(cur_var->ast_node_type == AstNodeType::Var);

			if (j)
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("\n"));

			INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent + 1));

			INTERBUFC_RETURN_IF_COMP_ERROR(_write_internal_storage_type_name(header_file_out, cur_var->type));
			INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(" "));
			INTERBUFC_RETURN_IF_COMP_ERROR(_write_internal_storage_name(header_file_out, cur_var->name));
			INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(";\n"));

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent + 1));

				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("INTERBUF_FORCEINLINE "));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_storage_type_name(header_file_out, cur_var->type));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(" get_"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_method_var_name(header_file_out, cur_var->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("();\n"));
			}

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent + 1));

				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("INTERBUF_FORCEINLINE void set_"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_method_var_name(header_file_out, cur_var->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("("));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_storage_type_name(header_file_out, cur_var->type));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(" data);\n"));
			}
		}

		INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));

		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("};\n"));
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("\n"));

	for (size_t i = 0; i < classes.size(); ++i) {
		AstNodePtr<ClassNode> cur_class = mod->members.at(classes.at(i)).cast_to<ClassNode>();

		INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));

		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("class "));
		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(cur_class->name));
		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(" {\n"));

		INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));
		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("public:\n"));

		for (size_t j = 0; j < cur_class->members.size(); ++j) {
			AstNodePtr<VarNode> cur_var = cur_class->members.at(j).cast_to<VarNode>();

			assert(cur_var->ast_node_type == AstNodeType::Var);

			if (j)
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("\n"));

			INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent + 1));

			INTERBUFC_RETURN_IF_COMP_ERROR(_write_internal_storage_type_name(header_file_out, cur_var->type));
			INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(" "));
			INTERBUFC_RETURN_IF_COMP_ERROR(_write_internal_storage_name(header_file_out, cur_var->name));
			INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(";\n"));

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent + 1));

				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("INTERBUF_FORCEINLINE "));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_storage_type_name(header_file_out, cur_var->type));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(" get_"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_method_var_name(header_file_out, cur_var->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("();\n"));
			}

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent + 1));

				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("INTERBUF_FORCEINLINE void set_"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_method_var_name(header_file_out, cur_var->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("("));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_storage_type_name(header_file_out, cur_var->type));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(" data);\n"));
			}
		}

		INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));

		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("};\n"));
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("\n"));

	for (size_t i = 0; i < structs.size(); ++i) {
		AstNodePtr<StructNode> cur_struct = mod->members.at(structs.at(i)).cast_to<StructNode>();

		for (size_t j = 0; j < cur_struct->members.size(); ++j) {
			AstNodePtr<VarNode> cur_var = cur_struct->members.at(j).cast_to<VarNode>();

			assert(cur_var->ast_node_type == AstNodeType::Var);

			if (j)
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("\n"));

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));

				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("INTERBUF_FORCEINLINE "));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_storage_type_name(header_file_out, cur_var->type));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(" "));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(cur_struct->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("::get_"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_method_var_name(header_file_out, cur_var->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("() {\n"));

				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent + 1));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("return "));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_internal_storage_name(header_file_out, cur_var->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(";\n"));

				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("}\n"));
			}

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));

				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("INTERBUF_FORCEINLINE void "));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(cur_struct->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("::set_"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_method_var_name(header_file_out, cur_var->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("("));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_storage_type_name(header_file_out, cur_var->type));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(" data) {\n"));

				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent + 1));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("this->"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_internal_storage_name(header_file_out, cur_var->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(" = data;\n"));

				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("}\n"));
			}
		}
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("\n"));

	for (size_t i = 0; i < classes.size(); ++i) {
		AstNodePtr<ClassNode> cur_class = mod->members.at(classes.at(i)).cast_to<ClassNode>();

		for (size_t j = 0; j < cur_class->members.size(); ++j) {
			AstNodePtr<VarNode> cur_var = cur_class->members.at(j).cast_to<VarNode>();

			assert(cur_var->ast_node_type == AstNodeType::Var);

			if (j)
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("\n"));

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));

				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("INTERBUF_FORCEINLINE "));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_storage_type_name(header_file_out, cur_var->type));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(" "));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(cur_class->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("::get_"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_method_var_name(header_file_out, cur_var->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("() {\n"));

				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent + 1));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("return "));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_internal_storage_name(header_file_out, cur_var->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(";\n"));

				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("}\n"));
			}

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));

				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("INTERBUF_FORCEINLINE void "));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(cur_class->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("::set_"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_method_var_name(header_file_out, cur_var->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("("));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_storage_type_name(header_file_out, cur_var->type));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(" data) {\n"));

				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent + 1));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("this->"));
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_internal_storage_name(header_file_out, cur_var->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(" = data;\n"));

				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));
				INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("}\n"));
			}
		}
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("\n"));

	INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));
	INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("struct InterbufResources {\n"));

	INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent + 1));

	INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("interbuf::ObjectPtr<interbuf::Document> "));
	INTERBUFC_RETURN_IF_COMP_ERROR(_write_document_var_name(header_file_out));
	INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(";\n"));

	for (size_t i = 0; i < structs.size(); ++i) {
		AstNodePtr<StructNode> cur_struct = mod->members.at(structs.at(i)).cast_to<StructNode>();

		INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent + 1));

		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("interbuf::ObjectPtr<interbuf::StructLayoutObject> "));
		INTERBUFC_RETURN_IF_COMP_ERROR(_write_type_layout_name(header_file_out, cur_struct->name));
		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(";\n"));
	}

	for (size_t i = 0; i < classes.size(); ++i) {
		AstNodePtr<ClassNode> cur_class = mod->members.at(structs.at(i)).cast_to<ClassNode>();

		INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent + 1));

		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("interbuf::ObjectPtr<interbuf::ClassLayoutObject> "));
		INTERBUFC_RETURN_IF_COMP_ERROR(_write_type_layout_name(header_file_out, cur_class->name));
		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write(";\n"));
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));
	INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("};\n"));

	INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("\n"));

	INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, indent));
	INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("interbuf::InternalExceptionPointer create_interbuf_resources(peff::Alloc *allocator, InterbufResources &resources_out);\n"));

	for (size_t i = 0; i < namespace_scopes; ++i) {
		INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(header_file_out, namespace_scopes - i - 1));
		INTERBUFC_RETURN_IF_COMP_ERROR(header_file_out.write("}\n"));
	}

	header_file_out.close();

	//
	// Write the source file.
	//
	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("#include \""));
	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(g_output_file_name));
	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(".h\"\n"));

	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("\n"));

	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("interbuf::InternalExceptionPointer "));
	INTERBUFC_RETURN_IF_COMP_ERROR(_write_id_ref(source_file_out, mod->namespace_path.get()));
	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("::create_interbuf_resources(peff::Alloc *allocator, InterbufResources &resources_out) {\n"));

	const std::string_view document_tmp_var_name = "document",
						   type_object_tmp_var_name = "type_object",
						   struct_layout_tmp_var_name = "struct_layout",
						   class_layout_tmp_var_name = "class_layout";

	INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +1));

	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("interbuf::ObjectPtr<interbuf::Document> "));
	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(document_tmp_var_name));
	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(";\n"));

	INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +1));

	{
		INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("if (!("));
		INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(document_tmp_var_name));
		INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(" = interbuf::make_object<interbuf::Document>(allocator, allocator)))\n"));

		{
			INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +2));
			INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("return interbuf::OutOfMemoryError::alloc();\n"));
		}
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("\n"));

	INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +1));

	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("interbuf::DataType "));
	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(type_object_tmp_var_name));
	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(";\n"));

	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("\n"));

	INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +1));

	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("interbuf::ObjectPtr<interbuf::StructLayout> "));
	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(struct_layout_tmp_var_name));
	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(";\n"));

	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("\n"));

	for (size_t i = 0; i < structs.size(); ++i) {
		AstNodePtr<StructNode> cur_struct = mod->members.at(structs.at(i)).cast_to<StructNode>();

		INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +1));

		{
			INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("if (!("));
			INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(struct_layout_tmp_var_name));
			INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(" = interbuf::make_object<interbuf::StructLayout>(allocator, "));
			INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(document_tmp_var_name));
			INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(", allocator)))\n"));

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +2));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("return interbuf::OutOfMemoryError::alloc();\n"));
			}
		}

		INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +1));

		INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("resources_out."));
		INTERBUFC_RETURN_IF_COMP_ERROR(_write_type_layout_name(source_file_out, cur_struct->name));
		INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(" = "));
		INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(struct_layout_tmp_var_name));
		INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(";\n"));
	}

	for (size_t i = 0; i < classes.size(); ++i) {
		AstNodePtr<ClassNode> cur_class = mod->members.at(structs.at(i)).cast_to<ClassNode>();

		INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +1));

		{
			INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("if (!("));
			INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(struct_layout_tmp_var_name));
			INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(" = interbuf::make_object<interbuf::ClassLayout>(allocator, "));
			INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(document_tmp_var_name));
			INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(", allocator)))\n"));

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +2));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("return interbuf::OutOfMemoryError::alloc();\n"));
			}
		}

		INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +1));

		INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("resources_out."));
		INTERBUFC_RETURN_IF_COMP_ERROR(_write_type_layout_name(source_file_out, cur_class->name));
		INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(" = "));
		INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(struct_layout_tmp_var_name));
		INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(";\n"));
	}

	for (size_t i = 0; i < structs.size(); ++i) {
		AstNodePtr<StructNode> cur_struct = mod->members.at(structs.at(i)).cast_to<StructNode>();

		for (size_t j = 0; j < cur_struct->members.size(); ++j) {
			AstNodePtr<VarNode> cur_var = cur_struct->members.at(j).cast_to<VarNode>();

			assert(cur_var->ast_node_type == AstNodeType::Var);

			if (j)
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("\n"));

			INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +1));
			INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(type_object_tmp_var_name));
			INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(" = "));
			INTERBUFC_RETURN_IF_COMP_ERROR(_write_interbuf_type_name_instance_init_value(source_file_out, cur_var->type));
			INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(";\n"));

			switch (cur_var->type->type_name_kind) {
				case TypeNameKind::Custom: {
					AstNodePtr<MemberNode> m;

					INTERBUFC_RETURN_IF_COMP_ERROR(resolve_custom_type_name(cur_var->type.cast_to<CustomTypeNameNode>(), m));

					if (!m)
						return CompilationError(cur_var->type->token_range, CompilationErrorKind::InvalidTypeName);

					switch (m->ast_node_type) {
						case AstNodeType::Class:
							INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +1));
							INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(type_object_tmp_var_name));
							INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(".type_def_object = "));
							INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("resources_out."));
							INTERBUFC_RETURN_IF_COMP_ERROR(_write_type_layout_name(source_file_out, m->name));
							INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(".cast_to<Object>()\n"));
							break;
						case AstNodeType::Struct:
							INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +1));
							INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(type_object_tmp_var_name));
							INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(".type_def_object = "));
							INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("resources_out."));
							INTERBUFC_RETURN_IF_COMP_ERROR(_write_type_layout_name(source_file_out, m->name));
							INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(".cast_to<Object>()\n"));
							break;
						case AstNodeType::Enum:
							break;
						default:
							std::terminate();
					}
				}
				default:;
			}

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +1));

				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("if (!("));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(struct_layout_tmp_var_name));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("->add_field("));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("interbuf::StructField {"));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(type_object_tmp_var_name));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(", interbuf_offsetof("));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(cur_struct->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(", "));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(cur_var->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(")"));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("})"));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("))\n"));

				{
					INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +2));
					INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("return interbuf::OutOfMemoryError::alloc();\n"));
				}
			}
		}
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +1));

	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("interbuf::ObjectPtr<interbuf::ClassLayout> "));
	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(class_layout_tmp_var_name));
	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(";\n"));

	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("\n"));

	for (size_t i = 0; i < classes.size(); ++i) {
		AstNodePtr<ClassNode> cur_class = mod->members.at(classes.at(i)).cast_to<ClassNode>();

		for (size_t j = 0; j < cur_class->members.size(); ++j) {
			AstNodePtr<VarNode> cur_var = cur_class->members.at(j).cast_to<VarNode>();

			assert(cur_var->ast_node_type == AstNodeType::Var);

			if (j)
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("\n"));

			INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +1));
			INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(type_object_tmp_var_name));
			INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(" = "));
			INTERBUFC_RETURN_IF_COMP_ERROR(_write_interbuf_type_name_instance_init_value(source_file_out, cur_var->type));
			INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(";\n"));

			switch (cur_var->type->type_name_kind) {
				case TypeNameKind::Custom: {
					AstNodePtr<MemberNode> m;

					INTERBUFC_RETURN_IF_COMP_ERROR(resolve_custom_type_name(cur_var->type.cast_to<CustomTypeNameNode>(), m));

					if (!m)
						return CompilationError(cur_var->type->token_range, CompilationErrorKind::InvalidTypeName);

					switch (m->ast_node_type) {
						case AstNodeType::Class:
							INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +1));
							INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(type_object_tmp_var_name));
							INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(".type_def_object = "));
							INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("resources_out."));
							INTERBUFC_RETURN_IF_COMP_ERROR(_write_type_layout_name(source_file_out, m->name));
							INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(".cast_to<Object>()\n"));
							break;
						case AstNodeType::Struct:
							INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +1));
							INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(type_object_tmp_var_name));
							INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(".type_def_object = "));
							INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("resources_out."));
							INTERBUFC_RETURN_IF_COMP_ERROR(_write_type_layout_name(source_file_out, m->name));
							INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(".cast_to<Object>()\n"));
							break;
						case AstNodeType::Enum:
							break;
						default:
							std::terminate();
					}
				}
				default:;
			}

			{
				INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +1));

				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("if (!("));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(struct_layout_tmp_var_name));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("->add_field("));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("interbuf::ClassField {\""));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(cur_var->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("\", "));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(type_object_tmp_var_name));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(", interbuf_offsetof("));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(cur_class->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(", "));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(cur_var->name));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write(")"));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("})"));
				INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("))\n"));

				{
					INTERBUFC_RETURN_IF_COMP_ERROR(_write_indent(source_file_out, +2));
					INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("return interbuf::OutOfMemoryError::alloc();\n"));
				}
			}
		}
	}

	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("\n"));

	// TODO: Implement it.

	INTERBUFC_RETURN_IF_COMP_ERROR(source_file_out.write("}\n"));

	return {};
}
