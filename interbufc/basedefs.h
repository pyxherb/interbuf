#ifndef _INTERBUFC_BASE_BASEDEFS_H_
#define _INTERBUFC_BASE_BASEDEFS_H_

#include <peff/base/basedefs.h>

#if INTERBUFC_DYNAMIC_LINK
	#if defined(_MSC_VER)
		#define INTERBUFC_DLLEXPORT __declspec(dllexport)
		#define INTERBUFC_DLLIMPORT __declspec(dllimport)
	#elif defined(__GNUC__) || defined(__clang__)
		#define INTERBUFC_DLLEXPORT __attribute__((__visibility__("default")))
		#define INTERBUFC_DLLIMPORT __attribute__((__visibility__("default")))
	#endif
#else
	#define INTERBUFC_DLLEXPORT
	#define INTERBUFC_DLLIMPORT
#endif

#define INTERBUFC_FORCEINLINE PEFF_FORCEINLINE

#if defined(_MSC_VER)
	#define INTERBUFC_DECL_EXPLICIT_INSTANTIATED_CLASS(apiModifier, name, ...) \
		apiModifier extern template class name<__VA_ARGS__>;
	#define INTERBUFC_DEF_EXPLICIT_INSTANTIATED_CLASS(apiModifier, name, ...) \
		apiModifier template class name<__VA_ARGS__>;
#elif defined(__GNUC__) || defined(__clang__)
	#define INTERBUFC_DECL_EXPLICIT_INSTANTIATED_CLASS(apiModifier, name, ...) \
		extern template class apiModifier name<__VA_ARGS__>;
	#define INTERBUFC_DEF_EXPLICIT_INSTANTIATED_CLASS(apiModifier, name, ...) \
		template class name<__VA_ARGS__>;
#else
	#define INTERBUFC_DECL_EXPLICIT_INSTANTIATED_CLASS(apiModifier, name, ...)
	#define INTERBUFC_DEF_EXPLICIT_INSTANTIATED_CLASS(apiModifier, name, ...)
#endif

#if IS_INTERBUFC_BUILDING
	#define INTERBUFC_API INTERBUFC_DLLEXPORT
#else
	#define INTERBUFC_API INTERBUFC_DLLIMPORT
#endif

#endif
