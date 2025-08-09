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
		TokenId expectingTokenId;
	};

	struct ExpectingTokensErrorExData {
		peff::Set<TokenId> expectingTokenIds;

		INTERBUFC_FORCEINLINE ExpectingTokensErrorExData(peff::Alloc *allocator) : expectingTokenIds(allocator) {
		}
	};

	struct NoMatchingTokensFoundErrorExData {
		peff::Set<TokenId> expectingTokenIds;

		INTERBUFC_FORCEINLINE NoMatchingTokensFoundErrorExData(peff::Alloc *allocator) : expectingTokenIds(allocator) {
		}
	};

	struct ConflictingDefinitionsErrorExData {
		peff::String memberName;

		INTERBUFC_FORCEINLINE ConflictingDefinitionsErrorExData(peff::String &&name) : memberName(std::move(name)) {
		}
	};

	struct SyntaxError {
		TokenRange tokenRange;
		SyntaxErrorKind errorKind;
		std::variant<std::monostate, ExpectingTokensErrorExData, NoMatchingTokensFoundErrorExData, ExpectingSingleTokenErrorExData, ConflictingDefinitionsErrorExData> exData;

		INTERBUFC_FORCEINLINE SyntaxError(
			const TokenRange &tokenRange,
			SyntaxErrorKind errorKind)
			: tokenRange(tokenRange),
			  errorKind(errorKind) {
		}

		INTERBUFC_FORCEINLINE SyntaxError(
			const TokenRange &tokenRange,
			ExpectingTokensErrorExData &&exData)
			: tokenRange(tokenRange),
			  errorKind(SyntaxErrorKind::ExpectingTokens),
			  exData(std::move(exData)) {
		}

		INTERBUFC_FORCEINLINE SyntaxError(
			const TokenRange &tokenRange,
			ExpectingSingleTokenErrorExData &&exData)
			: tokenRange(tokenRange),
			  errorKind(SyntaxErrorKind::ExpectingSingleToken),
			  exData(std::move(exData)) {
		}

		INTERBUFC_FORCEINLINE SyntaxError(
			const TokenRange &tokenRange,
			NoMatchingTokensFoundErrorExData &&exData)
			: tokenRange(tokenRange),
			  errorKind(SyntaxErrorKind::NoMatchingTokensFound),
			  exData(std::move(exData)) {
		}

		INTERBUFC_FORCEINLINE SyntaxError(
			const TokenRange &tokenRange,
			ConflictingDefinitionsErrorExData &&exData)
			: tokenRange(tokenRange),
			  errorKind(SyntaxErrorKind::ConflictingDefinitions),
			  exData(std::move(exData)) {
		}

		INTERBUFC_FORCEINLINE ExpectingTokensErrorExData &getExpectingTokensErrorExData() {
			return std::get<ExpectingTokensErrorExData>(exData);
		}

		INTERBUFC_FORCEINLINE const ExpectingTokensErrorExData &getExpectingTokensErrorExData() const {
			return std::get<ExpectingTokensErrorExData>(exData);
		}

		INTERBUFC_FORCEINLINE const NoMatchingTokensFoundErrorExData &getNoMatchingTokensFoundErrorExData() const {
			return std::get<NoMatchingTokensFoundErrorExData>(exData);
		}
	};

	class Parser : public peff::SharedFromThis<Parser> {
	public:
		AstNodePtr<Document> document;
		AstNodePtr<MemberNode> curParent;
		peff::RcObjectPtr<peff::Alloc> resourceAllocator;
		TokenList tokenList;
		struct ParseContext {
			size_t idxPrevToken = 0, idxCurrentToken = 0;
		};
		ParseContext parseContext;
		peff::DynArray<SyntaxError> syntaxErrors;

		INTERBUFC_API Parser(AstNodePtr<Document> document, TokenList &&tokenList, peff::Alloc *resourceAllocator);
		INTERBUFC_API ~Parser();

		INTERBUFC_API SyntaxError genOutOfMemoryError() {
			return SyntaxError(TokenRange{ 0 }, SyntaxErrorKind::OutOfMemory);
		}

		INTERBUFC_API std::optional<SyntaxError> lookaheadUntil(size_t nTokenIds, const TokenId tokenIds[]);
		INTERBUFC_API Token *nextToken(bool keepNewLine = false, bool keepWhitespace = false, bool keepComment = false);
		INTERBUFC_API Token *peekToken(bool keepNewLine = false, bool keepWhitespace = false, bool keepComment = false);

		[[nodiscard]] INTERBUFC_FORCEINLINE std::optional<SyntaxError> expectToken(Token *token, TokenId tokenId) {
			if (token->tokenId != tokenId) {
				ExpectingSingleTokenErrorExData exData = { tokenId };

				return SyntaxError(TokenRange{ token->index }, std::move(exData));
			}

			return {};
		}

		[[nodiscard]] INTERBUFC_FORCEINLINE std::optional<SyntaxError> expectToken(Token *token) {
			if (token->tokenId == TokenId::End) {
				ExpectingTokensErrorExData exData(resourceAllocator.get());

				return SyntaxError(TokenRange{ token->index }, std::move(exData));
			}

			return {};
		}

		[[nodiscard]] INTERBUFC_API std::optional<SyntaxError> parseIdName(peff::String &nameOut);
		[[nodiscard]] INTERBUFC_API std::optional<SyntaxError> parseIdRef(IdRefPtr &idRefOut);

		[[nodiscard]] INTERBUFC_API std::optional<SyntaxError> parseTypeName(AstNodePtr<TypeNameNode> &typeNameOut, bool withCircumfixes);

		[[nodiscard]] INTERBUFC_API std::optional<SyntaxError> parseVarField(AstNodePtr<VarNode> &varNodeOut);
		[[nodiscard]] INTERBUFC_API std::optional<SyntaxError> parseProgramStmt();

		/// @brief Parse a whole program.
		/// @return The syntax error that forced the parser to interrupt the parse progress.
		/// @note Don't forget that there still may be syntax errors emitted even the parse progress is not interrupted.
		[[nodiscard]] INTERBUFC_API std::optional<SyntaxError> parseProgram(const AstNodePtr<ModuleNode> &initialMod, IdRefPtr &moduleNameOut);
	};
}

#endif
