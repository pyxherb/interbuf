#include "../parser.h"

using namespace interbufc;

INTERBUFC_API std::optional<SyntaxError> Parser::lookaheadUntil(size_t nTokenIds, const TokenId tokenIds[]) {
	// stub.
	return {};

	Token *token;
	while ((token->tokenId != TokenId::End)) {
		for(size_t i = 0 ; i < nTokenIds; ++i) {
			if(token->tokenId == tokenIds[i]) {
				return {};
			}
		}
		token = nextToken(true, true, true);
	}

	NoMatchingTokensFoundErrorExData exData(resourceAllocator.get());

	for(size_t i = 0 ; i < nTokenIds; ++i) {
		TokenId copiedTokenId = tokenIds[i];
		if(!exData.expectingTokenIds.insert(std::move(copiedTokenId)))
			return genOutOfMemoryError();
	}

	return SyntaxError(token->index, std::move(exData));
}

INTERBUFC_API Token *Parser::nextToken(bool keepNewLine, bool keepWhitespace, bool keepComment) {
	size_t &i = parseContext.idxCurrentToken;

	while (i < tokenList.size()) {
		Token *currentToken = tokenList.at(i).get();
		currentToken->index = i;

		switch (tokenList.at(i)->tokenId) {
			case TokenId::NewLine:
				if (keepNewLine) {
					parseContext.idxPrevToken = parseContext.idxCurrentToken;
					++i;
					return currentToken;
				}
				break;
			case TokenId::Whitespace:
				if (keepWhitespace) {
					parseContext.idxPrevToken = parseContext.idxCurrentToken;
					++i;
					return currentToken;
				}
				break;
			case TokenId::LineComment:
			case TokenId::BlockComment:
			case TokenId::DocumentationComment:
				if (keepComment) {
					parseContext.idxPrevToken = parseContext.idxCurrentToken;
					++i;
					return currentToken;
				}
				break;
			default:
				assert(isValidToken(currentToken->tokenId));
				parseContext.idxPrevToken = parseContext.idxCurrentToken;
				++i;
				return currentToken;
		}

		++i;
	}

	return tokenList.back().get();
}

INTERBUFC_API Token *Parser::peekToken(bool keepNewLine, bool keepWhitespace, bool keepComment) {
	size_t i = parseContext.idxCurrentToken;

	while (i < tokenList.size()) {
		Token *currentToken = tokenList.at(i).get();
		currentToken->index = i;

		switch (currentToken->tokenId) {
			case TokenId::NewLine:
				if (keepNewLine)
					return currentToken;
				break;
			case TokenId::Whitespace:
				if (keepWhitespace)
					return currentToken;
				break;
			case TokenId::LineComment:
			case TokenId::BlockComment:
			case TokenId::DocumentationComment:
				if (keepComment)
					return currentToken;
				break;
			default:
				assert(isValidToken(currentToken->tokenId));
				return currentToken;
		}

		++i;
	}

	return tokenList.back().get();
}
