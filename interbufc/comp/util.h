#ifndef _INTERBUFC_COMP_UTIL_H_
#define _INTERBUFC_COMP_UTIL_H_

#include "compiler.h"

namespace interbufc {
	class File final {
	private:
		FILE *_fp;

	public:
		INTERBUFC_FORCEINLINE void close() {
			if (_fp)
				fclose(_fp);
			_fp = nullptr;
		}
		INTERBUFC_FORCEINLINE File() : _fp(nullptr) {}
		INTERBUFC_FORCEINLINE File(FILE *fp) : _fp(fp) {}
		INTERBUFC_FORCEINLINE ~File() {
			close();
		}

		INTERBUFC_FORCEINLINE std::optional<CompilationError> read(char *buffer, size_t size) const {
			if (!size)
				return {};
			if (fread(buffer, size, 1, _fp) < 1)
				return genIOCompError();
			return {};
		}

		INTERBUFC_FORCEINLINE std::optional<CompilationError> write(const char *buffer, size_t size) const {
			if (!size)
				return {};
			if (fwrite(buffer, size, 1, _fp) < 1)
				return genIOCompError();
			return {};
		}

		INTERBUFC_FORCEINLINE std::optional<CompilationError> write(const std::string_view &s) const {
			if (!s.size())
				return {};
			if (fwrite(s.data(), s.size(), 1, _fp) < 1)
				return genIOCompError();
			return {};
		}

		INTERBUFC_FORCEINLINE void setCFile(FILE *fp) {
			close();
			_fp = fp;
		}

		INTERBUFC_FORCEINLINE FILE* cFile() const {
			return _fp;
		}
	};
}

#endif
