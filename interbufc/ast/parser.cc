#include "parser.h"

using namespace interbufc;

INTERBUFC_API Parser::Parser(AstNodePtr<Document> document, TokenList &&token_list, peff::Alloc *resource_allocator) : document(document), token_list(std::move(token_list)), resource_allocator(resource_allocator), syntax_errors(resource_allocator) {
}

INTERBUFC_API Parser::~Parser() {
	assert(!document);
}

INTERBUFC_API std::optional<SyntaxError> Parser::parse_id_name(peff::String &name_out) {
	std::optional<SyntaxError> syntax_error;
	Token *t = peek_token();

	switch (t->token_id) {
		case TokenId::Id:
			if (!name_out.build(t->source_text)) {
				return gen_out_of_memory_error();
			}
			next_token();
			break;
		default:
			return SyntaxError(TokenRange{ t->index }, SyntaxErrorKind::ExpectingId);
	}
	return {};
}

INTERBUFC_API std::optional<SyntaxError> Parser::parse_id_ref(IdRefPtr &id_ref_out) {
	std::optional<SyntaxError> syntax_error;
	IdRefPtr id_ref_ptr(peff::alloc_and_construct<IdRef>(resource_allocator.get(), ASTNODE_ALIGNMENT, resource_allocator.get()));
	if (!id_ref_ptr)
		return gen_out_of_memory_error();
	Token *t = peek_token();

	id_ref_ptr->token_range = TokenRange{ t->index };

	if (t->token_id == TokenId::ScopeOp) {
		next_token();

		IdRefEntry entry(resource_allocator.get());
		peff::String id_text(resource_allocator.get());

		entry.name = std::move(id_text);

		entry.access_op_token_index = t->index;

		if (!id_ref_ptr->entries.push_back(std::move(entry)))
			return gen_out_of_memory_error();
	}

	for (;;) {
		if ((syntax_error = expect_token(t = peek_token(), TokenId::Id)))
			return syntax_error;

		next_token();

		IdRefEntry entry(resource_allocator.get());
		peff::String id_text(resource_allocator.get());
		if (!id_text.build(t->source_text)) {
			return gen_out_of_memory_error();
		}

		entry.name = std::move(id_text);
		entry.name_token_index = t->index;
		id_ref_ptr->token_range.end_index = t->index;

		if (!id_ref_ptr->entries.push_back(std::move(entry)))
			return gen_out_of_memory_error();

		if ((t = peek_token())->token_id != TokenId::Dot) {
			break;
		}

		entry.access_op_token_index = t->index;
		id_ref_ptr->token_range.end_index = t->index;

		next_token();
	}

	id_ref_out = std::move(id_ref_ptr);

	return {};
}

INTERBUFC_API std::optional<SyntaxError> Parser::parse_var_field(AstNodePtr<VarNode> &var_node_out) {
	std::optional<SyntaxError> syntax_error;

	Token *token = peek_token();

	AstNodePtr<ModuleNode> p = cur_parent.cast_to<ModuleNode>();

	Token *name_token;

	if ((syntax_error = expect_token((name_token = peek_token()), TokenId::Id)))
		return syntax_error;

	next_token();

	if (!(var_node_out = make_ast_node<VarNode>(resource_allocator.get(), resource_allocator.get(), document))) {
		return gen_out_of_memory_error();
	}

	if (!var_node_out->name.build(name_token->source_text))
		return gen_out_of_memory_error();

	peff::ScopeGuard set_token_range_guard([this, token, var_node_out]() noexcept {
		var_node_out->token_range = TokenRange{ token->index, parse_context.idx_prev_token };
	});

	Token *colon_token;

	if ((syntax_error = expect_token((colon_token = peek_token()), TokenId::Colon)))
		return syntax_error;

	next_token();

	peff::SharedPtr<TypeNameNode> type_name;

	if ((syntax_error = parse_type_name(var_node_out->type, true)))
		return syntax_error;

	return {};
}

INTERBUFC_API std::optional<SyntaxError> Parser::parse_program_stmt() {
	std::optional<SyntaxError> syntax_error;

	Token *token = peek_token();

	AstNodePtr<ModuleNode> p = cur_parent.cast_to<ModuleNode>();

	switch (token->token_id) {
		case TokenId::ClassKeyword: {
			// Class.
			next_token();

			AstNodePtr<ClassNode> class_node;

			if (!(class_node = make_ast_node<ClassNode>(resource_allocator.get(), resource_allocator.get(), document))) {
				return gen_out_of_memory_error();
			}

			Token *name_token;

			if ((syntax_error = expect_token((name_token = peek_token()), TokenId::Id))) {
				return syntax_error;
			}

			size_t idx_member;

			if ((idx_member = p->push_member(class_node.cast_to<MemberNode>())) == SIZE_MAX) {
				return gen_out_of_memory_error();
			}

			{
				peff::ScopeGuard restore_parent_guard([original_p = p, &p]() noexcept {
					p = original_p;
				});
				p = class_node.cast_to<ModuleNode>();

				next_token();

				if (!class_node->name.build(name_token->source_text)) {
					return gen_out_of_memory_error();
				}

				{
					peff::ScopeGuard set_token_range_guard([this, token, class_node]() noexcept {
						class_node->token_range = TokenRange{ token->index, parse_context.idx_prev_token };
					});

					Token *l_brace_token;

					if ((syntax_error = expect_token((l_brace_token = peek_token()), TokenId::LBrace))) {
						return syntax_error;
					}

					next_token();

					Token *current_token;
					peff::SharedPtr<VarNode> member_field;
					for (;;) {
						if ((syntax_error = parse_var_field(member_field))) {
							// Parse the rest to make sure that we have gained all of the information,
							// instead of ignoring them.
							if (!syntax_errors.push_back(std::move(syntax_error.value())))
								return gen_out_of_memory_error();
							syntax_error.reset();
						}

						size_t idx_var_member;

						if ((idx_var_member = p->push_member(member_field.cast_to<MemberNode>())) == SIZE_MAX) {
							return gen_out_of_memory_error();
						}

						if (auto it = p->member_indices.find(member_field->name); it != p->member_indices.end()) {
							peff::String s(resource_allocator.get());

							if (!s.build(member_field->name)) {
								return gen_out_of_memory_error();
							}

							ConflictingDefinitionsErrorExData ex_data(std::move(s));

							return SyntaxError(member_field->token_range, std::move(ex_data));
						} else {
							if (!(p->index_member(idx_var_member))) {
								return gen_out_of_memory_error();
							}
						}

						Token *comma_token;

						if ((comma_token = peek_token())->token_id != TokenId::Comma) {
							break;
						}

						next_token();
					}

					Token *r_brace_token;

					if ((syntax_error = expect_token((r_brace_token = peek_token()), TokenId::RBrace))) {
						return syntax_error;
					}

					next_token();
				}
			}

			if (auto it = p->member_indices.find(class_node->name); it != p->member_indices.end()) {
				peff::String s(resource_allocator.get());

				if (!s.build(class_node->name)) {
					return gen_out_of_memory_error();
				}

				ConflictingDefinitionsErrorExData ex_data(std::move(s));

				return SyntaxError(class_node->token_range, std::move(ex_data));
			} else {
				if (!(p->index_member(idx_member))) {
					return gen_out_of_memory_error();
				}
			}

			break;
		}
		case TokenId::StructKeyword: {
			// Struct.
			next_token();

			AstNodePtr<StructNode> struct_node;

			if (!(struct_node = make_ast_node<StructNode>(resource_allocator.get(), resource_allocator.get(), document))) {
				return gen_out_of_memory_error();
			}

			Token *name_token;

			if ((syntax_error = expect_token((name_token = peek_token()), TokenId::Id))) {
				return syntax_error;
			}

			size_t idx_member;

			if ((idx_member = p->push_member(struct_node.cast_to<MemberNode>())) == SIZE_MAX) {
				return gen_out_of_memory_error();
			}

			next_token();

			{
				peff::ScopeGuard restore_parent_guard([original_p = p, &p]() noexcept {
					p = original_p;
				});
				p = struct_node.cast_to<ModuleNode>();

				if (!struct_node->name.build(name_token->source_text)) {
					return gen_out_of_memory_error();
				}

				{
					peff::ScopeGuard set_token_range_guard([this, token, struct_node]() noexcept {
						struct_node->token_range = TokenRange{ token->index, parse_context.idx_prev_token };
					});

					Token *l_brace_token;

					if ((syntax_error = expect_token((l_brace_token = peek_token()), TokenId::LBrace))) {
						return syntax_error;
					}

					next_token();

					Token *current_token;
					peff::SharedPtr<VarNode> member_field;
					for (;;) {
						if ((syntax_error = parse_var_field(member_field))) {
							// Parse the rest to make sure that we have gained all of the information,
							// instead of ignoring them.
							if (!syntax_errors.push_back(std::move(syntax_error.value())))
								return gen_out_of_memory_error();
							syntax_error.reset();
						}

						size_t idx_var_member;

						if ((idx_var_member = p->push_member(member_field.cast_to<MemberNode>())) == SIZE_MAX) {
							return gen_out_of_memory_error();
						}

						if (auto it = p->member_indices.find(member_field->name); it != p->member_indices.end()) {
							peff::String s(resource_allocator.get());

							if (!s.build(member_field->name)) {
								return gen_out_of_memory_error();
							}

							ConflictingDefinitionsErrorExData ex_data(std::move(s));

							return SyntaxError(member_field->token_range, std::move(ex_data));
						} else {
							if (!(p->index_member(idx_var_member))) {
								return gen_out_of_memory_error();
							}
						}

						Token *comma_token;

						if ((comma_token = peek_token())->token_id != TokenId::Comma) {
							break;
						}

						next_token();
					}

					Token *r_brace_token;

					if ((syntax_error = expect_token((r_brace_token = peek_token()), TokenId::RBrace))) {
						return syntax_error;
					}

					next_token();
				}
			}

			if (auto it = p->member_indices.find(struct_node->name); it != p->member_indices.end()) {
				peff::String s(resource_allocator.get());

				if (!s.build(struct_node->name)) {
					return gen_out_of_memory_error();
				}

				ConflictingDefinitionsErrorExData ex_data(std::move(s));

				return SyntaxError(struct_node->token_range, std::move(ex_data));
			} else {
				if (!(p->index_member(idx_member))) {
					return gen_out_of_memory_error();
				}
			}

			break;
		}
		case TokenId::EnumKeyword: {
			// Class.
			next_token();

			AstNodePtr<EnumNode> enum_node;

			if (!(enum_node = make_ast_node<EnumNode>(resource_allocator.get(), resource_allocator.get(), document))) {
				return gen_out_of_memory_error();
			}

			Token *name_token;

			if ((syntax_error = expect_token((name_token = peek_token()), TokenId::Id))) {
				return syntax_error;
			}

			size_t idx_member;

			if ((idx_member = p->push_member(enum_node.cast_to<MemberNode>())) == SIZE_MAX) {
				return gen_out_of_memory_error();
			}

			next_token();

			{
				peff::ScopeGuard restore_parent_guard([original_p = p, &p]() noexcept {
					p = original_p;
				});
				p = enum_node.cast_to<ModuleNode>();

				if (!enum_node->name.build(name_token->source_text)) {
					return gen_out_of_memory_error();
				}

				{
					peff::ScopeGuard set_token_range_guard([this, token, enum_node]() noexcept {
						enum_node->token_range = TokenRange{ token->index, parse_context.idx_prev_token };
					});

					Token *colon_token;

					if ((syntax_error = expect_token((colon_token = peek_token()), TokenId::Colon))) {
						return syntax_error;
					}

					next_token();

					if ((syntax_error = parse_type_name(enum_node->base_type, false))) {
						return syntax_error;
					}

					Token *l_brace_token;

					if ((syntax_error = expect_token((l_brace_token = peek_token()), TokenId::LBrace))) {
						return syntax_error;
					}

					next_token();

					peff::SharedPtr<EnumItemNode> member_field;
					for (;;) {
						size_t idx_begin_token = parse_context.idx_prev_token;
						if (!(member_field = make_ast_node<EnumItemNode>(resource_allocator.get(), resource_allocator.get(), document))) {
							return gen_out_of_memory_error();
						}

						{
							peff::ScopeGuard set_enum_field_token_range_guard([this, token, member_field, idx_begin_token]() noexcept {
								member_field->token_range = TokenRange{ idx_begin_token, parse_context.idx_prev_token };
							});

							Token *field_name_token;

							if ((syntax_error = expect_token((field_name_token = peek_token()), TokenId::Id)))
								return syntax_error;

							next_token();

							if (!member_field->name.build(field_name_token->source_text))
								return gen_out_of_memory_error();

							Token *assign_token;

							if ((assign_token = peek_token())->token_id == TokenId::AssignOp) {
								next_token();

								if ((syntax_error = parse_expr(0, member_field->value)))
									return syntax_error;
							}
						}

						size_t idx_field_member;

						if ((idx_field_member = enum_node->push_member(member_field.cast_to<MemberNode>())) == SIZE_MAX) {
							return gen_out_of_memory_error();
						}

						if (auto it = enum_node->member_indices.find(member_field->name); it != enum_node->member_indices.end()) {
							peff::String s(resource_allocator.get());

							if (!s.build(member_field->name)) {
								return gen_out_of_memory_error();
							}

							ConflictingDefinitionsErrorExData ex_data(std::move(s));

							return SyntaxError(member_field->token_range, std::move(ex_data));
						} else {
							if (!(enum_node->index_member(idx_field_member))) {
								return gen_out_of_memory_error();
							}
						}

						Token *comma_token;

						if ((comma_token = peek_token())->token_id != TokenId::Comma) {
							break;
						}

						next_token();
					}

					Token *r_brace_token;

					if ((syntax_error = expect_token((r_brace_token = peek_token()), TokenId::RBrace))) {
						return syntax_error;
					}

					next_token();
				}
			}

			if (auto it = p->member_indices.find(enum_node->name); it != p->member_indices.end()) {
				peff::String s(resource_allocator.get());

				if (!s.build(enum_node->name)) {
					return gen_out_of_memory_error();
				}

				ConflictingDefinitionsErrorExData ex_data(std::move(s));

				return SyntaxError(enum_node->token_range, std::move(ex_data));
			} else {
				if (!(p->index_member(idx_member))) {
					return gen_out_of_memory_error();
				}
			}

			break;
		}
		case TokenId::ImportKeyword: {
			// Import item.
			next_token();

			AstNodePtr<ImportNode> import_node;

			if (!(import_node = make_ast_node<ImportNode>(resource_allocator.get(), resource_allocator.get(), document))) {
				return gen_out_of_memory_error();
			}

			if ((syntax_error = parse_id_ref(import_node->id_ref)))
				return syntax_error;

			size_t idx_member;
			if ((idx_member = p->push_member(import_node.cast_to<MemberNode>())) == SIZE_MAX) {
				return gen_out_of_memory_error();
			}

			if (Token *as_token = peek_token(); as_token->token_id == TokenId::AsKeyword) {
				next_token();

				Token *name_token;

				if ((syntax_error = expect_token((name_token = peek_token()), TokenId::Id))) {
					return syntax_error;
				}

				if (!import_node->name.build(name_token->source_text)) {
					return gen_out_of_memory_error();
				}

				if (!p->index_member(idx_member)) {
					return gen_out_of_memory_error();
				}
			} else {
				if (!p->anonymous_imports.push_back(AstNodePtr<ImportNode>(import_node))) {
					return gen_out_of_memory_error();
				}
			}

			Token *semicolon_token;

			if ((syntax_error = expect_token((semicolon_token = peek_token()), TokenId::Semicolon))) {
				return syntax_error;
			}

			next_token();

			break;
		}
		default:
			next_token();
			return SyntaxError(
				TokenRange{ token->index },
				SyntaxErrorKind::ExpectingDecl);
	}

	return {};
}

INTERBUFC_API std::optional<SyntaxError> Parser::parse_program(const AstNodePtr<ModuleNode> &initial_mod) {
	std::optional<SyntaxError> syntax_error;

	Token *t;

	cur_parent = initial_mod.cast_to<MemberNode>();

	if ((t = peek_token())->token_id == TokenId::ModuleKeyword) {
		next_token();

		IdRefPtr module_name;

		if ((syntax_error = parse_id_ref(module_name))) {
			if (!syntax_errors.push_back(std::move(syntax_error.value())))
				return gen_out_of_memory_error();
			syntax_error.reset();
		}

		Token *semicolon_token;
		if ((syntax_error = expect_token((semicolon_token = peek_token()), TokenId::Semicolon))) {
			return syntax_error;
		}
		next_token();

		initial_mod->namespace_path = std::move(module_name);
	}

	while ((t = peek_token())->token_id != TokenId::End) {
		if ((syntax_error = parse_program_stmt())) {
			// Parse the rest to make sure that we have gained all of the information,
			// instead of ignoring them.
			if (!syntax_errors.push_back(std::move(syntax_error.value())))
				return gen_out_of_memory_error();
			syntax_error.reset();
		}
	}

	initial_mod->set_parser(shared_from_this());

	return {};
}
