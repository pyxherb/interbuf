#include "compiler.h"

using namespace interbufc;

std::string_view interbufc::g_language, interbufc::g_sourceFileName, interbufc::g_outputDirectoryPath, interbufc::g_outputFileName;

INTERBUFC_API Compiler::Compiler(peff::Alloc *allocator) : warnings(allocator), errors(allocator), allocator(allocator) {
}

INTERBUFC_API Compiler::~Compiler() {
}

std::optional<interbufc::CompilationError> interbufc::fillEnum(Compiler& compiler, AstNodePtr<EnumNode> enumNode) {
	for (size_t i = 0; i < enumNode->members.size(); ++i) {
		assert(enumNode->members.at(i)->astNodeType == AstNodeType::EnumItem);

		auto m = enumNode->members.at(i).castTo<EnumItemNode>();

		if (!m->value) {
			switch (enumNode->baseType->typeNameKind) {
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
					return CompilationError(m->tokenRange, CompilationErrorKind::EnumTypeNotIncrementable);
				case TypeNameKind::F64:
					return CompilationError(m->tokenRange, CompilationErrorKind::EnumTypeNotIncrementable);
				case TypeNameKind::Bool:
					return CompilationError(m->tokenRange, CompilationErrorKind::EnumTypeNotIncrementable);
				default:
					return CompilationError(m->tokenRange, CompilationErrorKind::InvalidEnumBaseType);
			}
			if (i)
				INTERBUFC_RETURN_IF_COMP_ERROR(incEnumValue(compiler, enumNode->selfAllocator.get(), enumNode->members.at(i - 1).castTo<EnumItemNode>()->value, m->value));
			else
				INTERBUFC_RETURN_IF_COMP_ERROR(defaultEnumValue(compiler, enumNode->selfAllocator.get(), enumNode->baseType, m->value));
		}
	}

	return {};
}

std::optional<interbufc::CompilationError> interbufc::defaultEnumValue(Compiler& compiler, peff::Alloc* allocator, AstNodePtr<TypeNameNode> typeName, AstNodePtr<ExprNode>& exprOut) {
	switch (typeName->typeNameKind) {
		case TypeNameKind::I8:
			if (!(exprOut = makeAstNode<I8LiteralExprNode>(allocator, allocator, typeName->document->sharedFromThis(), 0).castTo<ExprNode>()))
				return genOutOfMemoryCompError();
			break;
		case TypeNameKind::I16:
			if (!(exprOut = makeAstNode<I16LiteralExprNode>(allocator, allocator, typeName->document->sharedFromThis(), 0).castTo<ExprNode>()))
				return genOutOfMemoryCompError();
			break;
		case TypeNameKind::I32:
			if (!(exprOut = makeAstNode<I32LiteralExprNode>(allocator, allocator, typeName->document->sharedFromThis(), 0).castTo<ExprNode>()))
				return genOutOfMemoryCompError();
			break;
		case TypeNameKind::I64:
			if (!(exprOut = makeAstNode<I64LiteralExprNode>(allocator, allocator, typeName->document->sharedFromThis(), 0).castTo<ExprNode>()))
				return genOutOfMemoryCompError();
			break;
		case TypeNameKind::U8:
			if (!(exprOut = makeAstNode<U8LiteralExprNode>(allocator, allocator, typeName->document->sharedFromThis(), 0).castTo<ExprNode>()))
				return genOutOfMemoryCompError();
			break;
		case TypeNameKind::U16:
			if (!(exprOut = makeAstNode<U16LiteralExprNode>(allocator, allocator, typeName->document->sharedFromThis(), 0).castTo<ExprNode>()))
				return genOutOfMemoryCompError();
			break;
		case TypeNameKind::U32:
			if (!(exprOut = makeAstNode<U32LiteralExprNode>(allocator, allocator, typeName->document->sharedFromThis(), 0).castTo<ExprNode>()))
				return genOutOfMemoryCompError();
			break;
		case TypeNameKind::U64:
			if (!(exprOut = makeAstNode<U64LiteralExprNode>(allocator, allocator, typeName->document->sharedFromThis(), 0).castTo<ExprNode>()))
				return genOutOfMemoryCompError();
			break;
		case TypeNameKind::F32:
			if (!(exprOut = makeAstNode<F32LiteralExprNode>(allocator, allocator, typeName->document->sharedFromThis(), 0).castTo<ExprNode>()))
				return genOutOfMemoryCompError();
			break;
		case TypeNameKind::F64:
			if (!(exprOut = makeAstNode<F64LiteralExprNode>(allocator, allocator, typeName->document->sharedFromThis(), 0).castTo<ExprNode>()))
				return genOutOfMemoryCompError();
			break;
		default:
			std::terminate();
	}

	return {};
}

std::optional<interbufc::CompilationError> interbufc::incEnumValue(Compiler &compiler, peff::Alloc *allocator, AstNodePtr<ExprNode> expr, AstNodePtr<ExprNode> &exprOut) {
	switch (expr->exprKind) {
		case ExprKind::I8: {
			AstNodePtr<I8LiteralExprNode> e = expr.castTo<I8LiteralExprNode>();

			if (!(exprOut = makeAstNode<I8LiteralExprNode>(allocator, allocator, expr->document->sharedFromThis(), e->data + 1).castTo<ExprNode>()))
				return genOutOfMemoryCompError();
			break;
		}
		case ExprKind::I16: {
			AstNodePtr<I16LiteralExprNode> e = expr.castTo<I16LiteralExprNode>();

			if (!(exprOut = makeAstNode<I16LiteralExprNode>(allocator, allocator, expr->document->sharedFromThis(), e->data + 1).castTo<ExprNode>()))
				return genOutOfMemoryCompError();
			break;
		}
		case ExprKind::I32: {
			AstNodePtr<I32LiteralExprNode> e = expr.castTo<I32LiteralExprNode>();

			if (!(exprOut = makeAstNode<I32LiteralExprNode>(allocator, allocator, expr->document->sharedFromThis(), e->data + 1).castTo<ExprNode>()))
				return genOutOfMemoryCompError();
			break;
		}
		case ExprKind::I64: {
			AstNodePtr<I64LiteralExprNode> e = expr.castTo<I64LiteralExprNode>();

			if (!(exprOut = makeAstNode<I64LiteralExprNode>(allocator, allocator, expr->document->sharedFromThis(), e->data + 1).castTo<ExprNode>()))
				return genOutOfMemoryCompError();
			break;
		}
		case ExprKind::U8: {
			AstNodePtr<U8LiteralExprNode> e = expr.castTo<U8LiteralExprNode>();

			if (!(exprOut = makeAstNode<U8LiteralExprNode>(allocator, allocator, expr->document->sharedFromThis(), e->data + 1).castTo<ExprNode>()))
				return genOutOfMemoryCompError();
			break;
		}
		case ExprKind::U16: {
			AstNodePtr<U16LiteralExprNode> e = expr.castTo<U16LiteralExprNode>();

			if (!(exprOut = makeAstNode<U16LiteralExprNode>(allocator, allocator, expr->document->sharedFromThis(), e->data + 1).castTo<ExprNode>()))
				return genOutOfMemoryCompError();
			break;
		}
		case ExprKind::U32: {
			AstNodePtr<U32LiteralExprNode> e = expr.castTo<U32LiteralExprNode>();

			if (!(exprOut = makeAstNode<U32LiteralExprNode>(allocator, allocator, expr->document->sharedFromThis(), e->data + 1).castTo<ExprNode>()))
				return genOutOfMemoryCompError();
			break;
		}
		case ExprKind::U64: {
			AstNodePtr<U64LiteralExprNode> e = expr.castTo<U64LiteralExprNode>();

			if (!(exprOut = makeAstNode<U64LiteralExprNode>(allocator, allocator, expr->document->sharedFromThis(), e->data + 1).castTo<ExprNode>()))
				return genOutOfMemoryCompError();
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

std::optional<CompilationError> interbufc::resolveCustomTypeName(AstNodePtr<CustomTypeNameNode> typeName, AstNodePtr<MemberNode> &memberOut) {
	memberOut = {};

	AstNodePtr<MemberNode> m = typeName->contextNode.lock();

	if (typeName->cachedResolveResult.isValid()) {
		memberOut = typeName->cachedResolveResult.lock();
		return {};
	}

	for (size_t i = 0; i < typeName->idRefPtr->entries.size(); ++i) {
		IdRefEntry &entry = typeName->idRefPtr->entries.at(i);

		switch (m->astNodeType) {
			case AstNodeType::Module: {
				AstNodePtr<ModuleNode> member = m.castTo<ModuleNode>();

				if (auto it = member->memberIndices.find(entry.name); it != member->memberIndices.end()) {
					m = member->members.at(it.value());
				} else
					return {};
				break;
			}
			case AstNodeType::Class: {
				AstNodePtr<ClassNode> member = m.castTo<ClassNode>();

				if (auto it = member->memberIndices.find(entry.name); it != member->memberIndices.end()) {
					m = member->members.at(it.value());
				} else
					return {};
				break;
			}
			case AstNodeType::Struct: {
				AstNodePtr<StructNode> member = m.castTo<StructNode>();

				if (auto it = member->memberIndices.find(entry.name); it != member->memberIndices.end()) {
					m = member->members.at(it.value());
				} else
					return {};
				break;
			}
			default:
				return {};
		}
	}

	memberOut = m;
	typeName->cachedResolveResult = m;

	return {};
}
