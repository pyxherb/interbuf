#include "../parser.h"
#include <climits>

using namespace interbufc;

INTERBUFC_API std::optional<SyntaxError> Parser::parseTypeName(AstNodePtr<TypeNameNode> &typeNameOut, bool withCircumfixes) {
	std::optional<SyntaxError> syntaxError;
	Token *t = peekToken();

	switch (t->tokenId) {
		case TokenId::VoidTypeName:
			if (!(typeNameOut = makeAstNode<VoidTypeNameNode>(
					  resourceAllocator.get(),
					  resourceAllocator.get(), document)
						.castTo<TypeNameNode>()))
				return genOutOfMemoryError();
			typeNameOut->tokenRange = TokenRange{ t->index };
			nextToken();
			break;
		case TokenId::I8TypeName:
			if (!(typeNameOut = makeAstNode<I8TypeNameNode>(
					  resourceAllocator.get(),
					  resourceAllocator.get(), document)
						.castTo<TypeNameNode>()))
				return genOutOfMemoryError();
			typeNameOut->tokenRange = TokenRange{ t->index };
			nextToken();
			break;
		case TokenId::I16TypeName:
			if (!(typeNameOut = makeAstNode<I16TypeNameNode>(
					  resourceAllocator.get(),
					  resourceAllocator.get(), document)
						.castTo<TypeNameNode>()))
				return genOutOfMemoryError();
			typeNameOut->tokenRange = TokenRange{ t->index };
			nextToken();
			break;
		case TokenId::I32TypeName:
			if (!(typeNameOut = makeAstNode<I32TypeNameNode>(
					  resourceAllocator.get(),
					  resourceAllocator.get(), document)
						.castTo<TypeNameNode>()))
				return genOutOfMemoryError();
			typeNameOut->tokenRange = TokenRange{ t->index };
			nextToken();
			break;
		case TokenId::I64TypeName:
			if (!(typeNameOut = makeAstNode<I64TypeNameNode>(
					  resourceAllocator.get(),
					  resourceAllocator.get(), document)
						.castTo<TypeNameNode>()))
				return genOutOfMemoryError();
			typeNameOut->tokenRange = TokenRange{ t->index };
			nextToken();
			break;
		case TokenId::U8TypeName:
			if (!(typeNameOut = makeAstNode<U8TypeNameNode>(
					  resourceAllocator.get(),
					  resourceAllocator.get(), document)
						.castTo<TypeNameNode>()))
				return genOutOfMemoryError();
			typeNameOut->tokenRange = TokenRange{ t->index };
			nextToken();
			break;
		case TokenId::U16TypeName:
			if (!(typeNameOut = makeAstNode<U16TypeNameNode>(
					  resourceAllocator.get(),
					  resourceAllocator.get(), document)
						.castTo<TypeNameNode>()))
				return genOutOfMemoryError();
			typeNameOut->tokenRange = TokenRange{ t->index };
			nextToken();
			break;
		case TokenId::U32TypeName:
			if (!(typeNameOut = makeAstNode<U32TypeNameNode>(
					  resourceAllocator.get(),
					  resourceAllocator.get(), document)
						.castTo<TypeNameNode>()))
				return genOutOfMemoryError();
			typeNameOut->tokenRange = TokenRange{ t->index };
			nextToken();
			break;
		case TokenId::U64TypeName:
			if (!(typeNameOut = makeAstNode<U64TypeNameNode>(
					  resourceAllocator.get(),
					  resourceAllocator.get(), document)
						.castTo<TypeNameNode>()))
				return genOutOfMemoryError();
			typeNameOut->tokenRange = TokenRange{ t->index };
			nextToken();
			break;
		case TokenId::F32TypeName:
			if (!(typeNameOut = makeAstNode<F32TypeNameNode>(
					  resourceAllocator.get(),
					  resourceAllocator.get(), document)
						.castTo<TypeNameNode>()))
				return genOutOfMemoryError();
			typeNameOut->tokenRange = TokenRange{ t->index };
			nextToken();
			break;
		case TokenId::F64TypeName:
			if (!(typeNameOut = makeAstNode<F64TypeNameNode>(
					  resourceAllocator.get(),
					  resourceAllocator.get(), document)
						.castTo<TypeNameNode>()))
				return genOutOfMemoryError();
			typeNameOut->tokenRange = TokenRange{ t->index };
			nextToken();
			break;
		case TokenId::BoolTypeName:
			if (!(typeNameOut = makeAstNode<BoolTypeNameNode>(
					  resourceAllocator.get(),
					  resourceAllocator.get(), document)
						.castTo<TypeNameNode>()))
				return genOutOfMemoryError();
			typeNameOut->tokenRange = TokenRange{ t->index };
			nextToken();
			break;
		case TokenId::StringTypeName:
			if (!(typeNameOut = makeAstNode<StringTypeNameNode>(
					  resourceAllocator.get(),
					  resourceAllocator.get(), document)
						.castTo<TypeNameNode>()))
				return genOutOfMemoryError();
			typeNameOut->tokenRange = TokenRange{ t->index };
			nextToken();
			break;
		case TokenId::Id: {
			IdRefPtr id;
			if ((syntaxError = parseIdRef(id)))
				return syntaxError;

			AstNodePtr<CustomTypeNameNode> tn;

			if (!(tn = makeAstNode<CustomTypeNameNode>(
					  resourceAllocator.get(),
					  resourceAllocator.get(),
					  document)))
				return genOutOfMemoryError();

			tn->contextNode = curParent;

			tn->tokenRange = id->tokenRange;
			tn->idRefPtr = std::move(id);

			typeNameOut = tn.castTo<TypeNameNode>();

			break;
		}
		default:
			return SyntaxError(TokenRange{ t->index }, SyntaxErrorKind::UnexpectedToken);
	}

	if (withCircumfixes) {
		while (true) {
			switch ((t = peekToken())->tokenId) {
				case TokenId::LBracket: {
					nextToken();

					Token *rBracketToken;
					if ((syntaxError = expectToken((rBracketToken = peekToken()), TokenId::RBracket)))
						return SyntaxError(TokenRange{ rBracketToken->index }, ExpectingSingleTokenErrorExData{ TokenId::RBracket });

					nextToken();

					if (!(typeNameOut = makeAstNode<ArrayTypeNameNode>(
							  resourceAllocator.get(),
							  resourceAllocator.get(),
							  document,
							  typeNameOut)
								.castTo<TypeNameNode>()))
						return genOutOfMemoryError();
					break;
				}
				default:
					goto end;
			}
		}
	}

end:
	if (withCircumfixes) {
		if ((t = peekToken())->tokenId == TokenId::AndOp) {
			nextToken();
			if (!(typeNameOut = makeAstNode<RefTypeNameNode>(
					  resourceAllocator.get(),
					  resourceAllocator.get(),
					  document,
					  typeNameOut)
						.castTo<TypeNameNode>()))
				return genOutOfMemoryError();
		} else if ((t = peekToken())->tokenId == TokenId::LAndOp) {
			nextToken();
			if (!(typeNameOut = makeAstNode<TempRefTypeNameNode>(
					  resourceAllocator.get(),
					  resourceAllocator.get(),
					  document,
					  typeNameOut)
						.castTo<TypeNameNode>()))
				return genOutOfMemoryError();
		}
	}

	return {};
}
