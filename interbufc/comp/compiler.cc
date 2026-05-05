#include "compiler.h"

using namespace interbufc;

std::string_view interbufc::g_language, interbufc::g_source_file_name, interbufc::g_output_directory_path, interbufc::g_output_file_name;

INTERBUFC_API Compiler::Compiler(peff::Alloc *allocator) : warnings(allocator), errors(allocator), allocator(allocator) {
}

INTERBUFC_API Compiler::~Compiler() {
}

std::optional<interbufc::CompilationError> interbufc::fill_enum(Compiler& compiler, AstNodePtr<EnumNode> enum_node) {
	for (size_t i = 0; i < enum_node->members.size(); ++i) {
		assert(enum_node->members.at(i)->ast_node_type == AstNodeType::EnumItem);

		auto m = enum_node->members.at(i).cast_to<EnumItemNode>();

		if (!m->value) {
			switch (enum_node->base_type->type_name_kind) {
				case TypeNameKind::I8:
				case TypeNameKind::I16:
				case TypeNameKind::I32:
				case TypeNameKind::I64:
				case TypeNameKind::U8:
				case TypeNameKind::U16:
				case TypeNameKind::U32:
				case TypeNameKind::U64:
					break;
				case TypeNameKind::F32:
					return CompilationError(m->token_range, CompilationErrorKind::EnumTypeNotIncrementable);
				case TypeNameKind::F64:
					return CompilationError(m->token_range, CompilationErrorKind::EnumTypeNotIncrementable);
				case TypeNameKind::Bool:
					return CompilationError(m->token_range, CompilationErrorKind::EnumTypeNotIncrementable);
				default:
					return CompilationError(m->token_range, CompilationErrorKind::InvalidEnumBaseType);
			}
			if (i)
				INTERBUFC_RETURN_IF_COMP_ERROR(inc_enum_value(compiler, enum_node->self_allocator.get(), enum_node->members.at(i - 1).cast_to<EnumItemNode>()->value, m->value));
			else
				INTERBUFC_RETURN_IF_COMP_ERROR(default_enum_value(compiler, enum_node->self_allocator.get(), enum_node->base_type, m->value));
		}
	}

	return {};
}

std::optional<interbufc::CompilationError> interbufc::default_enum_value(Compiler& compiler, peff::Alloc* allocator, AstNodePtr<TypeNameNode> type_name, AstNodePtr<ExprNode>& expr_out) {
	switch (type_name->type_name_kind) {
		case TypeNameKind::I8:
			if (!(expr_out = make_ast_node<I8LiteralExprNode>(allocator, allocator, type_name->document->shared_from_this(), 0).cast_to<ExprNode>()))
				return gen_out_of_memory_comp_error();
			break;
		case TypeNameKind::I16:
			if (!(expr_out = make_ast_node<I16LiteralExprNode>(allocator, allocator, type_name->document->shared_from_this(), 0).cast_to<ExprNode>()))
				return gen_out_of_memory_comp_error();
			break;
		case TypeNameKind::I32:
			if (!(expr_out = make_ast_node<I32LiteralExprNode>(allocator, allocator, type_name->document->shared_from_this(), 0).cast_to<ExprNode>()))
				return gen_out_of_memory_comp_error();
			break;
		case TypeNameKind::I64:
			if (!(expr_out = make_ast_node<I64LiteralExprNode>(allocator, allocator, type_name->document->shared_from_this(), 0).cast_to<ExprNode>()))
				return gen_out_of_memory_comp_error();
			break;
		case TypeNameKind::U8:
			if (!(expr_out = make_ast_node<U8LiteralExprNode>(allocator, allocator, type_name->document->shared_from_this(), 0).cast_to<ExprNode>()))
				return gen_out_of_memory_comp_error();
			break;
		case TypeNameKind::U16:
			if (!(expr_out = make_ast_node<U16LiteralExprNode>(allocator, allocator, type_name->document->shared_from_this(), 0).cast_to<ExprNode>()))
				return gen_out_of_memory_comp_error();
			break;
		case TypeNameKind::U32:
			if (!(expr_out = make_ast_node<U32LiteralExprNode>(allocator, allocator, type_name->document->shared_from_this(), 0).cast_to<ExprNode>()))
				return gen_out_of_memory_comp_error();
			break;
		case TypeNameKind::U64:
			if (!(expr_out = make_ast_node<U64LiteralExprNode>(allocator, allocator, type_name->document->shared_from_this(), 0).cast_to<ExprNode>()))
				return gen_out_of_memory_comp_error();
			break;
		case TypeNameKind::F32:
			if (!(expr_out = make_ast_node<F32LiteralExprNode>(allocator, allocator, type_name->document->shared_from_this(), 0).cast_to<ExprNode>()))
				return gen_out_of_memory_comp_error();
			break;
		case TypeNameKind::F64:
			if (!(expr_out = make_ast_node<F64LiteralExprNode>(allocator, allocator, type_name->document->shared_from_this(), 0).cast_to<ExprNode>()))
				return gen_out_of_memory_comp_error();
			break;
		default:
			std::terminate();
	}

	return {};
}

std::optional<interbufc::CompilationError> interbufc::inc_enum_value(Compiler &compiler, peff::Alloc *allocator, AstNodePtr<ExprNode> expr, AstNodePtr<ExprNode> &expr_out) {
	switch (expr->expr_kind) {
		case ExprKind::I8: {
			AstNodePtr<I8LiteralExprNode> e = expr.cast_to<I8LiteralExprNode>();

			if (!(expr_out = make_ast_node<I8LiteralExprNode>(allocator, allocator, expr->document->shared_from_this(), e->data + 1).cast_to<ExprNode>()))
				return gen_out_of_memory_comp_error();
			break;
		}
		case ExprKind::I16: {
			AstNodePtr<I16LiteralExprNode> e = expr.cast_to<I16LiteralExprNode>();

			if (!(expr_out = make_ast_node<I16LiteralExprNode>(allocator, allocator, expr->document->shared_from_this(), e->data + 1).cast_to<ExprNode>()))
				return gen_out_of_memory_comp_error();
			break;
		}
		case ExprKind::I32: {
			AstNodePtr<I32LiteralExprNode> e = expr.cast_to<I32LiteralExprNode>();

			if (!(expr_out = make_ast_node<I32LiteralExprNode>(allocator, allocator, expr->document->shared_from_this(), e->data + 1).cast_to<ExprNode>()))
				return gen_out_of_memory_comp_error();
			break;
		}
		case ExprKind::I64: {
			AstNodePtr<I64LiteralExprNode> e = expr.cast_to<I64LiteralExprNode>();

			if (!(expr_out = make_ast_node<I64LiteralExprNode>(allocator, allocator, expr->document->shared_from_this(), e->data + 1).cast_to<ExprNode>()))
				return gen_out_of_memory_comp_error();
			break;
		}
		case ExprKind::U8: {
			AstNodePtr<U8LiteralExprNode> e = expr.cast_to<U8LiteralExprNode>();

			if (!(expr_out = make_ast_node<U8LiteralExprNode>(allocator, allocator, expr->document->shared_from_this(), e->data + 1).cast_to<ExprNode>()))
				return gen_out_of_memory_comp_error();
			break;
		}
		case ExprKind::U16: {
			AstNodePtr<U16LiteralExprNode> e = expr.cast_to<U16LiteralExprNode>();

			if (!(expr_out = make_ast_node<U16LiteralExprNode>(allocator, allocator, expr->document->shared_from_this(), e->data + 1).cast_to<ExprNode>()))
				return gen_out_of_memory_comp_error();
			break;
		}
		case ExprKind::U32: {
			AstNodePtr<U32LiteralExprNode> e = expr.cast_to<U32LiteralExprNode>();

			if (!(expr_out = make_ast_node<U32LiteralExprNode>(allocator, allocator, expr->document->shared_from_this(), e->data + 1).cast_to<ExprNode>()))
				return gen_out_of_memory_comp_error();
			break;
		}
		case ExprKind::U64: {
			AstNodePtr<U64LiteralExprNode> e = expr.cast_to<U64LiteralExprNode>();

			if (!(expr_out = make_ast_node<U64LiteralExprNode>(allocator, allocator, expr->document->shared_from_this(), e->data + 1).cast_to<ExprNode>()))
				return gen_out_of_memory_comp_error();
			break;
		}
		case ExprKind::F32:
			std::terminate();
		case ExprKind::F64:
			std::terminate();
		default:
			std::terminate();
	}

	return {};
}

std::optional<CompilationError> interbufc::resolve_custom_type_name(AstNodePtr<CustomTypeNameNode> type_name, AstNodePtr<MemberNode> &member_out) {
	member_out = {};

	AstNodePtr<MemberNode> m = type_name->context_node.lock();

	if (type_name->cached_resolve_result.is_valid()) {
		member_out = type_name->cached_resolve_result.lock();
		return {};
	}

	for (size_t i = 0; i < type_name->id_ref_ptr->entries.size(); ++i) {
		IdRefEntry &entry = type_name->id_ref_ptr->entries.at(i);

		switch (m->ast_node_type) {
			case AstNodeType::Module: {
				AstNodePtr<ModuleNode> member = m.cast_to<ModuleNode>();

				if (auto it = member->member_indices.find(entry.name); it != member->member_indices.end()) {
					m = member->members.at(it.value());
				} else
					return {};
				break;
			}
			case AstNodeType::Class: {
				AstNodePtr<ClassNode> member = m.cast_to<ClassNode>();

				if (auto it = member->member_indices.find(entry.name); it != member->member_indices.end()) {
					m = member->members.at(it.value());
				} else
					return {};
				break;
			}
			case AstNodeType::Struct: {
				AstNodePtr<StructNode> member = m.cast_to<StructNode>();

				if (auto it = member->member_indices.find(entry.name); it != member->member_indices.end()) {
					m = member->members.at(it.value());
				} else
					return {};
				break;
			}
			default:
				return {};
		}
	}

	member_out = m;
	type_name->cached_resolve_result = m;

	return {};
}
