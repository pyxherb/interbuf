#ifndef _INTERBUFC_COMP_COMPILER_H_
#define _INTERBUFC_COMP_COMPILER_H_

#include <interbufc/ast/parser.h>
#include <interbufc/ast/typename.h>

namespace interbufc {
	extern std::string_view g_language, g_source_file_name, g_output_directory_path, g_output_file_name;

	class Compiler {
	public:
		peff::RcObjectPtr<peff::Alloc> allocator;
		peff::DynArray<CompilationError> errors;
		peff::DynArray<CompilationWarning> warnings;

		INTERBUFC_API Compiler(peff::Alloc *allocator);
		INTERBUFC_API virtual ~Compiler();

		[[nodiscard]] INTERBUFC_FORCEINLINE std::optional<CompilationError> push_error(CompilationError &&error) noexcept {
			if (!errors.push_back(std::move(error)))
				return gen_out_of_memory_comp_error();

			return {};
		}

		[[nodiscard]] INTERBUFC_FORCEINLINE std::optional<CompilationError> push_warning(CompilationWarning &&warning) noexcept {
			if (!warnings.push_back(std::move(warning)))
				return gen_out_of_memory_comp_error();

			return {};
		}

		[[nodiscard]] virtual std::optional<CompilationError> compile(
			AstNodePtr<ModuleNode> mod) = 0;
	};

	std::optional<interbufc::CompilationError> fill_enum(Compiler &compiler, AstNodePtr<EnumNode> enum_node);
	std::optional<interbufc::CompilationError> default_enum_value(Compiler &compiler, peff::Alloc *allocator, AstNodePtr<TypeNameNode> expr, AstNodePtr<ExprNode> &expr_out);
	std::optional<interbufc::CompilationError> inc_enum_value(Compiler &compiler, peff::Alloc *allocator, AstNodePtr<ExprNode> expr, AstNodePtr<ExprNode> &expr_out);
	std::optional<CompilationError> resolve_custom_type_name(AstNodePtr<CustomTypeNameNode> type_name, AstNodePtr<MemberNode> &member_out);
}

#define INTERBUFC_RETURN_EXCEPT_IF_WRITE_FAILED(allocator, e)         \
	if (!(e))                                                        \
		return interbufc::gen_io_comp_error(); \
	else                                                             \
		;

#endif
