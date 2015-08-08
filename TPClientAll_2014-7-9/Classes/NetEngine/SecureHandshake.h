#pragma once

#include "NativeFeatureIncludes.h"

#if LIBCAT_SECURITY==1

#if !defined(_VENET_LIB) && defined(_VENET_DLL)
# define CAT_BUILD_DLL
#else
# define CAT_NEUTER_EXPORT
#endif

#include "cat/AllTunnel.hpp"

#endif
