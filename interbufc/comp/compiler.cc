#include "compiler.h"

using namespace interbufc;

std::string_view interbufc::g_sourceFileName, interbufc::g_outputDirectoryPath;

INTERBUFC_API Compiler::Compiler(peff::Alloc *allocator) : warnings(allocator), errors(allocator), allocator(allocator) {
}

INTERBUFC_API Compiler::~Compiler() {
}
