#include "../parser.h"

using namespace interbufc;

INTERBUFC_API std::optional<SyntaxError> Parser::parse_expr(int precedence, AstNodePtr<ExprNode> &expr_out) {
	Token *prefix_token;

	std::optional<SyntaxError> syntax_error;
	AstNodePtr<ExprNode> lhs, rhs;

	if ((syntax_error = expect_token((prefix_token = peek_token()))))
		goto gen_bad_expr;

	{
		{
			peff::ScopeGuard set_token_range_guard([this, prefix_token, &lhs]() noexcept {
				if (lhs) {
					lhs->token_range = TokenRange{ prefix_token->index, parse_context.idx_prev_token };
				}
			});

			switch (prefix_token->token_id) {
				case TokenId::IntLiteral: {
					next_token();
					if (!(lhs = peff::make_shared_with_control_block<I32LiteralExprNode, AstNodeControlBlock<I32LiteralExprNode>>(
							  resource_allocator.get(), resource_allocator.get(), document,
							  ((IntTokenExtension *)prefix_token->ex_data.get())->data)
								.template cast_to<ExprNode>()))
						return gen_out_of_memory_error();
					break;
				}
				case TokenId::LongLiteral: {
					next_token();
					if (!(lhs = peff::make_shared_with_control_block<I64LiteralExprNode, AstNodeControlBlock<I64LiteralExprNode>>(
							  resource_allocator.get(), resource_allocator.get(), document,
							  ((LongTokenExtension *)prefix_token->ex_data.get())->data)
								.template cast_to<ExprNode>()))
						return gen_out_of_memory_error();
					break;
				}
				case TokenId::UIntLiteral: {
					next_token();
					if (!(lhs = peff::make_shared_with_control_block<U32LiteralExprNode, AstNodeControlBlock<U32LiteralExprNode>>(
							  resource_allocator.get(), resource_allocator.get(), document,
							  ((UIntTokenExtension *)prefix_token->ex_data.get())->data)
								.template cast_to<ExprNode>()))
						return gen_out_of_memory_error();
					break;
				}
				case TokenId::ULongLiteral: {
					next_token();
					if (!(lhs = peff::make_shared_with_control_block<U64LiteralExprNode, AstNodeControlBlock<U64LiteralExprNode>>(
							  resource_allocator.get(), resource_allocator.get(), document,
							  ((ULongTokenExtension *)prefix_token->ex_data.get())->data)
								.template cast_to<ExprNode>()))
						return gen_out_of_memory_error();
					break;
				}
				case TokenId::StringLiteral: {
					next_token();
					peff::String s(resource_allocator.get());

					if (!s.build(((StringTokenExtension *)prefix_token->ex_data.get())->data)) {
						return gen_out_of_memory_error();
					}

					if (!(lhs = make_ast_node<StringLiteralExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document,
							  std::move(s))
								.template cast_to<ExprNode>()))
						return gen_out_of_memory_error();
					break;
				}
				case TokenId::F32Literal: {
					next_token();
					if (!(lhs = peff::make_shared_with_control_block<F32LiteralExprNode, AstNodeControlBlock<F32LiteralExprNode>>(
							  resource_allocator.get(), resource_allocator.get(), document,
							  ((F32TokenExtension *)prefix_token->ex_data.get())->data)
								.template cast_to<ExprNode>()))
						return gen_out_of_memory_error();
					break;
				}
				case TokenId::F64Literal: {
					next_token();
					if (!(lhs = peff::make_shared_with_control_block<F64LiteralExprNode, AstNodeControlBlock<F64LiteralExprNode>>(
							  resource_allocator.get(), resource_allocator.get(), document,
							  ((F64TokenExtension *)prefix_token->ex_data.get())->data)
								.template cast_to<ExprNode>()))
						return gen_out_of_memory_error();
					break;
				}
				case TokenId::TrueKeyword: {
					next_token();
					if (!(lhs = make_ast_node<BoolLiteralExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document,
							  true)
								.template cast_to<ExprNode>()))
						return gen_out_of_memory_error();
					break;
				}
				case TokenId::FalseKeyword: {
					next_token();
					if (!(lhs = make_ast_node<BoolLiteralExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document,
							  false)
								.template cast_to<ExprNode>()))
						return gen_out_of_memory_error();
					break;
				}
				default:
					next_token();
					return SyntaxError(
						TokenRange{ prefix_token->index },
						SyntaxErrorKind::ExpectingExpr);
			}
		}

		Token *infix_token;

		/* for (;;) {
			peff::ScopeGuard set_token_range_guard([this, prefix_token, &lhs]() noexcept {
				if (lhs) {
					lhs->token_range = TokenRange{ prefix_token->index, parse_context.idx_prev_token };
				}
			});

			switch ((infix_token = peek_token())->token_id) {
				case TokenId::LParenthese: {
					if (precedence > 140)
						goto end;
					next_token();

					AstNodePtr<CallExprNode> expr;

					if (!(expr = make_ast_node<CallExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document, AstNodePtr<ExprNode>(), peff::DynArray<AstNodePtr<ExprNode>>{ resource_allocator.get() })))
						return gen_out_of_memory_error();

					expr->target = lhs;

					lhs = expr.template cast_to<ExprNode>();

					expr->l_parenthese_token_index = infix_token->index;

					if ((syntax_error = parse_args(expr->args, expr->idx_comma_tokens))) {
						goto gen_bad_expr;
					}

					Token *r_parenthese_token;

					if ((syntax_error = expect_token((r_parenthese_token = peek_token()), TokenId::RParenthese)))
						goto gen_bad_expr;

					next_token();

					expr->r_parenthese_token_index = r_parenthese_token->index;

					if (peek_token()->token_id == TokenId::WithKeyword) {
						next_token();

						if (auto e = parse_expr(121, expr->with_object); e)
							return e;
					}

					break;
				}
				case TokenId::LBracket: {
					if (precedence > 140)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::Subscript;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(0, expr->rhs)))
						goto gen_bad_expr;

					if ((syntax_error = split_rd_brackets_token()))
						goto gen_bad_expr;

					Token *r_bracket_token;

					if ((syntax_error = expect_token((r_bracket_token = next_token()), TokenId::RBracket)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::Dot: {
					if (precedence > 140)
						goto end;
					next_token();

					AstNodePtr<HeadedIdRefExprNode> expr;

					if (!(expr = make_ast_node<HeadedIdRefExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document, lhs, IdRefPtr{})))
						return gen_out_of_memory_error();

					expr->head = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_id_ref(expr->id_ref_ptr)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::AsKeyword: {
					if (precedence > 130)
						goto end;
					next_token();

					AstNodePtr<CastExprNode> expr;

					if (!(expr = make_ast_node<CastExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->source = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_type_name(expr->target_type)))
						goto gen_bad_expr;

					expr->token_range.end_index = expr->target_type->token_range.end_index;

					break;
				}

				case TokenId::MulOp: {
					if (precedence > 120)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::Mul;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(121, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::DivOp: {
					if (precedence > 120)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::Div;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(121, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::ModOp: {
					if (precedence > 120)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::Mod;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(121, expr->rhs)))
						goto gen_bad_expr;

					break;
				}

				case TokenId::AddOp: {
					if (precedence > 110)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::Add;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(111, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::SubOp: {
					if (precedence > 110)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::Sub;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(111, expr->rhs)))
						goto gen_bad_expr;

					break;
				}

				case TokenId::LshOp: {
					if (precedence > 100)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::Shl;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(101, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::RshOp: {
					if (precedence > 100)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::Shr;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(101, expr->rhs)))
						goto gen_bad_expr;

					break;
				}

				case TokenId::CmpOp: {
					if (precedence > 90)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::Cmp;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(91, expr->rhs)))
						goto gen_bad_expr;

					break;
				}

				case TokenId::GtOp: {
					if (precedence > 80)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::Gt;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(81, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::GtEqOp: {
					if (precedence > 80)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::GtEq;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(81, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::LtOp: {
					if (precedence > 80)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::Lt;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(81, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::LtEqOp: {
					if (precedence > 80)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::LtEq;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(81, expr->rhs)))
						goto gen_bad_expr;

					break;
				}

				case TokenId::EqOp: {
					if (precedence > 70)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::Eq;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(71, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::NeqOp: {
					if (precedence > 70)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::Neq;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(71, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::StrictEqOp: {
					if (precedence > 70)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::StrictEq;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(71, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::StrictNeqOp: {
					if (precedence > 70)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::StrictNeq;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(71, expr->rhs)))
						goto gen_bad_expr;

					break;
				}

				case TokenId::AndOp: {
					if (precedence > 60)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::And;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(61, expr->rhs)))
						goto gen_bad_expr;

					break;
				}

				case TokenId::XorOp: {
					if (precedence > 50)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::Xor;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(51, expr->rhs)))
						goto gen_bad_expr;

					break;
				}

				case TokenId::OrOp: {
					if (precedence > 40)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::Or;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(41, expr->rhs)))
						goto gen_bad_expr;

					break;
				}

				case TokenId::LAndOp: {
					if (precedence > 30)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::LAnd;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(31, expr->rhs)))
						goto gen_bad_expr;

					break;
				}

				case TokenId::LOrOp: {
					if (precedence > 20)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::LOr;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(21, expr->rhs)))
						goto gen_bad_expr;

					break;
				}

				case TokenId::Question: {
					if (precedence > 10)
						goto end;
					next_token();

					AstNodePtr<TernaryExprNode> expr;

					if (!(expr = make_ast_node<TernaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(10, expr->lhs)))
						goto gen_bad_expr;

					expr->token_range.end_index = expr->lhs->token_range.end_index;

					Token *colon_token;
					if ((syntax_error = expect_token((colon_token = next_token()), TokenId::Colon)))
						goto gen_bad_expr;

					expr->token_range.end_index = colon_token->index;

					if ((syntax_error = parse_expr(10, expr->rhs)))
						goto gen_bad_expr;

					break;
				}

				case TokenId::AssignOp: {
					if (precedence > 1)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::Assign;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(0, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::AddAssignOp: {
					if (precedence > 1)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::AddAssign;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(0, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::SubAssignOp: {
					if (precedence > 1)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::SubAssign;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(0, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::MulAssignOp: {
					if (precedence > 1)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::MulAssign;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(0, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::DivAssignOp: {
					if (precedence > 1)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::DivAssign;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(0, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::AndAssignOp: {
					if (precedence > 1)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::AndAssign;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(0, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::OrAssignOp: {
					if (precedence > 1)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::OrAssign;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(0, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::XorAssignOp: {
					if (precedence > 1)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::XorAssign;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(0, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::LshAssignOp: {
					if (precedence > 1)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::ShlAssign;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(0, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::RshAssignOp: {
					if (precedence > 1)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::ShrAssign;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(0, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				case TokenId::Comma: {
					if (precedence > -9)
						goto end;
					next_token();

					AstNodePtr<BinaryExprNode> expr;

					if (!(expr = make_ast_node<BinaryExprNode>(
							  resource_allocator.get(), resource_allocator.get(), document)))
						return gen_out_of_memory_error();

					expr->binary_op = BinaryOp::Comma;
					expr->lhs = lhs;

					lhs = expr.template cast_to<ExprNode>();

					if ((syntax_error = parse_expr(-10, expr->rhs)))
						goto gen_bad_expr;

					break;
				}
				default:
					goto end;
			}
		}*/
	}

end:
	expr_out = lhs;

	return {};

gen_bad_expr:
	if (!(expr_out = make_ast_node<BadExprNode>(resource_allocator.get(), resource_allocator.get(), document, lhs).template cast_to<ExprNode>()))
		return gen_out_of_memory_error();
	expr_out->token_range = { prefix_token->index, parse_context.idx_current_token };
	return syntax_error;
}
