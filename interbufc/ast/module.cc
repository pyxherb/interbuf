#include "module.h"
#include "import.h"
#include "parser.h"
#include "document.h"

using namespace interbufc;

INTERBUFC_API MemberNode::MemberNode(
	AstNodeType astNodeType,
	peff::Alloc *selfAllocator,
	const AstNodePtr<Document> &document)
	: AstNode(astNodeType, selfAllocator, document),
	  name(selfAllocator),
	  genericArgs(selfAllocator) {
}

INTERBUFC_API MemberNode::MemberNode(const MemberNode &rhs, peff::Alloc *allocator, bool &succeededOut) : AstNode(rhs, allocator), name(allocator), genericArgs(allocator) {
	if (!name.build(rhs.name)) {
		succeededOut = false;
		return;
	}

	if (!genericArgs.resize(rhs.genericArgs.size())) {
		succeededOut = false;
		return;
	}

	for (size_t i = 0; i < genericArgs.size(); ++i) {
		if (!(genericArgs.at(i) = rhs.genericArgs.at(i)->duplicate<TypeNameNode>(allocator))) {
			succeededOut = false;
			return;
		}
	}

	succeededOut = true;
}

INTERBUFC_API MemberNode::~MemberNode() {
}

INTERBUFC_API AstNodePtr<AstNode> ModuleNode::doDuplicate(peff::Alloc *newAllocator) const {
	bool succeeded = false;
	AstNodePtr<ModuleNode> duplicatedNode(makeAstNode<ModuleNode>(newAllocator, *this, newAllocator, succeeded));
	if ((!duplicatedNode) || (!succeeded)) {
		return {};
	}

	return duplicatedNode.castTo<AstNode>();
}

INTERBUFC_API ModuleNode::ModuleNode(
	peff::Alloc *selfAllocator,
	const AstNodePtr<Document> &document,
	AstNodeType astNodeType)
	: MemberNode(astNodeType, selfAllocator, document),
	  members(selfAllocator),
	  memberIndices(selfAllocator),
	  anonymousImports(selfAllocator) {
}

INTERBUFC_API ModuleNode::ModuleNode(const ModuleNode &rhs, peff::Alloc *allocator, bool &succeededOut) : MemberNode(rhs, allocator, succeededOut), members(allocator), memberIndices(allocator), anonymousImports(allocator) {
	if (!succeededOut) {
		return;
	}

	parser = rhs.parser;

	if (!anonymousImports.resize(rhs.anonymousImports.size())) {
		succeededOut = false;
		return;
	}

	for (size_t i = 0; i < anonymousImports.size(); ++i) {
		if (!(anonymousImports.at(i) = rhs.anonymousImports.at(i)->duplicate<ImportNode>(allocator))) {
			succeededOut = false;
			return;
		}
	}

	if (!members.resize(rhs.members.size())) {
		succeededOut = false;
		return;
	}

	for (size_t i = 0; i < members.size(); ++i) {
		AstNodePtr<MemberNode> &m = members.at(i);
		const AstNodePtr<MemberNode> &rm = rhs.members.at(i);
		if (!(m = rm->duplicate<MemberNode>(allocator))) {
			succeededOut = false;
			return;
		}

		if (!indexMember(i)) {
			succeededOut = false;
			return;
		}
	}

	isVarDefStmtsNormalized = rhs.isVarDefStmtsNormalized;

	succeededOut = true;
}

INTERBUFC_API ModuleNode::~ModuleNode() {
}

INTERBUFC_API size_t ModuleNode::pushMember(AstNodePtr<MemberNode> memberNode) noexcept {
	size_t n = members.size();

	if (!members.pushBack(std::move(memberNode))) {
		return SIZE_MAX;
	}

	return n;
}

INTERBUFC_API bool ModuleNode::addMember(AstNodePtr<MemberNode> memberNode) noexcept {
	size_t index;

	if ((index = pushMember(memberNode)) == SIZE_MAX) {
		return false;
	}

	return indexMember(index);
}

INTERBUFC_API bool ModuleNode::indexMember(size_t indexInMemberArray) noexcept {
	AstNodePtr<MemberNode> m = members.at(indexInMemberArray);

	if (!memberIndices.insert(m->name, +indexInMemberArray)) {
		return false;
	}

	m->setParent(this);

	return true;
}

INTERBUFC_API bool ModuleNode::removeMember(const std::string_view &name) noexcept {
	size_t index = memberIndices.at(name);
	if (!members.eraseRange(index, index + 1)) {
		return false;
	}
	memberIndices.remove(name);
	for (auto i : memberIndices) {
		if (i.second > index) {
			--i.second;
		}
	}
	return true;
}

INTERBUFC_API void ModuleNode::setParser(AstNodePtr<Parser> parser) {
	parser->document = {};
	parser->curParent = {};
	this->parser = parser;
}
