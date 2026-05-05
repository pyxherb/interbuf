#include <interbufc/ast/lexer.h>
#include <algorithm>

using namespace interbufc;

enum LexCondition {
	yycInitialCondition = 0,

	yycStringCondition,
	yycEscapeCondition,

	yycCommentCondition,
	yycLineCommentCondition,
};

INTERBUFC_API std::optional<LexicalError> Lexer::lex(const std::string_view &src, peff::Alloc *allocator, const AstNodePtr<Document> &document) {
	const char *YYCURSOR = src.data(), *YYMARKER = YYCURSOR, *YYLIMIT = src.data() + src.size();
	const char *prev_yycursor = YYCURSOR;

	LexCondition YYCONDITION = yycInitialCondition;

#define YYSETCONDITION(cond) (YYCONDITION = (yyc##cond))
#define YYGETCONDITION() (YYCONDITION)

	OwnedTokenPtr token;

	while (true) {
		peff::String str_literal(allocator);

		if (!(token = OwnedTokenPtr(peff::alloc_and_construct<Token>(allocator, sizeof(std::max_align_t), allocator, peff::WeakPtr<Document>(document)))))
			goto out_of_memory;

		while (true) {
			/*!re2c
				re2c:yyfill:enable = 0;
				re2c:define:YYCTYPE = char;
				re2c:eof = 1;

				<InitialCondition>"///"		{ YYSETCONDITION(LineCommentCondition); token->token_id = TokenId::DocumentationComment; continue; }
				<InitialCondition>"//"		{ YYSETCONDITION(LineCommentCondition); token->token_id = TokenId::LineComment; continue; }
				<InitialCondition>"/*"		{ YYSETCONDITION(CommentCondition); token->token_id = TokenId::BlockComment; continue; }

				<InitialCondition>"->"		{ token->token_id = TokenId::ReturnTypeOp; break; }
				<InitialCondition>"::"		{ token->token_id = TokenId::ScopeOp; break; }
				<InitialCondition>"=>"		{ token->token_id = TokenId::MatchOp; break; }
				<InitialCondition>"&&"		{ token->token_id = TokenId::LAndOp; break; }
				<InitialCondition>"||"		{ token->token_id = TokenId::LOrOp; break; }
				<InitialCondition>"+"		{ token->token_id = TokenId::AddOp; break; }
				<InitialCondition>"-"		{ token->token_id = TokenId::SubOp; break; }
				<InitialCondition>"*"		{ token->token_id = TokenId::MulOp; break; }
				<InitialCondition>"/"		{ token->token_id = TokenId::DivOp; break; }
				<InitialCondition>"%"		{ token->token_id = TokenId::ModOp; break; }
				<InitialCondition>"&"		{ token->token_id = TokenId::AndOp; break; }
				<InitialCondition>"|"		{ token->token_id = TokenId::OrOp; break; }
				<InitialCondition>"^"		{ token->token_id = TokenId::XorOp; break; }
				<InitialCondition>"!"		{ token->token_id = TokenId::LNotOp; break; }
				<InitialCondition>"~"		{ token->token_id = TokenId::NotOp; break; }
				<InitialCondition>"="		{ token->token_id = TokenId::AssignOp; break; }
				<InitialCondition>"+="		{ token->token_id = TokenId::AddAssignOp; break; }
				<InitialCondition>"-="		{ token->token_id = TokenId::SubAssignOp; break; }
				<InitialCondition>"*="		{ token->token_id = TokenId::MulAssignOp; break; }
				<InitialCondition>"/="		{ token->token_id = TokenId::DivAssignOp; break; }
				<InitialCondition>"%="		{ token->token_id = TokenId::ModAssignOp; break; }
				<InitialCondition>"&="		{ token->token_id = TokenId::AndAssignOp; break; }
				<InitialCondition>"|="		{ token->token_id = TokenId::OrAssignOp; break; }
				<InitialCondition>"^="		{ token->token_id = TokenId::XorAssignOp; break; }
				<InitialCondition>"<<="		{ token->token_id = TokenId::LshAssignOp; break; }
				<InitialCondition>">>="		{ token->token_id = TokenId::RshAssignOp; break; }
				<InitialCondition>"==="		{ token->token_id = TokenId::StrictEqOp; break; }
				<InitialCondition>"!=="		{ token->token_id = TokenId::StrictNeqOp; break; }
				<InitialCondition>"=="		{ token->token_id = TokenId::EqOp; break; }
				<InitialCondition>"!="		{ token->token_id = TokenId::NeqOp; break; }
				<InitialCondition>"<<"		{ token->token_id = TokenId::LshOp; break; }
				<InitialCondition>">>"		{ token->token_id = TokenId::RshOp; break; }
				<InitialCondition>"<=>"		{ token->token_id = TokenId::CmpOp; break; }
				<InitialCondition>"<="		{ token->token_id = TokenId::LtEqOp; break; }
				<InitialCondition>">="		{ token->token_id = TokenId::GtEqOp; break; }
				<InitialCondition>"<"		{ token->token_id = TokenId::LtOp; break; }
				<InitialCondition>">"		{ token->token_id = TokenId::GtOp; break; }
				<InitialCondition>"$"		{ token->token_id = TokenId::DollarOp; break; }
				<InitialCondition>"@"		{ token->token_id = TokenId::At; break; }

				<InitialCondition>"as"			{ token->token_id = TokenId::AsKeyword; break; }
				<InitialCondition>"class"		{ token->token_id = TokenId::ClassKeyword; break; }
				<InitialCondition>"enum"		{ token->token_id = TokenId::EnumKeyword; break; }
				<InitialCondition>"import"		{ token->token_id = TokenId::ImportKeyword; break; }
				<InitialCondition>"module"		{ token->token_id = TokenId::ModuleKeyword; break; }
				<InitialCondition>"struct"		{ token->token_id = TokenId::StructKeyword; break; }

				<InitialCondition>"i8"			{ token->token_id = TokenId::I8TypeName; break; }
				<InitialCondition>"i16"			{ token->token_id = TokenId::I16TypeName; break; }
				<InitialCondition>"i32"			{ token->token_id = TokenId::I32TypeName; break; }
				<InitialCondition>"i64"			{ token->token_id = TokenId::I64TypeName; break; }
				<InitialCondition>"u8"			{ token->token_id = TokenId::U8TypeName; break; }
				<InitialCondition>"u16"			{ token->token_id = TokenId::U16TypeName; break; }
				<InitialCondition>"u32"			{ token->token_id = TokenId::U32TypeName; break; }
				<InitialCondition>"u64"			{ token->token_id = TokenId::U64TypeName; break; }
				<InitialCondition>"f32"			{ token->token_id = TokenId::F32TypeName; break; }
				<InitialCondition>"f64"			{ token->token_id = TokenId::F64TypeName; break; }
				<InitialCondition>"bool"		{ token->token_id = TokenId::BoolTypeName; break; }
				<InitialCondition>"void"		{ token->token_id = TokenId::VoidTypeName; break; }
				<InitialCondition>"string"		{ token->token_id = TokenId::StringTypeName; break; }

				<InitialCondition>","		{ token->token_id = TokenId::Comma; break; }
				<InitialCondition>"?"		{ token->token_id = TokenId::Question; break; }
				<InitialCondition>":"		{ token->token_id = TokenId::Colon; break; }
				<InitialCondition>";"     	{ token->token_id = TokenId::Semicolon; break; }
				<InitialCondition>"[["		{ token->token_id = TokenId::LDBracket; break; }
				<InitialCondition>"]]"		{ token->token_id = TokenId::RDBracket; break; }
				<InitialCondition>"["		{ token->token_id = TokenId::LBracket; break; }
				<InitialCondition>"]"		{ token->token_id = TokenId::RBracket; break; }
				<InitialCondition>"{"		{ token->token_id = TokenId::LBrace; break; }
				<InitialCondition>"}"		{ token->token_id = TokenId::RBrace; break; }
				<InitialCondition>"("		{ token->token_id = TokenId::LParenthese; break; }
				<InitialCondition>")"		{ token->token_id = TokenId::RParenthese; break; }
				<InitialCondition>"..."		{ token->token_id = TokenId::VarArg; break; }
				<InitialCondition>"."		{ token->token_id = TokenId::Dot; break; }

				<InitialCondition>[a-zA-Z_][a-zA-Z0-9_]* {
					token->token_id = TokenId::Id;
					break;
				}

				<InitialCondition>"0"[0-7]+ {
					token->token_id = TokenId::UIntLiteral;
					token->ex_data = std::unique_ptr<TokenExtension, peff::DeallocableDeleter<TokenExtension>>(
						peff::alloc_and_construct<UIntTokenExtension>(allocator, sizeof(std::max_align_t), allocator, strtoul(prev_yycursor, nullptr, 8)));
					break;
				}

				<InitialCondition>[0-9]+ {
					token->token_id = TokenId::IntLiteral;
					token->ex_data = std::unique_ptr<TokenExtension, peff::DeallocableDeleter<TokenExtension>>(
						peff::alloc_and_construct<IntTokenExtension>(allocator, sizeof(std::max_align_t), allocator, strtol(prev_yycursor, nullptr, 10)));
					break;
				}

				<InitialCondition>"0"[xX][0-9a-fA-F]+ {
					token->token_id = TokenId::UIntLiteral;
					token->ex_data = std::unique_ptr<TokenExtension, peff::DeallocableDeleter<TokenExtension>>(
						peff::alloc_and_construct<UIntTokenExtension>(allocator, sizeof(std::max_align_t), allocator, strtoul(prev_yycursor, nullptr, 16)));
					break;
				}

				<InitialCondition>"0"[bB][01]+ {
					token->token_id = TokenId::UIntLiteral;
					token->ex_data = std::unique_ptr<TokenExtension, peff::DeallocableDeleter<TokenExtension>>(
						peff::alloc_and_construct<UIntTokenExtension>(allocator, sizeof(std::max_align_t), allocator, strtoul(prev_yycursor, nullptr, 2)));
					break;
				}

				<InitialCondition>[0-9]+"."[0-9]+[f_f] {
					token->token_id = TokenId::F32Literal;
					token->ex_data = std::unique_ptr<TokenExtension, peff::DeallocableDeleter<TokenExtension>>(
						peff::alloc_and_construct<F32TokenExtension>(allocator, sizeof(std::max_align_t), allocator, strtof(prev_yycursor, nullptr)));
					break;
				}

				<InitialCondition>[0-9]+"."[0-9]+ {
					token->token_id = TokenId::F64Literal;
					token->ex_data = std::unique_ptr<TokenExtension, peff::DeallocableDeleter<TokenExtension>>(
						peff::alloc_and_construct<F64TokenExtension>(allocator, sizeof(std::max_align_t), allocator, strtod(prev_yycursor, nullptr)));
					break;
				}

				<InitialCondition>"\""		{ YYSETCONDITION(StringCondition); continue; }

				<InitialCondition>"\n"		{ token->token_id = TokenId::NewLine; break; }
				<InitialCondition>$			{ goto end; }

				<InitialCondition>[ \r\t]+	{ token->token_id = TokenId::Whitespace; break; }

				<InitialCondition>[^]		{
					size_t begin_index = prev_yycursor - src.data(), end_index = YYCURSOR - src.data();
					std::string_view str_to_begin = src.substr(0, begin_index), str_to_end = src.substr(0, end_index);

					size_t prev_yycursor_index = prev_yycursor - src.data();
					auto prev_yycursor_pos = src.find_last_of('\n', prev_yycursor_index);
					if(prev_yycursor_pos == std::string_view::npos)
						prev_yycursor_pos = 0;
					prev_yycursor_pos = prev_yycursor_index - prev_yycursor_pos;

					size_t YYCURSORIndex = YYCURSOR - src.data();
					auto YYCURSORPos = src.find_last_of('\n', YYCURSORIndex);
					if(YYCURSORPos == std::string_view::npos)
						YYCURSORPos = 0;
					YYCURSORPos = YYCURSORIndex - YYCURSORPos;

					return LexicalError {
						SourceLocation {
						{ (size_t)std::count(str_to_begin.begin(), str_to_begin.end(), '\n'), prev_yycursor_pos },
						{ (size_t)std::count(str_to_end.begin(), str_to_end.end(), '\n'), YYCURSORPos }
					}, LexicalErrorKind::UnrecognizedToken};
				}

				<StringCondition>"\""		{
					YYSETCONDITION(InitialCondition);
					token->token_id = TokenId::StringLiteral;
					token->ex_data = std::unique_ptr<TokenExtension, peff::DeallocableDeleter<TokenExtension>>(
						peff::alloc_and_construct<StringTokenExtension>(allocator, sizeof(std::max_align_t), allocator, std::move(str_literal)));
					break;
				}
				<StringCondition>"\\"		{ YYSETCONDITION(EscapeCondition); continue; }
				<StringCondition>"\n"		{
					size_t begin_index = prev_yycursor - src.data(), end_index = YYCURSOR - src.data();
					std::string_view str_to_begin = src.substr(0, begin_index), str_to_end = src.substr(0, end_index);

					size_t prev_yycursor_index = prev_yycursor - src.data();
					auto prev_yycursor_pos = src.find_last_of('\n', prev_yycursor_index);
					if(prev_yycursor_pos == std::string_view::npos)
						prev_yycursor_pos = 0;
					prev_yycursor_pos = prev_yycursor_index - prev_yycursor_pos;

					size_t YYCURSORIndex = YYCURSOR - src.data();
					auto YYCURSORPos = src.find_last_of('\n', YYCURSORIndex);
					if(YYCURSORPos == std::string_view::npos)
						YYCURSORPos = 0;
					YYCURSORPos = YYCURSORIndex - YYCURSORPos;

					return LexicalError {
						SourceLocation {
						{ (size_t)std::count(str_to_begin.begin(), str_to_begin.end(), '\n'), prev_yycursor_pos },
						{ (size_t)std::count(str_to_end.begin(), str_to_end.end(), '\n'), YYCURSORPos }
					}, LexicalErrorKind::UnexpectedEndOfLine};
				}
				<StringCondition>$	{
					size_t begin_index = prev_yycursor - src.data(), end_index = YYCURSOR - src.data();
					std::string_view str_to_begin = src.substr(0, begin_index), str_to_end = src.substr(0, end_index);

					size_t prev_yycursor_index = prev_yycursor - src.data();
					auto prev_yycursor_pos = src.find_last_of('\n', prev_yycursor_index);
					if(prev_yycursor_pos == std::string_view::npos)
						prev_yycursor_pos = 0;
					prev_yycursor_pos = prev_yycursor_index - prev_yycursor_pos;

					size_t YYCURSORIndex = YYCURSOR - src.data();
					auto YYCURSORPos = src.find_last_of('\n', YYCURSORIndex);
					if(YYCURSORPos == std::string_view::npos)
						YYCURSORPos = 0;
					YYCURSORPos = YYCURSORIndex - YYCURSORPos;

					return LexicalError {
						SourceLocation {
						{ (size_t)std::count(str_to_begin.begin(), str_to_begin.end(), '\n'), prev_yycursor_pos },
						{ (size_t)std::count(str_to_end.begin(), str_to_end.end(), '\n'), YYCURSORPos }
					}, LexicalErrorKind::PrematuredEndOfFile};
				}
				<StringCondition>[^]		{ if(!str_literal.push_back(+YYCURSOR[-1])) goto out_of_memory; continue; }

				<EscapeCondition>"'"	{ YYSETCONDITION(StringCondition); if(!str_literal.push_back('\'')) goto out_of_memory; continue; }
				<EscapeCondition>"\""	{ YYSETCONDITION(StringCondition); if(!str_literal.push_back('"')) goto out_of_memory; continue; }
				<EscapeCondition>"?"	{ YYSETCONDITION(StringCondition); if(!str_literal.push_back('?')) goto out_of_memory; continue; }
				<EscapeCondition>"\\"	{ YYSETCONDITION(StringCondition); if(!str_literal.push_back('\\')) goto out_of_memory; continue; }
				<EscapeCondition>"a"	{ YYSETCONDITION(StringCondition); if(!str_literal.push_back('\a')) goto out_of_memory; continue; }
				<EscapeCondition>"b"	{ YYSETCONDITION(StringCondition); if(!str_literal.push_back('\b')) goto out_of_memory; continue; }
				<EscapeCondition>"f"	{ YYSETCONDITION(StringCondition); if(!str_literal.push_back('\f')) goto out_of_memory; continue; }
				<EscapeCondition>"n"	{ YYSETCONDITION(StringCondition); if(!str_literal.push_back('\n')) goto out_of_memory; continue; }
				<EscapeCondition>"r"	{ YYSETCONDITION(StringCondition); if(!str_literal.push_back('\r')) goto out_of_memory; continue; }
				<EscapeCondition>"t"	{ YYSETCONDITION(StringCondition); if(!str_literal.push_back('\t')) goto out_of_memory; continue; }
				<EscapeCondition>"v"	{ YYSETCONDITION(StringCondition); if(!str_literal.push_back('\v')) goto out_of_memory; continue; }
				<EscapeCondition>[0-7]{1,3}	{
					YYSETCONDITION(StringCondition);

					size_t size = YYCURSOR - prev_yycursor;

					char c = 0;
					for(uint_fast8_t i = 0; i < size; ++i) {
						c *= 8;
						c += prev_yycursor[i] - '0';
					}

					if(!str_literal.push_back(+c))
						goto out_of_memory;
				}
				<EscapeCondition>[xX][0-9a-fA-F]{1,2}	{
					YYSETCONDITION(StringCondition);

					size_t size = YYCURSOR - prev_yycursor;

					char c = 0, j;

					for(uint_fast8_t i = 1; i < size; ++i) {
						c *= 16;

						j = prev_yycursor[i];
						if((j >= '0') && (j <= '9'))
							c += prev_yycursor[i] - '0';
						else if((j >= 'a') && (j <= 'f'))
							c += prev_yycursor[i] - 'a';
						else if((j >= 'A') && (j <= 'F'))
							c += prev_yycursor[i] - 'A';
					}

					if(!str_literal.push_back(+c))
						goto out_of_memory;
				}
				<EscapeCondition>$	{
					size_t begin_index = prev_yycursor - src.data(), end_index = YYCURSOR - src.data();
					std::string_view str_to_begin = src.substr(0, begin_index), str_to_end = src.substr(0, end_index);

					size_t prev_yycursor_index = prev_yycursor - src.data();
					auto prev_yycursor_pos = src.find_last_of('\n', prev_yycursor_index);
					if(prev_yycursor_pos == std::string_view::npos)
						prev_yycursor_pos = 0;
					prev_yycursor_pos = prev_yycursor_index - prev_yycursor_pos;

					size_t YYCURSORIndex = YYCURSOR - src.data();
					auto YYCURSORPos = src.find_last_of('\n', YYCURSORIndex);
					if(YYCURSORPos == std::string_view::npos)
						YYCURSORPos = 0;
					YYCURSORPos = YYCURSORIndex - YYCURSORPos;

					return LexicalError {
						SourceLocation {
						{ (size_t)std::count(str_to_begin.begin(), str_to_begin.end(), '\n'), prev_yycursor_pos },
						{ (size_t)std::count(str_to_end.begin(), str_to_end.end(), '\n'), YYCURSORPos }
					}, LexicalErrorKind::PrematuredEndOfFile};
				}

				<CommentCondition>"*"[/]	{ YYSETCONDITION(InitialCondition); break; }
				<CommentCondition>[^]		{ continue; }
				<CommentCondition>$	{
					size_t begin_index = prev_yycursor - src.data(), end_index = YYCURSOR - src.data();
					std::string_view str_to_begin = src.substr(0, begin_index), str_to_end = src.substr(0, end_index);

					size_t prev_yycursor_index = prev_yycursor - src.data();
					auto prev_yycursor_pos = src.find_last_of('\n', prev_yycursor_index);
					if(prev_yycursor_pos == std::string_view::npos)
						prev_yycursor_pos = 0;
					prev_yycursor_pos = prev_yycursor_index - prev_yycursor_pos;

					size_t YYCURSORIndex = YYCURSOR - src.data();
					auto YYCURSORPos = src.find_last_of('\n', YYCURSORIndex);
					if(YYCURSORPos == std::string_view::npos)
						YYCURSORPos = 0;
					YYCURSORPos = YYCURSORIndex - YYCURSORPos;

					return LexicalError {
						SourceLocation {
						{ (size_t)std::count(str_to_begin.begin(), str_to_begin.end(), '\n'), prev_yycursor_pos },
						{ (size_t)std::count(str_to_end.begin(), str_to_end.end(), '\n'), YYCURSORPos }
					}, LexicalErrorKind::PrematuredEndOfFile};
				}

				<LineCommentCondition>"\n"	{ YYSETCONDITION(InitialCondition); break; }
				<LineCommentCondition>$		{ YYSETCONDITION(InitialCondition); break; }
				<LineCommentCondition>[^]	{ continue; }
			*/
		}

		size_t begin_index = prev_yycursor - src.data(), end_index = YYCURSOR - src.data();

		std::string_view str_to_begin = src.substr(0, begin_index), str_to_end = src.substr(0, end_index);

		token->source_text = std::string_view(prev_yycursor, YYCURSOR - prev_yycursor);

		size_t idx_last_begin_newline = src.find_last_of('\n', begin_index),
			   idx_last_end_newline = src.find_last_of('\n', end_index);

		token->source_location.begin_position = {
			(size_t)std::count(str_to_begin.begin(), str_to_begin.end(), '\n'),
			(idx_last_begin_newline == std::string_view::npos
					? begin_index
					: begin_index - idx_last_begin_newline - 1)
		};
		token->source_location.end_position = {
			(size_t)std::count(str_to_end.begin(), str_to_end.end(), '\n'),
			(idx_last_end_newline == std::string_view::npos
					? end_index
					: end_index - idx_last_end_newline)
		};
		if (!token_list.push_back(std::move(token)))
			goto out_of_memory;

		prev_yycursor = YYCURSOR;
	}

end : {
	SourceLocation end_location = token->source_location;

	token = OwnedTokenPtr(peff::alloc_and_construct<Token>(allocator, sizeof(std::max_align_t), allocator, document));
	token->token_id = TokenId::End;
	token->source_location = end_location;

	if (!token_list.push_back(std::move(token)))
		goto out_of_memory;
}

	return {};

out_of_memory:
	return LexicalError{ SourceLocation{ { 0, 0 }, { 0, 0 } }, LexicalErrorKind::OutOfMemory };
}
