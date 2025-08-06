#include "class.h"

using namespace interbufc;

INTERBUFC_API AstNodePtr<AstNode> ClassNode::doDuplicate(peff::Alloc *newAllocator) const {
	bool succeeded = false;
	AstNodePtr<ClassNode> duplicatedNode(makeAstNode<ClassNode>(newAllocator, *this, newAllocator, succeeded));
	if ((!duplicatedNode) || (!succeeded)) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API ClassNode::ClassNode(
	peff::Alloc *selfAllocator,
	const AstNodePtr<Document> &document)
	: ModuleNode(selfAllocator, document, AstNodeType::Class),
	  genericParamIndices(selfAllocator),
	  idxGenericParamCommaTokens(selfAllocator) {
}

INTERBUFC_API ClassNode::ClassNode(const ClassNode &rhs, peff::Alloc *allocator, bool &succeededOut) : ModuleNode(rhs, allocator, succeededOut), genericParamIndices(allocator), idxGenericParamCommaTokens(allocator) {
	if (!succeededOut) {
		return;
	}

	if (!idxGenericParamCommaTokens.resize(rhs.idxGenericParamCommaTokens.size())) {
		succeededOut = false;
		return;
	}

	memcpy(idxGenericParamCommaTokens.data(), rhs.idxGenericParamCommaTokens.data(), sizeof(size_t) * idxGenericParamCommaTokens.size());

	idxLAngleBracketToken = rhs.idxLAngleBracketToken;
	idxRAngleBracketToken = rhs.idxRAngleBracketToken;

	isGenericParamsIndexed = rhs.isGenericParamsIndexed;

	succeededOut = true;
}

INTERBUFC_API ClassNode::~ClassNode() {
}
