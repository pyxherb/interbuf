#ifndef _INTERBUFC_AST_IDREF_H_
#define _INTERBUFC_AST_IDREF_H_

#include "typename_base.h"
#include <peff/containers/dynarray.h>

namespace interbufc {
	struct IdRefEntry {
		peff::String name;
		size_t accessOpTokenIndex = SIZE_MAX, nameTokenIndex = SIZE_MAX, leftAngleBracketTokenIndex = SIZE_MAX, rightAngleBracketTokenIndex = SIZE_MAX;
		peff::DynArray<size_t> commaTokenIndices;

		INTERBUFC_FORCEINLINE IdRefEntry(peff::Alloc *selfAllocator): name(selfAllocator), commaTokenIndices(selfAllocator) {}
		INTERBUFC_FORCEINLINE IdRefEntry(IdRefEntry&& rhs): name(std::move(rhs.name)), accessOpTokenIndex(rhs.accessOpTokenIndex), nameTokenIndex(rhs.nameTokenIndex), leftAngleBracketTokenIndex(rhs.leftAngleBracketTokenIndex), rightAngleBracketTokenIndex(rhs.rightAngleBracketTokenIndex), commaTokenIndices(std::move(rhs.commaTokenIndices)) {
		}
	};

	INTERBUFC_API std::optional<IdRefEntry> duplicateIdRefEntry(peff::Alloc *selfAllocator, const IdRefEntry &rhs);

	class IdRef final {
	public:
		peff::RcObjectPtr<peff::Alloc> selfAllocator;
		peff::DynArray<IdRefEntry> entries;
		TokenRange tokenRange;

		INTERBUFC_API IdRef(peff::Alloc *selfAllocator);
		INTERBUFC_API virtual ~IdRef();

		INTERBUFC_API void dealloc() noexcept;
	};

	using IdRefPtr = std::unique_ptr<IdRef, peff::DeallocableDeleter<IdRef>>;

	INTERBUFC_API IdRefPtr duplicateIdRef(peff::Alloc *selfAllocator, IdRef *rhs);
}

#endif
