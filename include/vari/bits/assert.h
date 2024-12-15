
#pragma once

#undef VARI_ASSERT

#if defined( VARI_DISABLE_ASSERTS ) || defined( NDEBUG )

#define VARI_ASSERT( _e ) ( (void) 0 )

#else

#include <cassert>

#define VARI_ASSERT( _e ) assert( _e )

#endif