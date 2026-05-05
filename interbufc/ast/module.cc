#include "module.h"
#include "import.h"
#include "parser.h"
#include "document.h"

using namespace interbufc;

INTERBUFC_API MemberNode::MemberNode(
	AstNodeType ast_node_type,
	peff::Alloc *self_allocator,
	const AstNodePtr<Document> &document)
	: AstNode(ast_node_type, self_allocator, document),
	  name(self_allocator) {
}

INTERBUFC_API MemberNode::MemberNode(const MemberNode &rhs, peff::Alloc *allocator, bool &succeeded_out) : AstNode(rhs, allocator), name(allocator) {
	if (!name.build(rhs.name)) {
		succeeded_out = false;
		return;
	}

	succeeded_out = true;
}

INTERBUFC_API MemberNode::~MemberNode() {
}

INTERBUFC_API AstNodePtr<AstNode> ModuleNode::do_duplicate(peff::Alloc *new_allocator) const {
	bool succeeded = false;
	AstNodePtr<ModuleNode> duplicated_node(make_ast_node<ModuleNode>(new_allocator, *this, new_allocator, succeeded));
	if ((!duplicated_node) || (!succeeded)) {
		return {};
	}

	return duplicated_node.cast_to<AstNode>();
}

INTERBUFC_API ModuleNode::ModuleNode(
	peff::Alloc *self_allocator,
	const AstNodePtr<Document> &document,
	AstNodeType ast_node_type)
	: MemberNode(ast_node_type, self_allocator, document),
	  members(self_allocator),
	  member_indices(self_allocator),
	  anonymous_imports(self_allocator) {
}

INTERBUFC_API ModuleNode::ModuleNode(const ModuleNode &rhs, peff::Alloc *allocator, bool &succeeded_out) : MemberNode(rhs, allocator, succeeded_out), members(allocator), member_indices(allocator), anonymous_imports(allocator) {
	if (!succeeded_out) {
		return;
	}

	parser = rhs.parser;

	if (!anonymous_imports.resize(rhs.anonymous_imports.size())) {
		succeeded_out = false;
		return;
	}

	for (size_t i = 0; i < anonymous_imports.size(); ++i) {
		if (!(anonymous_imports.at(i) = rhs.anonymous_imports.at(i)->duplicate<ImportNode>(allocator))) {
			succeeded_out = false;
			return;
		}
	}

	if (!members.resize(rhs.members.size())) {
		succeeded_out = false;
		return;
	}

	for (size_t i = 0; i < members.size(); ++i) {
		AstNodePtr<MemberNode> &m = members.at(i);
		const AstNodePtr<MemberNode> &rm = rhs.members.at(i);
		if (!(m = rm->duplicate<MemberNode>(allocator))) {
			succeeded_out = false;
			return;
		}

		if (!index_member(i)) {
			succeeded_out = false;
			return;
		}
	}

	is_var_def_stmts_normalized = rhs.is_var_def_stmts_normalized;

	succeeded_out = true;
}

INTERBUFC_API ModuleNode::~ModuleNode() {
}

INTERBUFC_API size_t ModuleNode::push_member(AstNodePtr<MemberNode> member_node) noexcept {
	if (!members.push_back(std::move(member_node))) {
		return SIZE_MAX;
	}

	return members.size() - 1;
}

INTERBUFC_API bool ModuleNode::add_member(AstNodePtr<MemberNode> member_node) noexcept {
	size_t index;

	if ((index = push_member(member_node)) == SIZE_MAX) {
		return false;
	}

	return index_member(index);
}

INTERBUFC_API bool ModuleNode::index_member(size_t index_in_member_array) noexcept {
	AstNodePtr<MemberNode> m = members.at(index_in_member_array);

	if (!member_indices.insert(m->name, +index_in_member_array)) {
		return false;
	}

	m->set_parent(this);

	return true;
}

INTERBUFC_API void ModuleNode::remove_member(const std::string_view &name) noexcept {
	size_t index = member_indices.at(name);
	members.erase_range(index, index + 1);
	member_indices.remove(name);
	for (auto i : member_indices) {
		if (i.second > index) {
			--i.second;
		}
	}
}

INTERBUFC_API void ModuleNode::set_parser(AstNodePtr<Parser> parser) {
	parser->document = {};
	parser->cur_parent = {};
	this->parser = parser;
}
