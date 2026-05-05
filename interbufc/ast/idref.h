#ifndef _INTERBUFC_AST_IDREF_H_
#define _INTERBUFC_AST_IDREF_H_

#include "typename_base.h"
#include <peff/containers/dynarray.h>

namespace interbufc {
	struct IdRefEntry {
		peff::String name;
		size_t access_op_token_index = SIZE_MAX, name_token_index = SIZE_MAX, left_angle_bracket_token_index = SIZE_MAX, right_angle_bracket_token_index = SIZE_MAX;
		peff::DynArray<size_t> comma_token_indices;

		INTERBUFC_FORCEINLINE IdRefEntry(peff::Alloc *self_allocator) : name(self_allocator), comma_token_indices(self_allocator) {}
		INTERBUFC_FORCEINLINE IdRefEntry(IdRefEntry &&rhs) noexcept : name(std::move(rhs.name)), access_op_token_index(rhs.access_op_token_index), name_token_index(rhs.name_token_index), left_angle_bracket_token_index(rhs.left_angle_bracket_token_index), right_angle_bracket_token_index(rhs.right_angle_bracket_token_index), comma_token_indices(std::move(rhs.comma_token_indices)) {
		}
	};

	INTERBUFC_API std::optional<IdRefEntry> duplicate_id_ref_entry(peff::Alloc *self_allocator, const IdRefEntry &rhs);

	class IdRef final {
	public:
		peff::RcObjectPtr<peff::Alloc> self_allocator;
		peff::DynArray<IdRefEntry> entries;
		TokenRange token_range;

		INTERBUFC_API IdRef(peff::Alloc *self_allocator);
		INTERBUFC_API virtual ~IdRef();

		INTERBUFC_API void dealloc() noexcept;
	};

	using IdRefPtr = std::unique_ptr<IdRef, peff::DeallocableDeleter<IdRef>>;

	INTERBUFC_API IdRefPtr duplicate_id_ref(peff::Alloc *self_allocator, IdRef *rhs);
}

#endif
