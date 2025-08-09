#include "ast/parser.h"
#include <initializer_list>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

struct OptionMatchContext {
	const int argc;
	char **const argv;
	int i;
	void *userData;
};

struct SingleArgOption;

typedef int (*ArglessOptionCallback)(const OptionMatchContext &matchContext, const char *option);
typedef int (*SingleArgOptionCallback)(const OptionMatchContext &matchContext, const char *option, const char *arg);
typedef int (*CustomOptionCallback)(OptionMatchContext &matchContext, const char *option);
typedef int (*FallbackOptionCallback)(OptionMatchContext &matchContext, const char *option);
typedef void (*RequireOptionArgCallback)(const OptionMatchContext &matchContext, const SingleArgOption &option);

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
	peff::HashMap<std::string_view, const ArglessOption *> arglessOptions;
	peff::HashMap<std::string_view, const SingleArgOption *> singleArgOptions;
	peff::HashMap<std::string_view, const CustomOption *> customOptions;
	FallbackOptionCallback fallbackOptionCallback;
	RequireOptionArgCallback requireOptionArgCallback;

	INTERBUFC_FORCEINLINE CompiledOptionMap(peff::Alloc *alloc, FallbackOptionCallback fallbackOptionCallback, RequireOptionArgCallback requireOptionArgCallback) noexcept : arglessOptions(alloc), singleArgOptions(alloc), customOptions(alloc), fallbackOptionCallback(fallbackOptionCallback), requireOptionArgCallback(requireOptionArgCallback) {}
};

[[nodiscard]] bool buildOptionMap(
	CompiledOptionMap &optionMapOut,
	const ArglessOptionMap &arglessOptions,
	const SingleArgOptionMap &singleArgOptions,
	const CustomOptionMap &customOptions) {
	for (const auto &i : arglessOptions) {
		if (!optionMapOut.arglessOptions.insert(std::string_view(i.name), &i)) {
			return false;
		}
	}

	for (const auto &i : singleArgOptions) {
		if (!optionMapOut.singleArgOptions.insert(std::string_view(i.name), &i)) {
			return false;
		}
	}

	for (const auto &i : customOptions) {
		if (!optionMapOut.customOptions.insert(std::string_view(i.name), &i)) {
			return false;
		}
	}

	return true;
}

[[nodiscard]] int matchArgs(const CompiledOptionMap &optionMap, int argc, char **argv, void *userData) {
	OptionMatchContext matchContext = { argc, argv, 0, userData };
	for (int i = 1; i < argc; ++i) {
		if (auto it = optionMap.arglessOptions.find(std::string_view(argv[i])); it != optionMap.arglessOptions.end()) {
			if (int result = it.value()->callback(matchContext, argv[i]); result) {
				return result;
			}

			continue;
		}

		if (auto it = optionMap.singleArgOptions.find(std::string_view(argv[i])); it != optionMap.singleArgOptions.end()) {
			const char *opt = argv[i];
			if (++i == argc) {
				optionMap.requireOptionArgCallback(matchContext, *it.value());
				return EINVAL;
			}

			if (int result = it.value()->callback(matchContext, opt, argv[i]); result) {
				return result;
			}

			continue;
		}

		if (auto it = optionMap.customOptions.find(std::string_view(argv[i])); it != optionMap.customOptions.end()) {
			if (int result = it.value()->callback(matchContext, argv[i]); result) {
				return result;
			}

			continue;
		}

		if (int result = optionMap.fallbackOptionCallback(matchContext, argv[i]); result) {
			return result;
		}
	}

	return 0;
}

#define printError(fmt, ...) fprintf(stderr, "Error: " fmt, ##__VA_ARGS__)

struct MatchUserData {
	peff::DynArray<peff::String> *includeDirs;
};

const ArglessOptionMap g_arglessOptions = {

};

const char *g_modFileName = nullptr, *g_outputFileName = nullptr;

const SingleArgOptionMap g_singleArgOptions = {
	{ "-I", [](const OptionMatchContext &matchContext, const char *option, const char *arg) -> int {
		 MatchUserData *userData = ((MatchUserData *)matchContext.userData);

		 peff::String dir(peff::getDefaultAlloc());

		 if (!dir.build(arg)) {
			 printError("Out of memory");
			 return ENOMEM;
		 }

		 if (!userData->includeDirs->pushBack(std::move(dir))) {
			 printError("Out of memory");
			 return ENOMEM;
		 }

		 return 0;
	 } },
	{ "-o", [](const OptionMatchContext &matchContext, const char *option, const char *arg) -> int {
		 g_outputFileName = arg;

		 return 0;
	 } }
};

const CustomOptionMap g_customOptions = {

};

void dumpLexicalError(const interbufc::LexicalError &lexicalError, int indentLevel = 0) {
	for (int i = 0; i < indentLevel; ++i) {
		putc('\t', stderr);
	}

	switch (lexicalError.kind) {
		case interbufc::LexicalErrorKind::UnrecognizedToken:
			printError("Syntax error at %zu, %zu: Unrecognized token\n",
				lexicalError.location.beginPosition.line + 1,
				lexicalError.location.beginPosition.column + 1);
			break;
		case interbufc::LexicalErrorKind::UnexpectedEndOfLine:
			printError("Syntax error at %zu, %zu: Unexpected end of line\n",
				lexicalError.location.beginPosition.line + 1,
				lexicalError.location.beginPosition.column + 1);
			break;
		case interbufc::LexicalErrorKind::PrematuredEndOfFile:
			printError("Syntax error at %zu, %zu: Prematured end of file\n",
				lexicalError.location.beginPosition.line + 1,
				lexicalError.location.beginPosition.column + 1);
			break;
		case interbufc::LexicalErrorKind::OutOfMemory:
			printError("Out of memory during lexical analysis\n");
			break;
	}
}

void dumpSyntaxError(interbufc::Parser *parser, const interbufc::SyntaxError &syntaxError, int indentLevel = 0) {
	const interbufc::Token *beginToken = parser->tokenList.at(syntaxError.tokenRange.beginIndex).get();
	const interbufc::Token *endToken = parser->tokenList.at(syntaxError.tokenRange.endIndex).get();

	for (int i = 0; i < indentLevel; ++i) {
		putc('\t', stderr);
	}

	switch (syntaxError.errorKind) {
		case interbufc::SyntaxErrorKind::OutOfMemory:
			printError("Syntax error at %zu, %zu: Out of memory\n",
				beginToken->sourceLocation.beginPosition.line + 1,
				beginToken->sourceLocation.beginPosition.column + 1);
			break;
		case interbufc::SyntaxErrorKind::UnexpectedToken:
			printError("Syntax error at %zu, %zu: Unexpected token\n",
				beginToken->sourceLocation.beginPosition.line + 1,
				beginToken->sourceLocation.beginPosition.column + 1);
			break;
		case interbufc::SyntaxErrorKind::ExpectingSingleToken:
			printError("Syntax error at %zu, %zu: Expecting %s\n",
				beginToken->sourceLocation.beginPosition.line + 1,
				beginToken->sourceLocation.beginPosition.column + 1,
				interbufc::getTokenName(std::get<interbufc::ExpectingSingleTokenErrorExData>(syntaxError.exData).expectingTokenId));
			break;
		case interbufc::SyntaxErrorKind::ExpectingTokens: {
			printError("Syntax error at %zu, %zu: Expecting ",
				beginToken->sourceLocation.beginPosition.line + 1,
				beginToken->sourceLocation.beginPosition.column + 1);

			const interbufc::ExpectingTokensErrorExData &exData = std::get<interbufc::ExpectingTokensErrorExData>(syntaxError.exData);

			auto it = exData.expectingTokenIds.begin();

			fprintf(stderr, "%s", interbufc::getTokenName(*it));

			while (++it != exData.expectingTokenIds.end()) {
				fprintf(stderr, " or %s", interbufc::getTokenName(*it));
			}

			fprintf(stderr, "\n");
			break;
		}
		case interbufc::SyntaxErrorKind::ExpectingId:
			printError("Syntax error at %zu, %zu: Expecting an identifier\n",
				beginToken->sourceLocation.beginPosition.line + 1,
				beginToken->sourceLocation.beginPosition.column + 1);
			break;
		case interbufc::SyntaxErrorKind::ExpectingExpr:
			printError("Syntax error at %zu, %zu: Expecting an expression\n",
				beginToken->sourceLocation.beginPosition.line + 1,
				beginToken->sourceLocation.beginPosition.column + 1);
			break;
		case interbufc::SyntaxErrorKind::ExpectingStmt:
			printError("Syntax error at %zu, %zu: Expecting a statement\n",
				beginToken->sourceLocation.beginPosition.line + 1,
				beginToken->sourceLocation.beginPosition.column + 1);
			break;
		case interbufc::SyntaxErrorKind::ExpectingDecl:
			printError("Syntax error at %zu, %zu: Expecting a declaration\n",
				beginToken->sourceLocation.beginPosition.line + 1,
				beginToken->sourceLocation.beginPosition.column + 1);
			break;
		case interbufc::SyntaxErrorKind::NoMatchingTokensFound:
			printError("Syntax error at %zu, %zu: Matching token not found\n",
				beginToken->sourceLocation.beginPosition.line + 1,
				beginToken->sourceLocation.beginPosition.column + 1);
			break;
		case interbufc::SyntaxErrorKind::ConflictingDefinitions: {
			printError("Syntax error at %zu, %zu: Definition of `",
				beginToken->sourceLocation.beginPosition.line + 1,
				beginToken->sourceLocation.beginPosition.column + 1);

			const interbufc::ConflictingDefinitionsErrorExData &exData = std::get<interbufc::ConflictingDefinitionsErrorExData>(syntaxError.exData);

			fprintf(stderr, "%s' conflicts with other definitions\n", exData.memberName.data());
			break;
		}
		default:
			printError("Syntax error at %zu, %zu: Unknown error (%d)\n",
				beginToken->sourceLocation.beginPosition.line + 1,
				beginToken->sourceLocation.beginPosition.column + 1,
				(int)syntaxError.errorKind);
			break;
	}
}

void dumpCompilationError(peff::SharedPtr<interbufc::Parser> parser, const interbufc::CompilationError &error, int indentLevel = 0) {
	const interbufc::Token *beginToken = parser->tokenList.at(error.tokenRange.beginIndex).get();
	const interbufc::Token *endToken = parser->tokenList.at(error.tokenRange.endIndex).get();

	for (int i = 0; i < indentLevel; ++i) {
		putc('\t', stderr);
	}

	switch (error.errorKind) {
		case interbufc::CompilationErrorKind::OutOfMemory:
			printError("Error at %zu, %zu: Out of memory\n",
				beginToken->sourceLocation.beginPosition.line + 1,
				beginToken->sourceLocation.beginPosition.column + 1);
			break;
		default:
			printError("Error at %zu, %zu: Unknown error (%d)\n",
				beginToken->sourceLocation.beginPosition.line + 1,
				beginToken->sourceLocation.beginPosition.column + 1,
				(int)error.errorKind);
			break;
	}
}
/*
class FileWriter : public interbufc::Writer {
public:
	FILE *fp = NULL;

	FileWriter(FILE *fp) : fp(fp) {
	}

	SLKC_API virtual ~FileWriter() {
		if (fp)
			fclose(fp);
	}

	virtual std::optional<interbufc::CompilationError> write(const char *src, size_t size) override {
		if (fwrite(src, size, 1, fp) < 1) {
			return interbufc::CompilationError(interbufc::TokenRange{ 0 }, interbufc::CompilationErrorKind::ErrorWritingCompiledModule);
		}
		return {};
	}
};

class ANSIDumpWriter : public interbufc::DumpWriter {
public:
	ANSIDumpWriter() {
	}

	SLKC_API virtual ~ANSIDumpWriter() {
	}

	virtual bool write(const char *src, size_t size) override {
		fwrite(src, size, 1, stdout);
		return true;
	}
};*/

int main(int argc, char *argv[]) {
#ifdef _MSC_VER
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	peff::DynArray<peff::String> includeDirs(peff::getDefaultAlloc());
	{
		CompiledOptionMap optionMap(
			peff::getDefaultAlloc(),
			[](OptionMatchContext &matchContext, const char *option) -> int {
				if (g_modFileName) {
					printError("Duplicated target file name");
					return EINVAL;
				}

				g_modFileName = option;

				return 0;
			},
			[](const OptionMatchContext &matchContext, const SingleArgOption &option) {
				printError("Option `%s' requires more arguments", option.name);
			});

		if (!buildOptionMap(optionMap, g_arglessOptions, g_singleArgOptions, g_customOptions)) {
			printError("Out of memory");
			return ENOMEM;
		}

		{
			MatchUserData matchUserData = {};
			matchUserData.includeDirs = &includeDirs;

			if (int result = matchArgs(optionMap, argc, argv, &matchUserData); result) {
				return result;
			}
		}
	}

	if (!g_modFileName) {
		printError("Missing target file name");
		return EINVAL;
	}

	if (!g_outputFileName) {
		printError("Missing output file name");
		return EINVAL;
	}

	FILE *fp = fopen(g_modFileName, "rb");

	if (!fp) {
		printError("Error opening the file");
		return EIO;
	}

	peff::ScopeGuard closeFpGuard([fp]() noexcept {
		fclose(fp);
	});

	if (fseek(fp, 0, SEEK_END)) {
		printError("Error evaluating file size");
		return EIO;
	}

	long fileSize;
	if ((fileSize = ftell(fp)) < 1) {
		printError("Error evaluating file size");
		return EIO;
	}

	if (fseek(fp, 0, SEEK_SET)) {
		printError("Error evaluating file size");
		return EIO;
	}

	{
		auto deleter = [](char *ptr) {
			free(ptr);
		};
		std::unique_ptr<char[], decltype(deleter)> buf((char *)malloc((size_t)fileSize + 1), deleter);

		if (!buf) {
			printError("Error allocating memory for reading the file");
			return ENOMEM;
		}

		(buf.get())[fileSize] = '\0';

		if (fread(buf.get(), fileSize, 1, fp) < 1) {
			printError("Error reading the file");
			return EIO;
		}

		peff::SharedPtr<interbufc::Document> document(peff::makeShared<interbufc::Document>(peff::getDefaultAlloc(), peff::getDefaultAlloc()));

		/* peff::SharedPtr<interbufc::FileSystemExternalModuleProvider> fsExternalModProvider;

		if (!(fsExternalModProvider = peff::makeShared<interbufc::FileSystemExternalModuleProvider>(peff::getDefaultAlloc(), peff::getDefaultAlloc()))) {
			printError("Out of memory");
			return ENOMEM;
		}

		for (auto &i : includeDirs) {
			if (!fsExternalModProvider->importPaths.pushBack(std::move(i))) {
				printError("Out of memory");
				return ENOMEM;
			}
		}

		includeDirs.clear();

		if (!document->externalModuleProviders.pushBack(fsExternalModProvider.castTo<interbufc::ExternalModuleProvider>())) {
			printError("Out of memory");
			return ENOMEM;
		}*/

		peff::Uninitialized<interbufc::TokenList> tokenList;
		{
			interbufc::Lexer lexer(peff::getDefaultAlloc());

			std::string_view sv(buf.get(), fileSize);

			if (auto e = lexer.lex(sv, peff::getDefaultAlloc(), document); e) {
				dumpLexicalError(*e);
				return -1;
			}

			tokenList.moveFrom(std::move(lexer.tokenList));
		}

		{
			peff::SharedPtr<interbufc::Parser> parser;
			if (!(parser = peff::makeShared<interbufc::Parser>(peff::getDefaultAlloc(), document, tokenList.release(), peff::getDefaultAlloc()))) {
				printError("Error allocating memory for the parser");
				return ENOMEM;
			}

			interbufc::AstNodePtr<interbufc::ModuleNode> rootMod;
			if (!(rootMod = peff::makeSharedWithControlBlock<interbufc::ModuleNode, interbufc::AstNodeControlBlock<interbufc::ModuleNode>>(peff::getDefaultAlloc(), peff::getDefaultAlloc(), document))) {
				printError("Error allocating memory for the root module");
				return ENOMEM;
			}
			document->rootModule = rootMod;

			interbufc::IdRefPtr moduleName;

			interbufc::AstNodePtr<interbufc::ModuleNode> mod(peff::makeSharedWithControlBlock<interbufc::ModuleNode, interbufc::AstNodeControlBlock<interbufc::ModuleNode>>(peff::getDefaultAlloc(), peff::getDefaultAlloc(), document));
			if (!(mod = peff::makeSharedWithControlBlock<interbufc::ModuleNode, interbufc::AstNodeControlBlock<interbufc::ModuleNode>>(peff::getDefaultAlloc(), peff::getDefaultAlloc(), document))) {
				printError("Error allocating memory for the target module");
				return ENOMEM;
			}

			bool encounteredErrors = false;
			if (auto e = parser->parseProgram(mod, moduleName); e) {
				encounteredErrors = true;
				dumpSyntaxError(parser, *e);
			}

			for (auto &i : parser->syntaxErrors) {
				encounteredErrors = true;
				dumpSyntaxError(parser, i);
			}
		}
	}

	return 0;
}
