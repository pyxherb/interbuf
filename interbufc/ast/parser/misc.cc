#include "../parser.h"

using namespace interbufc;

INTERBUFC_API std::optional<SyntaxError> Parser::lookahead_until(size_t n_token_ids, const TokenId token_ids[]) {
	// stub.
	return {};

	Token *token;
	while ((token->token_id != TokenId::End)) {
		for(size_t i = 0 ; i < n_token_ids; ++i) {
			if(token->token_id == token_ids[i]) {
				return {};
			}
		}
		token = next_token(true, true, true);
	}

	NoMatchingTokensFoundErrorExData ex_data(resource_allocator.get());

	for(size_t i = 0 ; i < n_token_ids; ++i) {
		TokenId copied_token_id = token_ids[i];
		if(!ex_data.expecting_token_ids.insert(std::move(copied_token_id)))
			return gen_out_of_memory_error();
	}

	return SyntaxError(token->index, std::move(ex_data));
}

INTERBUFC_API Token *Parser::next_token(bool keep_new_line, bool keep_whitespace, bool keep_comment) {
	size_t &i = parse_context.idx_current_token;

	while (i < token_list.size()) {
		Token *current_token = token_list.at(i).get();
		current_token->index = i;

		switch (token_list.at(i)->token_id) {
			case TokenId::NewLine:
				if (keep_new_line) {
					parse_context.idx_prev_token = parse_context.idx_current_token;
					++i;
					return current_token;
				}
				break;
			case TokenId::Whitespace:
				if (keep_whitespace) {
					parse_context.idx_prev_token = parse_context.idx_current_token;
					++i;
					return current_token;
				}
				break;
			case TokenId::LineComment:
			case TokenId::BlockComment:
			case TokenId::DocumentationComment:
				if (keep_comment) {
					parse_context.idx_prev_token = parse_context.idx_current_token;
					++i;
					return current_token;
				}
				break;
			default:
				assert(is_valid_token(current_token->token_id));
				parse_context.idx_prev_token = parse_context.idx_current_token;
				++i;
				return current_token;
		}

		++i;
	}

	return token_list.back().get();
}

INTERBUFC_API Token *Parser::peek_token(bool keep_new_line, bool keep_whitespace, bool keep_comment) {
	size_t i = parse_context.idx_current_token;

	while (i < token_list.size()) {
		Token *current_token = token_list.at(i).get();
		current_token->index = i;

		switch (current_token->token_id) {
			case TokenId::NewLine:
				if (keep_new_line)
					return current_token;
				break;
			case TokenId::Whitespace:
				if (keep_whitespace)
					return current_token;
				break;
			case TokenId::LineComment:
			case TokenId::BlockComment:
			case TokenId::DocumentationComment:
				if (keep_comment)
					return current_token;
				break;
			default:
				assert(is_valid_token(current_token->token_id));
				return current_token;
		}

		++i;
	}

	return token_list.back().get();
}
