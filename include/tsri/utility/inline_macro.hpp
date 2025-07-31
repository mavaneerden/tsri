#pragma once

#ifdef TSRI_OPTION_ENABLE_ALWAYS_INLINE
#define TSRI_INLINE [[gnu::always_inline]]
#else
#define TSRI_INLINE inline
#endif
