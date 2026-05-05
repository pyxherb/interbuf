#include "../parser.h"
#include <climits>

using namespace interbufc;

INTERBUFC_API std::optional<SyntaxError> Parser::parse_type_name(AstNodePtr<TypeNameNode> &type_name_out, bool with_circumfixes) {
	std::optional<SyntaxError> syntax_error;
	Token *t = peek_token();

	switch (t->token_id) {
		case TokenId::I8TypeName:
			if (!(type_name_out = make_ast_node<I8TypeNameNode>(
					  resource_allocator.get(),
					  resource_allocator.get(), document)
						.cast_to<TypeNameNode>()))
				return gen_out_of_memory_error();
			type_name_out->token_range = TokenRange{ t->index };
			next_token();
			break;
		case TokenId::I16TypeName:
			if (!(type_name_out = make_ast_node<I16TypeNameNode>(
					  resource_allocator.get(),
					  resource_allocator.get(), document)
						.cast_to<TypeNameNode>()))
				return gen_out_of_memory_error();
			type_name_out->token_range = TokenRange{ t->index };
			next_token();
			break;
		case TokenId::I32TypeName:
			if (!(type_name_out = make_ast_node<I32TypeNameNode>(
					  resource_allocator.get(),
					  resource_allocator.get(), document)
						.cast_to<TypeNameNode>()))
				return gen_out_of_memory_error();
			type_name_out->token_range = TokenRange{ t->index };
			next_token();
			break;
		case TokenId::I64TypeName:
			if (!(type_name_out = make_ast_node<I64TypeNameNode>(
					  resource_allocator.get(),
					  resource_allocator.get(), document)
						.cast_to<TypeNameNode>()))
				return gen_out_of_memory_error();
			type_name_out->token_range = TokenRange{ t->index };
			next_token();
			break;
		case TokenId::U8TypeName:
			if (!(type_name_out = make_ast_node<U8TypeNameNode>(
					  resource_allocator.get(),
					  resource_allocator.get(), document)
						.cast_to<TypeNameNode>()))
				return gen_out_of_memory_error();
			type_name_out->token_range = TokenRange{ t->index };
			next_token();
			break;
		case TokenId::U16TypeName:
			if (!(type_name_out = make_ast_node<U16TypeNameNode>(
					  resource_allocator.get(),
					  resource_allocator.get(), document)
						.cast_to<TypeNameNode>()))
				return gen_out_of_memory_error();
			type_name_out->token_range = TokenRange{ t->index };
			next_token();
			break;
		case TokenId::U32TypeName:
			if (!(type_name_out = make_ast_node<U32TypeNameNode>(
					  resource_allocator.get(),
					  resource_allocator.get(), document)
						.cast_to<TypeNameNode>()))
				return gen_out_of_memory_error();
			type_name_out->token_range = TokenRange{ t->index };
			next_token();
			break;
		case TokenId::U64TypeName:
			if (!(type_name_out = make_ast_node<U64TypeNameNode>(
					  resource_allocator.get(),
					  resource_allocator.get(), document)
						.cast_to<TypeNameNode>()))
				return gen_out_of_memory_error();
			type_name_out->token_range = TokenRange{ t->index };
			next_token();
			break;
		case TokenId::F32TypeName:
			if (!(type_name_out = make_ast_node<F32TypeNameNode>(
					  resource_allocator.get(),
					  resource_allocator.get(), document)
						.cast_to<TypeNameNode>()))
				return gen_out_of_memory_error();
			type_name_out->token_range = TokenRange{ t->index };
			next_token();
			break;
		case TokenId::F64TypeName:
			if (!(type_name_out = make_ast_node<F64TypeNameNode>(
					  resource_allocator.get(),
					  resource_allocator.get(), document)
						.cast_to<TypeNameNode>()))
				return gen_out_of_memory_error();
			type_name_out->token_range = TokenRange{ t->index };
			next_token();
			break;
		case TokenId::ObjectTypeName:
			if (!(type_name_out = make_ast_node<ObjectTypeNameNode>(
					  resource_allocator.get(),
					  resource_allocator.get(), document)
						.cast_to<TypeNameNode>()))
				return gen_out_of_memory_error();
			type_name_out->token_range = TokenRange{ t->index };
			next_token();
			break;
		case TokenId::BoolTypeName:
			if (!(type_name_out = make_ast_node<BoolTypeNameNode>(
					  resource_allocator.get(),
					  resource_allocator.get(), document)
						.cast_to<TypeNameNode>()))
				return gen_out_of_memory_error();
			type_name_out->token_range = TokenRange{ t->index };
			next_token();
			break;
		case TokenId::StringTypeName:
			if (!(type_name_out = make_ast_node<StringTypeNameNode>(
					  resource_allocator.get(),
					  resource_allocator.get(), document)
						.cast_to<TypeNameNode>()))
				return gen_out_of_memory_error();
			type_name_out->token_range = TokenRange{ t->index };
			next_token();
			break;
		case TokenId::Id: {
			IdRefPtr id;
			if ((syntax_error = parse_id_ref(id)))
				return syntax_error;

			AstNodePtr<CustomTypeNameNode> tn;

			if (!(tn = make_ast_node<CustomTypeNameNode>(
					  resource_allocator.get(),
					  resource_allocator.get(),
					  document)))
				return gen_out_of_memory_error();

			tn->context_node = cur_parent;

			tn->token_range = id->token_range;
			tn->id_ref_ptr = std::move(id);

			type_name_out = tn.cast_to<TypeNameNode>();

			break;
		}
		default:
			return SyntaxError(TokenRange{ t->index }, SyntaxErrorKind::UnexpectedToken);
	}

	if (with_circumfixes) {
		while (true) {
			switch ((t = peek_token())->token_id) {
				case TokenId::LBracket: {
					next_token();

					Token *r_bracket_token;
					if ((syntax_error = expect_token((r_bracket_token = peek_token()), TokenId::RBracket)))
						return SyntaxError(TokenRange{ r_bracket_token->index }, ExpectingSingleTokenErrorExData{ TokenId::RBracket });

					next_token();

					if (!(type_name_out = make_ast_node<ArrayTypeNameNode>(
							  resource_allocator.get(),
							  resource_allocator.get(),
							  document,
							  type_name_out)
								.cast_to<TypeNameNode>()))
						return gen_out_of_memory_error();
					break;
				}
				default:
					goto end;
			}
		}
	}

end:
	return {};
}
