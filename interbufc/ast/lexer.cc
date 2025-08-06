#include "lexer.h"

using namespace interbufc;

INTERBUFC_API TokenExtension::~TokenExtension() {}

INTERBUFC_API IntTokenExtension::IntTokenExtension(peff::Alloc *allocator, int data) : allocator(allocator), data(data) {
}
INTERBUFC_API IntTokenExtension::~IntTokenExtension() {
}
INTERBUFC_API void IntTokenExtension::dealloc() {
	peff::destroyAndRelease<IntTokenExtension>(allocator.get(), this, sizeof(std::max_align_t));
}

INTERBUFC_API UIntTokenExtension::UIntTokenExtension(peff::Alloc *allocator, unsigned int data) : allocator(allocator), data(data) {
}
INTERBUFC_API UIntTokenExtension::~UIntTokenExtension() {
}
INTERBUFC_API void UIntTokenExtension::dealloc() {
	peff::destroyAndRelease<UIntTokenExtension>(allocator.get(), this, sizeof(std::max_align_t));
}

INTERBUFC_API LongTokenExtension::LongTokenExtension(peff::Alloc *allocator, long long data) : allocator(allocator), data(data) {
}
INTERBUFC_API LongTokenExtension::~LongTokenExtension() {
}
INTERBUFC_API void LongTokenExtension::dealloc() {
	peff::destroyAndRelease<LongTokenExtension>(allocator.get(), this, sizeof(std::max_align_t));
}

INTERBUFC_API ULongTokenExtension::ULongTokenExtension(peff::Alloc *allocator, unsigned long long data) : allocator(allocator), data(data) {
}
INTERBUFC_API ULongTokenExtension::~ULongTokenExtension() {
}
INTERBUFC_API void ULongTokenExtension::dealloc() {
	peff::destroyAndRelease<ULongTokenExtension>(allocator.get(), this, sizeof(std::max_align_t));
}

INTERBUFC_API F32TokenExtension::F32TokenExtension(peff::Alloc *allocator, float data) : allocator(allocator), data(data) {
}
INTERBUFC_API F32TokenExtension::~F32TokenExtension() {
}
INTERBUFC_API void F32TokenExtension::dealloc() {
	peff::destroyAndRelease<F32TokenExtension>(allocator.get(), this, sizeof(std::max_align_t));
}

INTERBUFC_API F64TokenExtension::F64TokenExtension(peff::Alloc *allocator, double data) : allocator(allocator), data(data) {
}
INTERBUFC_API F64TokenExtension::~F64TokenExtension() {
}
INTERBUFC_API void F64TokenExtension::dealloc() {
	peff::destroyAndRelease<F64TokenExtension>(allocator.get(), this, sizeof(std::max_align_t));
}

INTERBUFC_API StringTokenExtension::StringTokenExtension(peff::Alloc *allocator, peff::String &&data) : allocator(allocator), data(std::move(data)) {
}
INTERBUFC_API StringTokenExtension::~StringTokenExtension() {
}
INTERBUFC_API void StringTokenExtension::dealloc() {
	peff::destroyAndRelease<StringTokenExtension>(allocator.get(), this, sizeof(std::max_align_t));
}

INTERBUFC_API Token::Token(peff::Alloc *allocator, const peff::WeakPtr<Document> &document) : allocator(allocator), document(document) {
}
INTERBUFC_API Token::~Token() {
}
INTERBUFC_API void Token::dealloc() {
	peff::destroyAndRelease<Token>(allocator.get(), this, sizeof(std::max_align_t));
}

INTERBUFC_API const char *interbufc::getTokenName(TokenId tokenId) {
	switch (tokenId) {
		case TokenId::End:
			return "end of file";
		case TokenId::Comma:
			return ",";
		case TokenId::Question:
			return "?";
		case TokenId::Colon:
			return ":";
		case TokenId::Semicolon:
			return ";";
		case TokenId::LBracket:
			return "[";
		case TokenId::RBracket:
			return "]";
		case TokenId::LBrace:
			return "{";
		case TokenId::RBrace:
			return "}";
		case TokenId::LParenthese:
			return "(";
		case TokenId::RParenthese:
			return ")";
		case TokenId::At:
			return "@";
		case TokenId::Dot:
			return ".";
		case TokenId::VarArg:
			return "...";
		case TokenId::AddOp:
			return "+";
		case TokenId::SubOp:
			return "-";
		case TokenId::MulOp:
			return "*";
		case TokenId::DivOp:
			return "/";
		case TokenId::ModOp:
			return "%";
		case TokenId::AndOp:
			return "&";
		case TokenId::OrOp:
			return "|";
		case TokenId::XorOp:
			return "^";
		case TokenId::LNotOp:
			return "!";
		case TokenId::NotOp:
			return "~";
		case TokenId::AssignOp:
			return "=";
		case TokenId::AddAssignOp:
			return "+=";
		case TokenId::SubAssignOp:
			return "-=";
		case TokenId::MulAssignOp:
			return "*=";
		case TokenId::DivAssignOp:
			return "/=";
		case TokenId::ModAssignOp:
			return "%=";
		case TokenId::AndAssignOp:
			return "&=";
		case TokenId::OrAssignOp:
			return "|=";
		case TokenId::XorAssignOp:
			return "^=";
		case TokenId::LshAssignOp:
			return "<<=";
		case TokenId::RshAssignOp:
			return ">>=";
		case TokenId::StrictEqOp:
			return "===";
		case TokenId::StrictNeqOp:
			return "!==";
		case TokenId::EqOp:
			return "==";
		case TokenId::NeqOp:
			return "!=";
		case TokenId::LshOp:
			return "<<";
		case TokenId::RshOp:
			return ">>";
		case TokenId::LtOp:
			return "<";
		case TokenId::GtOp:
			return ">";
		case TokenId::LtEqOp:
			return "<=";
		case TokenId::GtEqOp:
			return ">=";
		case TokenId::LAndOp:
			return "&&";
		case TokenId::LOrOp:
			return "||";
		case TokenId::MatchOp:
			return "=>";
		case TokenId::ReturnTypeOp:
			return "->";
		case TokenId::CmpOp:
			return "<=>";
		case TokenId::DollarOp:
			return "$";
		case TokenId::AsKeyword:
			return "as";
		case TokenId::ClassKeyword:
			return "class";
		case TokenId::EnumKeyword:
			return "enum";
		case TokenId::ImportKeyword:
			return "import";
		case TokenId::ModuleKeyword:
			return "module";
		case TokenId::I8TypeName:
			return "i8";
		case TokenId::I16TypeName:
			return "i16";
		case TokenId::I32TypeName:
			return "i32";
		case TokenId::I64TypeName:
			return "i64";
		case TokenId::U8TypeName:
			return "u8";
		case TokenId::U16TypeName:
			return "u16";
		case TokenId::U32TypeName:
			return "u32";
		case TokenId::U64TypeName:
			return "u64";
		case TokenId::F32TypeName:
			return "f32";
		case TokenId::F64TypeName:
			return "f64";
		case TokenId::StringTypeName:
			return "string";
		case TokenId::BoolTypeName:
			return "bool";
		case TokenId::AutoTypeName:
			return "auto";
		case TokenId::VoidTypeName:
			return "void";
		case TokenId::ObjectTypeName:
			return "object";
		case TokenId::AnyTypeName:
			return "any";
		case TokenId::IntLiteral:
			return "integer literal";
		case TokenId::LongLiteral:
			return "long literal";
		case TokenId::UIntLiteral:
			return "unsigned integer literal";
		case TokenId::ULongLiteral:
			return "unsigned long literal";
		case TokenId::F32Literal:
			return "32-bit floating-point number literal";
		case TokenId::F64Literal:
			return "64-bit floating-point number literal";
		case TokenId::StringLiteral:
			return "string literal";
		case TokenId::RawStringLiteral:
			return "raw string literal";
		case TokenId::Id:
			return "identifier";
	}

	return "<unknown tokenId>";
}
