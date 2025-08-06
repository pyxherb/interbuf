#ifndef _INTERBUFC_AST_LEXER_H_
#define _INTERBUFC_AST_LEXER_H_

#include "../basedefs.h"
#include <peff/base/deallocable.h>
#include <peff/containers/dynarray.h>
#include <peff/containers/string.h>
#include <peff/advutils/shared_ptr.h>
#include <optional>
#include <variant>

namespace interbufc {
	struct SourcePosition {
		size_t line, column;

		INTERBUFC_FORCEINLINE SourcePosition() : line(SIZE_MAX), column(SIZE_MAX) {}
		INTERBUFC_FORCEINLINE SourcePosition(size_t line, size_t column) : line(line), column(column) {}

		INTERBUFC_FORCEINLINE bool operator<(const SourcePosition &loc) const {
			if (line < loc.line)
				return true;
			if (line > loc.line)
				return false;
			return column < loc.column;
		}

		INTERBUFC_FORCEINLINE bool operator>(const SourcePosition &loc) const {
			if (line > loc.line)
				return true;
			if (line < loc.line)
				return false;
			return column > loc.column;
		}

		INTERBUFC_FORCEINLINE bool operator==(const SourcePosition &loc) const {
			return (line == loc.line) && (column == loc.column);
		}

		INTERBUFC_FORCEINLINE bool operator>=(const SourcePosition &loc) const {
			return ((*this) == loc) || ((*this) > loc);
		}

		INTERBUFC_FORCEINLINE bool operator<=(const SourcePosition &loc) const {
			return ((*this) == loc) || ((*this) < loc);
		}
	};

	struct SourceLocation {
		SourcePosition beginPosition, endPosition;
	};

	class Lexer;

	enum class TokenId : int {
		End = 0,

		Unknown,

		Comma,
		Question,
		Colon,
		Semicolon,
		LBracket,
		RBracket,
		LDBracket,
		RDBracket,
		LBrace,
		RBrace,
		LParenthese,
		RParenthese,
		At,
		Dot,
		VarArg,

		ScopeOp,
		ReturnTypeOp,
		MatchOp,
		LAndOp,
		LOrOp,
		AddOp,
		SubOp,
		MulOp,
		DivOp,
		ModOp,
		AndOp,
		OrOp,
		XorOp,
		LNotOp,
		NotOp,
		AssignOp,
		AddAssignOp,
		SubAssignOp,
		MulAssignOp,
		DivAssignOp,
		ModAssignOp,
		AndAssignOp,
		OrAssignOp,
		XorAssignOp,
		LshAssignOp,
		RshAssignOp,
		StrictEqOp,
		StrictNeqOp,
		EqOp,
		NeqOp,
		LshOp,
		RshOp,
		LtEqOp,
		GtEqOp,
		LtOp,
		GtOp,
		CmpOp,
		DollarOp,

		AsKeyword,
		ClassKeyword,
		EnumKeyword,
		ImportKeyword,
		ModuleKeyword,

		I8TypeName,
		I16TypeName,
		I32TypeName,
		I64TypeName,
		U8TypeName,
		U16TypeName,
		U32TypeName,
		U64TypeName,
		F32TypeName,
		F64TypeName,
		StringTypeName,
		BoolTypeName,
		AutoTypeName,
		VoidTypeName,
		ObjectTypeName,
		AnyTypeName,

		IntLiteral,
		LongLiteral,
		UIntLiteral,
		ULongLiteral,
		F32Literal,
		F64Literal,
		StringLiteral,
		RawStringLiteral,

		Id,

		Whitespace,
		NewLine,
		LineComment,
		BlockComment,
		DocumentationComment,

		MaxToken
	};

	INTERBUFC_FORCEINLINE bool isValidToken(TokenId tokenId) {
		return (((int)tokenId) >= 0) && (((int)tokenId) < (int)TokenId::MaxToken);
	}

	class TokenExtension {
	public:
		INTERBUFC_API virtual ~TokenExtension();

		virtual void dealloc() = 0;
	};

	class IntTokenExtension : public TokenExtension {
	public:
		int data;
		peff::RcObjectPtr<peff::Alloc> allocator;

		INTERBUFC_API IntTokenExtension(peff::Alloc *allocator, int data);
		INTERBUFC_API virtual ~IntTokenExtension();

		INTERBUFC_API virtual void dealloc() override;
	};

	class UIntTokenExtension : public TokenExtension {
	public:
		unsigned int data;
		peff::RcObjectPtr<peff::Alloc> allocator;

		INTERBUFC_API UIntTokenExtension(peff::Alloc *allocator, unsigned int data);
		INTERBUFC_API virtual ~UIntTokenExtension();

		INTERBUFC_API virtual void dealloc() override;
	};

	class LongTokenExtension : public TokenExtension {
	public:
		long long data;
		peff::RcObjectPtr<peff::Alloc> allocator;

		INTERBUFC_API LongTokenExtension(peff::Alloc *allocator, long long data);
		INTERBUFC_API virtual ~LongTokenExtension();

		INTERBUFC_API virtual void dealloc() override;
	};

	class ULongTokenExtension : public TokenExtension {
	public:
		unsigned long long data;
		peff::RcObjectPtr<peff::Alloc> allocator;

		INTERBUFC_API ULongTokenExtension(peff::Alloc *allocator, unsigned long long data);
		INTERBUFC_API virtual ~ULongTokenExtension();

		INTERBUFC_API virtual void dealloc() override;
	};

	class F32TokenExtension : public TokenExtension {
	public:
		float data;
		peff::RcObjectPtr<peff::Alloc> allocator;

		INTERBUFC_API F32TokenExtension(peff::Alloc *allocator, float data);
		INTERBUFC_API virtual ~F32TokenExtension();

		INTERBUFC_API virtual void dealloc() override;
	};

	class F64TokenExtension : public TokenExtension {
	public:
		double data;
		peff::RcObjectPtr<peff::Alloc> allocator;

		INTERBUFC_API F64TokenExtension(peff::Alloc *allocator, double data);
		INTERBUFC_API virtual ~F64TokenExtension();

		INTERBUFC_API virtual void dealloc() override;
	};

	class StringTokenExtension : public TokenExtension {
	public:
		peff::String data;
		peff::RcObjectPtr<peff::Alloc> allocator;

		INTERBUFC_API StringTokenExtension(peff::Alloc *allocator, peff::String &&data);
		INTERBUFC_API virtual ~StringTokenExtension();

		INTERBUFC_API virtual void dealloc() override;
	};

	class Document;

	template <typename T>
	using AstNodePtr = peff::SharedPtr<T>;

	class Token {
	public:
		TokenId tokenId;
		peff::RcObjectPtr<peff::Alloc> allocator;
		std::string_view sourceText;
		peff::WeakPtr<Document> document;
		SourceLocation sourceLocation;
		std::unique_ptr<TokenExtension, peff::DeallocableDeleter<TokenExtension>> exData;
		size_t index = SIZE_MAX;

		INTERBUFC_API Token(peff::Alloc *allocator, const peff::WeakPtr<Document> &document);
		INTERBUFC_API virtual ~Token();

		INTERBUFC_API void dealloc();
	};

	using OwnedTokenPtr = std::unique_ptr<Token, peff::DeallocableDeleter<Token>>;
	using TokenList = peff::DynArray<OwnedTokenPtr>;

	enum class LexicalErrorKind {
		UnrecognizedToken = 0,
		UnexpectedEndOfLine,
		PrematuredEndOfFile,
		OutOfMemory
	};

	struct LexicalError {
		SourceLocation location;
		LexicalErrorKind kind;
	};

	class Lexer {
	public:
		TokenList tokenList;
		std::optional<LexicalError> lexicalError;

		INTERBUFC_FORCEINLINE Lexer(peff::Alloc *allocator) : tokenList(allocator) {
		}
		[[nodiscard]] INTERBUFC_API std::optional<LexicalError> lex(const std::string_view &src, peff::Alloc *allocator, const peff::SharedPtr<Document> &document);
	};

	INTERBUFC_API const char *getTokenName(TokenId tokenId);
}

#endif
