#include "parser.h"

using namespace interbufc;

INTERBUFC_API Parser::Parser(AstNodePtr<Document> document, TokenList &&tokenList, peff::Alloc *resourceAllocator) : document(document), tokenList(std::move(tokenList)), resourceAllocator(resourceAllocator), syntaxErrors(resourceAllocator) {
}

INTERBUFC_API Parser::~Parser() {
	assert(!document);
}

INTERBUFC_API std::optional<SyntaxError> Parser::parseIdName(peff::String &nameOut) {
	std::optional<SyntaxError> syntaxError;
	Token *t = peekToken();

	switch (t->tokenId) {
		case TokenId::Id:
			if (!nameOut.build(t->sourceText)) {
				return genOutOfMemoryError();
			}
			nextToken();
			break;
		default:
			return SyntaxError(TokenRange{ t->index }, SyntaxErrorKind::ExpectingId);
	}
	return {};
}

INTERBUFC_API std::optional<SyntaxError> Parser::parseIdRef(IdRefPtr &idRefOut) {
	std::optional<SyntaxError> syntaxError;
	IdRefPtr idRefPtr(peff::allocAndConstruct<IdRef>(resourceAllocator.get(), ASTNODE_ALIGNMENT, resourceAllocator.get()));
	if (!idRefPtr)
		return genOutOfMemoryError();
	Token *t = peekToken();

	idRefPtr->tokenRange = TokenRange{ t->index };

	if (t->tokenId == TokenId::ScopeOp) {
		nextToken();

		IdRefEntry entry(resourceAllocator.get());
		peff::String idText(resourceAllocator.get());

		entry.name = std::move(idText);

		entry.accessOpTokenIndex = t->index;

		if (!idRefPtr->entries.pushBack(std::move(entry)))
			return genOutOfMemoryError();
	}

	for (;;) {
		if ((syntaxError = expectToken(t = peekToken(), TokenId::Id)))
			return syntaxError;

		nextToken();

		IdRefEntry entry(resourceAllocator.get());
		peff::String idText(resourceAllocator.get());
		if (!idText.build(t->sourceText)) {
			return genOutOfMemoryError();
		}

		entry.name = std::move(idText);
		entry.nameTokenIndex = t->index;
		idRefPtr->tokenRange.endIndex = t->index;

		if (!idRefPtr->entries.pushBack(std::move(entry)))
			return genOutOfMemoryError();

		if ((t = peekToken())->tokenId != TokenId::Dot) {
			break;
		}

		entry.accessOpTokenIndex = t->index;
		idRefPtr->tokenRange.endIndex = t->index;

		nextToken();
	}

	idRefOut = std::move(idRefPtr);

	return {};
}

INTERBUFC_API std::optional<SyntaxError> Parser::parseClassStmts() {
	std::optional<SyntaxError> syntaxError;

	Token *t;

	while ((t = peekToken())->tokenId != TokenId::RBrace) {
		if ((syntaxError = parseProgramStmt())) {
			// Parse the rest to make sure that we have gained all of the information,
			// instead of ignoring them.
			if (!syntaxErrors.pushBack(std::move(syntaxError.value())))
				return genOutOfMemoryError();
			syntaxError.reset();
		}
	}

	return {};
}

INTERBUFC_API std::optional<SyntaxError> Parser::parseClassMember() {
	std::optional<SyntaxError> syntaxError;

	Token *token = peekToken();

	AstNodePtr<ModuleNode> p = curParent.castTo<ModuleNode>();

	Token *nameToken;

	if ((syntaxError = expectToken((nameToken = peekToken()), TokenId::Id)))
		return syntaxError;

	Token *colonToken;

	if ((syntaxError = expectToken((colonToken = peekToken()), TokenId::Colon)))
		return syntaxError;

	peff::SharedPtr<TypeNameNode> typeName;

	if ((syntaxError = parseTypeName(typeName, true)))
		return syntaxError;

	return {};
}

INTERBUFC_API std::optional<SyntaxError> Parser::parseProgramStmt() {
	std::optional<SyntaxError> syntaxError;

	Token *token = peekToken();

	AstNodePtr<ModuleNode> p = curParent.castTo<ModuleNode>();

	switch (token->tokenId) {
		case TokenId::ClassKeyword: {
			// Class.
			nextToken();

			AstNodePtr<ClassNode> classNode;

			if (!(classNode = makeAstNode<ClassNode>(resourceAllocator.get(), resourceAllocator.get(), document))) {
				return genOutOfMemoryError();
			}

			Token *nameToken;

			if ((syntaxError = expectToken((nameToken = peekToken()), TokenId::Id))) {
				return syntaxError;
			}

			size_t idxMember;

			if ((idxMember = p->pushMember(classNode.castTo<MemberNode>())) == SIZE_MAX) {
				return genOutOfMemoryError();
			}

			nextToken();

			if (!classNode->name.build(nameToken->sourceText)) {
				return genOutOfMemoryError();
			}

			{
				peff::ScopeGuard setTokenRangeGuard([this, token, classNode]() noexcept {
					classNode->tokenRange = TokenRange{ token->index, parseContext.idxPrevToken };
				});

				AstNodePtr<MemberNode> prevParent;
				prevParent = curParent;
				peff::ScopeGuard restorePrevModGuard([this, prevParent]() noexcept {
					curParent = prevParent;
				});
				curParent = classNode.castTo<MemberNode>();

				Token *lBraceToken;

				if ((syntaxError = expectToken((lBraceToken = peekToken()), TokenId::LBrace))) {
					return syntaxError;
				}

				nextToken();

				Token *currentToken;
				while (true) {
					if ((syntaxError = expectToken(currentToken = peekToken()))) {
						return syntaxError;
					}
					if (currentToken->tokenId == TokenId::RBrace) {
						break;
					}

					if ((syntaxError = parseProgramStmt())) {
						// Parse the rest to make sure that we have gained all of the information,
						// instead of ignoring them.
						if (!syntaxErrors.pushBack(std::move(syntaxError.value())))
							return genOutOfMemoryError();
						syntaxError.reset();
					}
				}

				Token *rBraceToken;

				if ((syntaxError = expectToken((rBraceToken = peekToken()), TokenId::RBrace))) {
					return syntaxError;
				}

				nextToken();
			}

			if (auto it = p->memberIndices.find(classNode->name); it != p->memberIndices.end()) {
				peff::String s(resourceAllocator.get());

				if (!s.build(classNode->name)) {
					return genOutOfMemoryError();
				}

				ConflictingDefinitionsErrorExData exData(std::move(s));

				return SyntaxError(classNode->tokenRange, std::move(exData));
			} else {
				if (!(p->indexMember(idxMember))) {
					return genOutOfMemoryError();
				}
			}

			break;
		}
		case TokenId::ImportKeyword: {
			// Import item.
			nextToken();

			AstNodePtr<ImportNode> importNode;

			if (!(importNode = makeAstNode<ImportNode>(resourceAllocator.get(), resourceAllocator.get(), document))) {
				return genOutOfMemoryError();
			}

			if ((syntaxError = parseIdRef(importNode->idRef)))
				return syntaxError;

			size_t idxMember;
			if ((idxMember = p->pushMember(importNode.castTo<MemberNode>())) == SIZE_MAX) {
				return genOutOfMemoryError();
			}

			if (Token *asToken = peekToken(); asToken->tokenId == TokenId::AsKeyword) {
				nextToken();

				Token *nameToken;

				if ((syntaxError = expectToken((nameToken = peekToken()), TokenId::Id))) {
					return syntaxError;
				}

				if (!importNode->name.build(nameToken->sourceText)) {
					return genOutOfMemoryError();
				}

				if (!p->indexMember(idxMember)) {
					return genOutOfMemoryError();
				}
			} else {
				if (!p->anonymousImports.pushBack(AstNodePtr<ImportNode>(importNode))) {
					return genOutOfMemoryError();
				}
			}

			Token *semicolonToken;

			if ((syntaxError = expectToken((semicolonToken = peekToken()), TokenId::Semicolon))) {
				return syntaxError;
			}

			nextToken();

			break;
		}
		default:
			nextToken();
			return SyntaxError(
				TokenRange{ token->index },
				SyntaxErrorKind::ExpectingDecl);
	}

	return {};
}

INTERBUFC_API std::optional<SyntaxError> Parser::parseProgram(const AstNodePtr<ModuleNode> &initialMod, IdRefPtr &moduleNameOut) {
	std::optional<SyntaxError> syntaxError;

	Token *t;

	curParent = initialMod.castTo<MemberNode>();

	moduleNameOut = {};
	if ((t = peekToken())->tokenId == TokenId::ModuleKeyword) {
		nextToken();

		IdRefPtr moduleName;

		if ((syntaxError = parseIdRef(moduleName))) {
			if (!syntaxErrors.pushBack(std::move(syntaxError.value())))
				return genOutOfMemoryError();
			syntaxError.reset();
		}

		Token *semicolonToken;
		if ((syntaxError = expectToken((semicolonToken = peekToken()), TokenId::Semicolon))) {
			return syntaxError;
		}
		nextToken();

		moduleNameOut = std::move(moduleName);
	}

	while ((t = peekToken())->tokenId != TokenId::End) {
		if ((syntaxError = parseProgramStmt())) {
			// Parse the rest to make sure that we have gained all of the information,
			// instead of ignoring them.
			if (!syntaxErrors.pushBack(std::move(syntaxError.value())))
				return genOutOfMemoryError();
			syntaxError.reset();
		}
	}

	initialMod->setParser(sharedFromThis());

	return {};
}
