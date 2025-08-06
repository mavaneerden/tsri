#pragma once

#ifdef TSRI_OPTION_DISABLE_ALWAYS_INLINE
#define TSRI_INLINE inline
#else
#define TSRI_INLINE [[gnu::always_inline]]
#endif
