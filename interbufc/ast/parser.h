#ifndef _INTERBUFC_AST_PARSER_H_
#define _INTERBUFC_AST_PARSER_H_

#include "lexer.h"
#include "typename.h"
#include "idref.h"
#include "import.h"
#include "document.h"
#include "class.h"
#include "var.h"

namespace interbufc {
	enum class SyntaxErrorKind : int {
		OutOfMemory = 0,
		UnexpectedToken,
		ExpectingSingleToken,
		ExpectingTokens,
		ExpectingId,
		ExpectingOperatorName,
		ExpectingExpr,
		ExpectingStmt,
		ExpectingDecl,
		InvalidMetaTypeName,
		NoMatchingTokensFound,
		ConflictingDefinitions
	};

	struct ExpectingSingleTokenErrorExData {
		TokenId expecting_token_id;
	};

	struct ExpectingTokensErrorExData {
		peff::Set<TokenId> expecting_token_ids;

		INTERBUFC_FORCEINLINE ExpectingTokensErrorExData(peff::Alloc *allocator) : expecting_token_ids(allocator) {
		}
	};

	struct NoMatchingTokensFoundErrorExData {
		peff::Set<TokenId> expecting_token_ids;

		INTERBUFC_FORCEINLINE NoMatchingTokensFoundErrorExData(peff::Alloc *allocator) : expecting_token_ids(allocator) {
		}
	};

	struct ConflictingDefinitionsErrorExData {
		peff::String member_name;

		INTERBUFC_FORCEINLINE ConflictingDefinitionsErrorExData(peff::String &&name) : member_name(std::move(name)) {
		}
	};

	struct SyntaxError {
		TokenRange token_range;
		SyntaxErrorKind error_kind;
		std::variant<std::monostate, ExpectingTokensErrorExData, NoMatchingTokensFoundErrorExData, ExpectingSingleTokenErrorExData, ConflictingDefinitionsErrorExData> ex_data;

		INTERBUFC_FORCEINLINE SyntaxError(
			const TokenRange &token_range,
			SyntaxErrorKind error_kind)
			: token_range(token_range),
			  error_kind(error_kind) {
		}

		INTERBUFC_FORCEINLINE SyntaxError(
			const TokenRange &token_range,
			ExpectingTokensErrorExData &&ex_data)
			: token_range(token_range),
			  error_kind(SyntaxErrorKind::ExpectingTokens),
			  ex_data(std::move(ex_data)) {
		}

		INTERBUFC_FORCEINLINE SyntaxError(
			const TokenRange &token_range,
			ExpectingSingleTokenErrorExData &&ex_data)
			: token_range(token_range),
			  error_kind(SyntaxErrorKind::ExpectingSingleToken),
			  ex_data(std::move(ex_data)) {
		}

		INTERBUFC_FORCEINLINE SyntaxError(
			const TokenRange &token_range,
			NoMatchingTokensFoundErrorExData &&ex_data)
			: token_range(token_range),
			  error_kind(SyntaxErrorKind::NoMatchingTokensFound),
			  ex_data(std::move(ex_data)) {
		}

		INTERBUFC_FORCEINLINE SyntaxError(
			const TokenRange &token_range,
			ConflictingDefinitionsErrorExData &&ex_data)
			: token_range(token_range),
			  error_kind(SyntaxErrorKind::ConflictingDefinitions),
			  ex_data(std::move(ex_data)) {
		}

		INTERBUFC_FORCEINLINE ExpectingTokensErrorExData &get_expecting_tokens_error_ex_data() {
			return std::get<ExpectingTokensErrorExData>(ex_data);
		}

		INTERBUFC_FORCEINLINE const ExpectingTokensErrorExData &get_expecting_tokens_error_ex_data() const {
			return std::get<ExpectingTokensErrorExData>(ex_data);
		}

		INTERBUFC_FORCEINLINE const NoMatchingTokensFoundErrorExData &get_no_matching_tokens_found_error_ex_data() const {
			return std::get<NoMatchingTokensFoundErrorExData>(ex_data);
		}
	};

	class Parser : public peff::SharedFromThis<Parser> {
	public:
		AstNodePtr<Document> document;
		AstNodePtr<MemberNode> cur_parent;
		peff::RcObjectPtr<peff::Alloc> resource_allocator;
		TokenList token_list;
		struct ParseContext {
			size_t idx_prev_token = 0, idx_current_token = 0;
		};
		ParseContext parse_context;
		peff::DynArray<SyntaxError> syntax_errors;

		INTERBUFC_API Parser(AstNodePtr<Document> document, TokenList &&token_list, peff::Alloc *resource_allocator);
		INTERBUFC_API ~Parser();

		INTERBUFC_API SyntaxError gen_out_of_memory_error() {
			return SyntaxError(TokenRange{ 0 }, SyntaxErrorKind::OutOfMemory);
		}

		INTERBUFC_API std::optional<SyntaxError> lookahead_until(size_t n_token_ids, const TokenId token_ids[]);
		INTERBUFC_API Token *next_token(bool keep_new_line = false, bool keep_whitespace = false, bool keep_comment = false);
		INTERBUFC_API Token *peek_token(bool keep_new_line = false, bool keep_whitespace = false, bool keep_comment = false);

		[[nodiscard]] INTERBUFC_FORCEINLINE std::optional<SyntaxError> expect_token(Token *token, TokenId token_id) {
			if (token->token_id != token_id) {
				ExpectingSingleTokenErrorExData ex_data = { token_id };

				return SyntaxError(TokenRange{ token->index }, std::move(ex_data));
			}

			return {};
		}

		[[nodiscard]] INTERBUFC_FORCEINLINE std::optional<SyntaxError> expect_token(Token *token) {
			if (token->token_id == TokenId::End) {
				ExpectingTokensErrorExData ex_data(resource_allocator.get());

				return SyntaxError(TokenRange{ token->index }, std::move(ex_data));
			}

			return {};
		}

		[[nodiscard]] INTERBUFC_API std::optional<SyntaxError> parse_id_name(peff::String &name_out);
		[[nodiscard]] INTERBUFC_API std::optional<SyntaxError> parse_id_ref(IdRefPtr &id_ref_out);

		[[nodiscard]] INTERBUFC_API std::optional<SyntaxError> parse_expr(int precedence, AstNodePtr<ExprNode> &expr_out);

		[[nodiscard]] INTERBUFC_API std::optional<SyntaxError> parse_type_name(AstNodePtr<TypeNameNode> &type_name_out, bool with_circumfixes);

		[[nodiscard]] INTERBUFC_API std::optional<SyntaxError> parse_var_field(AstNodePtr<VarNode> &var_node_out);
		[[nodiscard]] INTERBUFC_API std::optional<SyntaxError> parse_program_stmt();

		/// @brief Parse a whole program.
		/// @return The syntax error that forced the parser to interrupt the parse progress.
		/// @note Don't forget that there still may be syntax errors emitted even the parse progress is not interrupted.
		[[nodiscard]] INTERBUFC_API std::optional<SyntaxError> parse_program(const AstNodePtr<ModuleNode> &initial_mod);
	};
}

#endif
