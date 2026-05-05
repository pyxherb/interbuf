#include "comp/comp.h"
#include <initializer_list>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

struct OptionMatchContext {
	const int argc;
	char **const argv;
	int i;
	void *user_data;
};

struct SingleArgOption;

typedef int (*ArglessOptionCallback)(const OptionMatchContext &match_context, const char *option);
typedef int (*SingleArgOptionCallback)(const OptionMatchContext &match_context, const char *option, const char *arg);
typedef int (*CustomOptionCallback)(OptionMatchContext &match_context, const char *option);
typedef int (*FallbackOptionCallback)(OptionMatchContext &match_context, const char *option);
typedef void (*RequireOptionArgCallback)(const OptionMatchContext &match_context, const SingleArgOption &option);

struct ArglessOption {
	const char *name;
	ArglessOptionCallback callback;
};

struct SingleArgOption {
	const char *name;
	SingleArgOptionCallback callback;
};

struct CustomOption {
	const char *name;
	CustomOptionCallback callback;
};

using ArglessOptionMap = std::initializer_list<ArglessOption>;
using SingleArgOptionMap = std::initializer_list<SingleArgOption>;
using CustomOptionMap = std::initializer_list<CustomOption>;

struct CompiledOptionMap {
	peff::HashMap<std::string_view, const ArglessOption *> argless_options;
	peff::HashMap<std::string_view, const SingleArgOption *> single_arg_options;
	peff::HashMap<std::string_view, const CustomOption *> custom_options;
	FallbackOptionCallback fallback_option_callback;
	RequireOptionArgCallback require_option_arg_callback;

	INTERBUFC_FORCEINLINE CompiledOptionMap(peff::Alloc *alloc, FallbackOptionCallback fallback_option_callback, RequireOptionArgCallback require_option_arg_callback) noexcept : argless_options(alloc), single_arg_options(alloc), custom_options(alloc), fallback_option_callback(fallback_option_callback), require_option_arg_callback(require_option_arg_callback) {}
};

[[nodiscard]] bool build_option_map(
	CompiledOptionMap &option_map_out,
	const ArglessOptionMap &argless_options,
	const SingleArgOptionMap &single_arg_options,
	const CustomOptionMap &custom_options) {
	for (const auto &i : argless_options) {
		if (!option_map_out.argless_options.insert(std::string_view(i.name), &i)) {
			return false;
		}
	}

	for (const auto &i : single_arg_options) {
		if (!option_map_out.single_arg_options.insert(std::string_view(i.name), &i)) {
			return false;
		}
	}

	for (const auto &i : custom_options) {
		if (!option_map_out.custom_options.insert(std::string_view(i.name), &i)) {
			return false;
		}
	}

	return true;
}

[[nodiscard]] int match_args(const CompiledOptionMap &option_map, int argc, char **argv, void *user_data) {
	OptionMatchContext match_context = { argc, argv, 0, user_data };
	for (int i = 1; i < argc; ++i) {
		if (auto it = option_map.argless_options.find(std::string_view(argv[i])); it != option_map.argless_options.end()) {
			if (int result = it.value()->callback(match_context, argv[i]); result) {
				return result;
			}

			continue;
		}

		if (auto it = option_map.single_arg_options.find(std::string_view(argv[i])); it != option_map.single_arg_options.end()) {
			const char *opt = argv[i];
			if (++i == argc) {
				option_map.require_option_arg_callback(match_context, *it.value());
				return EINVAL;
			}

			if (int result = it.value()->callback(match_context, opt, argv[i]); result) {
				return result;
			}

			continue;
		}

		if (auto it = option_map.custom_options.find(std::string_view(argv[i])); it != option_map.custom_options.end()) {
			if (int result = it.value()->callback(match_context, argv[i]); result) {
				return result;
			}

			continue;
		}

		if (int result = option_map.fallback_option_callback(match_context, argv[i]); result) {
			return result;
		}
	}

	return 0;
}

#define print_error(fmt, ...) fprintf(stderr, "Error: " fmt, ##__VA_ARGS__)

struct MatchUserData {
	peff::DynArray<peff::String> *include_dirs;
};

const ArglessOptionMap g_argless_options = {

};

const SingleArgOptionMap g_single_arg_options = {
	{ "-l", [](const OptionMatchContext &match_context, const char *option, const char *arg) -> int {
		 MatchUserData *user_data = ((MatchUserData *)match_context.user_data);

		 if (!interbufc::g_language.empty()) {
			 print_error("Language is specified multiple times");
			 return EINVAL;
		 }

		 interbufc::g_language = arg;

		 return 0;
	 } },
	{ "-I", [](const OptionMatchContext &match_context, const char *option, const char *arg) -> int {
		 MatchUserData *user_data = ((MatchUserData *)match_context.user_data);

		 peff::String dir(peff::default_allocator());

		 if (!dir.build(arg)) {
			 print_error("Out of memory");
			 return ENOMEM;
		 }

		 if (!user_data->include_dirs->push_back(std::move(dir))) {
			 print_error("Out of memory");
			 return ENOMEM;
		 }

		 return 0;
	 } },
	{ "-o", [](const OptionMatchContext &match_context, const char *option, const char *arg) -> int {
		 interbufc::g_output_directory_path = arg;

		 return 0;
	 } }
};

const CustomOptionMap g_custom_options = {

};

void dump_lexical_error(const interbufc::LexicalError &lexical_error, int indent_level = 0) {
	for (int i = 0; i < indent_level; ++i) {
		putc('\t', stderr);
	}

	switch (lexical_error.kind) {
		case interbufc::LexicalErrorKind::UnrecognizedToken:
			print_error("Syntax error at %zu, %zu: Unrecognized token\n",
				lexical_error.location.begin_position.line + 1,
				lexical_error.location.begin_position.column + 1);
			break;
		case interbufc::LexicalErrorKind::UnexpectedEndOfLine:
			print_error("Syntax error at %zu, %zu: Unexpected end of line\n",
				lexical_error.location.begin_position.line + 1,
				lexical_error.location.begin_position.column + 1);
			break;
		case interbufc::LexicalErrorKind::PrematuredEndOfFile:
			print_error("Syntax error at %zu, %zu: Prematured end of file\n",
				lexical_error.location.begin_position.line + 1,
				lexical_error.location.begin_position.column + 1);
			break;
		case interbufc::LexicalErrorKind::OutOfMemory:
			print_error("Out of memory during lexical analysis\n");
			break;
	}
}

void dump_syntax_error(interbufc::Parser *parser, const interbufc::SyntaxError &syntax_error, int indent_level = 0) {
	const interbufc::Token *begin_token = parser->token_list.at(syntax_error.token_range.begin_index).get();
	const interbufc::Token *end_token = parser->token_list.at(syntax_error.token_range.end_index).get();

	for (int i = 0; i < indent_level; ++i) {
		putc('\t', stderr);
	}

	switch (syntax_error.error_kind) {
		case interbufc::SyntaxErrorKind::OutOfMemory:
			print_error("Syntax error at %zu, %zu: Out of memory\n",
				begin_token->source_location.begin_position.line + 1,
				begin_token->source_location.begin_position.column + 1);
			break;
		case interbufc::SyntaxErrorKind::UnexpectedToken:
			print_error("Syntax error at %zu, %zu: Unexpected token\n",
				begin_token->source_location.begin_position.line + 1,
				begin_token->source_location.begin_position.column + 1);
			break;
		case interbufc::SyntaxErrorKind::ExpectingSingleToken:
			print_error("Syntax error at %zu, %zu: Expecting %s\n",
				begin_token->source_location.begin_position.line + 1,
				begin_token->source_location.begin_position.column + 1,
				interbufc::get_token_name(std::get<interbufc::ExpectingSingleTokenErrorExData>(syntax_error.ex_data).expecting_token_id));
			break;
		case interbufc::SyntaxErrorKind::ExpectingTokens: {
			print_error("Syntax error at %zu, %zu: Expecting ",
				begin_token->source_location.begin_position.line + 1,
				begin_token->source_location.begin_position.column + 1);

			const interbufc::ExpectingTokensErrorExData &ex_data = std::get<interbufc::ExpectingTokensErrorExData>(syntax_error.ex_data);

			auto it = ex_data.expecting_token_ids.begin();

			fprintf(stderr, "%s", interbufc::get_token_name(*it));

			while (++it != ex_data.expecting_token_ids.end()) {
				fprintf(stderr, " or %s", interbufc::get_token_name(*it));
			}

			fprintf(stderr, "\n");
			break;
		}
		case interbufc::SyntaxErrorKind::ExpectingId:
			print_error("Syntax error at %zu, %zu: Expecting an identifier\n",
				begin_token->source_location.begin_position.line + 1,
				begin_token->source_location.begin_position.column + 1);
			break;
		case interbufc::SyntaxErrorKind::ExpectingExpr:
			print_error("Syntax error at %zu, %zu: Expecting an expression\n",
				begin_token->source_location.begin_position.line + 1,
				begin_token->source_location.begin_position.column + 1);
			break;
		case interbufc::SyntaxErrorKind::ExpectingStmt:
			print_error("Syntax error at %zu, %zu: Expecting a statement\n",
				begin_token->source_location.begin_position.line + 1,
				begin_token->source_location.begin_position.column + 1);
			break;
		case interbufc::SyntaxErrorKind::ExpectingDecl:
			print_error("Syntax error at %zu, %zu: Expecting a declaration\n",
				begin_token->source_location.begin_position.line + 1,
				begin_token->source_location.begin_position.column + 1);
			break;
		case interbufc::SyntaxErrorKind::NoMatchingTokensFound:
			print_error("Syntax error at %zu, %zu: Matching token not found\n",
				begin_token->source_location.begin_position.line + 1,
				begin_token->source_location.begin_position.column + 1);
			break;
		case interbufc::SyntaxErrorKind::ConflictingDefinitions: {
			print_error("Syntax error at %zu, %zu: Definition of `",
				begin_token->source_location.begin_position.line + 1,
				begin_token->source_location.begin_position.column + 1);

			const interbufc::ConflictingDefinitionsErrorExData &ex_data = std::get<interbufc::ConflictingDefinitionsErrorExData>(syntax_error.ex_data);

			fprintf(stderr, "%s' conflicts with other definitions\n", ex_data.member_name.data());
			break;
		}
		default:
			print_error("Syntax error at %zu, %zu: Unknown error (%d)\n",
				begin_token->source_location.begin_position.line + 1,
				begin_token->source_location.begin_position.column + 1,
				(int)syntax_error.error_kind);
			break;
	}
}

void dump_compilation_error(peff::SharedPtr<interbufc::Parser> parser, const interbufc::CompilationError &error, int indent_level = 0) {
	const interbufc::Token *begin_token = parser->token_list.at(error.token_range.begin_index).get();
	const interbufc::Token *end_token = parser->token_list.at(error.token_range.end_index).get();

	for (int i = 0; i < indent_level; ++i) {
		putc('\t', stderr);
	}

	switch (error.error_kind) {
		case interbufc::CompilationErrorKind::OutOfMemory:
			print_error("Error at %zu, %zu: Out of memory\n",
				begin_token->source_location.begin_position.line + 1,
				begin_token->source_location.begin_position.column + 1);
			break;
		case interbufc::CompilationErrorKind::ErrorOpeningFile: {
			const interbufc::ErrorOpeningFileError &e = std::get<interbufc::ErrorOpeningFileError>(error.ex_data);
			print_error("Error at %zu, %zu: Error opening file: %s\n",
				begin_token->source_location.begin_position.line + 1,
				begin_token->source_location.begin_position.column + 1,
				e.name.data());
			break;
		}
		case interbufc::CompilationErrorKind::IO:
			print_error("Error at %zu, %zu: File I/O error\n",
				begin_token->source_location.begin_position.line + 1,
				begin_token->source_location.begin_position.column + 1);
			break;
		case interbufc::CompilationErrorKind::InvalidEnumBaseType:
			print_error("Error at %zu, %zu: Invalid base type for enumeration\n",
				begin_token->source_location.begin_position.line + 1,
				begin_token->source_location.begin_position.column + 1);
			break;
		case interbufc::CompilationErrorKind::InvalidTypeName:
			print_error("Error at %zu, %zu: Invalid type name\n",
				begin_token->source_location.begin_position.line + 1,
				begin_token->source_location.begin_position.column + 1);
			break;
		default:
			print_error("Error at %zu, %zu: Unknown error (%d)\n",
				begin_token->source_location.begin_position.line + 1,
				begin_token->source_location.begin_position.column + 1,
				(int)error.error_kind);
			break;
	}
}

int main(int argc, char *argv[]) {
#ifdef _MSC_VER
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	peff::DynArray<peff::String> include_dirs(peff::default_allocator());
	{
		CompiledOptionMap option_map(
			peff::default_allocator(),
			[](OptionMatchContext &match_context, const char *option) -> int {
				if (interbufc::g_source_file_name.size()) {
					print_error("Duplicated target file name");
					return EINVAL;
				}

				interbufc::g_source_file_name = option;

				return 0;
			},
			[](const OptionMatchContext &match_context, const SingleArgOption &option) {
				print_error("Option `%s' requires more arguments", option.name);
			});

		if (!build_option_map(option_map, g_argless_options, g_single_arg_options, g_custom_options)) {
			print_error("Out of memory");
			return ENOMEM;
		}

		{
			MatchUserData match_user_data = {};
			match_user_data.include_dirs = &include_dirs;

			if (int result = match_args(option_map, argc, argv, &match_user_data); result) {
				return result;
			}
		}
	}

	if (interbufc::g_source_file_name.empty()) {
		print_error("Missing target file name");
		return EINVAL;
	}

	if (interbufc::g_output_directory_path.empty()) {
		print_error("Missing output path");
		return EINVAL;
	}

	FILE *fp = fopen(interbufc::g_source_file_name.data(), "rb");

	if (!fp) {
		print_error("Error opening the file");
		return EIO;
	}

	peff::ScopeGuard close_fp_guard([fp]() noexcept {
		fclose(fp);
	});

	if (fseek(fp, 0, SEEK_END)) {
		print_error("Error evaluating file size");
		return EIO;
	}

	long file_size;
	if ((file_size = ftell(fp)) < 1) {
		print_error("Error evaluating file size");
		return EIO;
	}

	if (fseek(fp, 0, SEEK_SET)) {
		print_error("Error evaluating file size");
		return EIO;
	}

	{
		auto deleter = [](char *ptr) {
			free(ptr);
		};
		std::unique_ptr<char[], decltype(deleter)> buf((char *)malloc((size_t)file_size + 1), deleter);

		if (!buf) {
			print_error("Error allocating memory for reading the file");
			return ENOMEM;
		}

		(buf.get())[file_size] = '\0';

		if (fread(buf.get(), file_size, 1, fp) < 1) {
			print_error("Error reading the file");
			return EIO;
		}

		peff::SharedPtr<interbufc::Document> document(peff::make_shared<interbufc::Document>(peff::default_allocator(), peff::default_allocator()));

		/* peff::SharedPtr<interbufc::FileSystemExternalModuleProvider> fs_external_mod_provider;

		if (!(fs_external_mod_provider = peff::make_shared<interbufc::FileSystemExternalModuleProvider>(peff::default_allocator(), peff::default_allocator()))) {
			print_error("Out of memory");
			return ENOMEM;
		}

		for (auto &i : include_dirs) {
			if (!fs_external_mod_provider->import_paths.push_back(std::move(i))) {
				print_error("Out of memory");
				return ENOMEM;
			}
		}

		include_dirs.clear();

		if (!document->external_module_providers.push_back(fs_external_mod_provider.cast_to<interbufc::ExternalModuleProvider>())) {
			print_error("Out of memory");
			return ENOMEM;
		}*/

		if (interbufc::g_language.empty()) {
			print_error("Language is not specified");
			return EINVAL;
		}

		if (interbufc::g_output_directory_path.empty()) {
			print_error("Output directory is not specified");
			return EINVAL;
		}

		interbufc::TokenList token_list(peff::default_allocator());
		{
			interbufc::Lexer lexer(peff::default_allocator());

			std::string_view sv(buf.get(), file_size);

			if (auto e = lexer.lex(sv, peff::default_allocator(), document); e) {
				dump_lexical_error(*e);
				return -1;
			}

			token_list = std::move(lexer.token_list);
		}

		{
			peff::SharedPtr<interbufc::Parser> parser;
			if (!(parser = peff::make_shared<interbufc::Parser>(peff::default_allocator(), document, std::move(token_list), peff::default_allocator()))) {
				print_error("Error allocating memory for the parser");
				return ENOMEM;
			}

			interbufc::AstNodePtr<interbufc::ModuleNode> root_mod;
			if (!(root_mod = peff::make_shared_with_control_block<interbufc::ModuleNode, interbufc::AstNodeControlBlock<interbufc::ModuleNode>>(peff::default_allocator(), peff::default_allocator(), document))) {
				print_error("Error allocating memory for the root module");
				return ENOMEM;
			}
			document->root_module = root_mod;

			interbufc::AstNodePtr<interbufc::ModuleNode> mod(peff::make_shared_with_control_block<interbufc::ModuleNode, interbufc::AstNodeControlBlock<interbufc::ModuleNode>>(peff::default_allocator(), peff::default_allocator(), document));
			if (!(mod = peff::make_shared_with_control_block<interbufc::ModuleNode, interbufc::AstNodeControlBlock<interbufc::ModuleNode>>(peff::default_allocator(), peff::default_allocator(), document))) {
				print_error("Error allocating memory for the target module");
				return ENOMEM;
			}

			bool encountered_errors = false;
			if (auto e = parser->parse_program(mod); e) {
				encountered_errors = true;
				dump_syntax_error(parser.get(), *e);
			}

			for (auto &i : parser->syntax_errors) {
				encountered_errors = true;
				dump_syntax_error(parser.get(), i);
			}

			if (interbufc::g_language == "cpp") {
				interbufc::CXXCompiler compiler(peff::default_allocator());

				std::optional<interbufc::CompilationError> e = compiler.compile(mod);

				if (e) {
					encountered_errors = true;
					dump_compilation_error(parser, *e, 0);
				}

				for (auto &i : compiler.errors) {
					encountered_errors = true;
					dump_compilation_error(parser, i, 0);
				}
			} else if (interbufc::g_language == "ts") {
				interbufc::TypeScriptCompiler compiler(peff::default_allocator());

				std::optional<interbufc::CompilationError> e = compiler.compile(mod);

				if (e) {
					encountered_errors = true;
					dump_compilation_error(parser, *e, 0);
				}

				for (auto &i : compiler.errors) {
					encountered_errors = true;
					dump_compilation_error(parser, i, 0);
				}
			} else {
				print_error("Unrecognized language");
				return EINVAL;
			}
		}
	}

	return 0;
}
