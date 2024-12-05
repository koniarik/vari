
#pragma once

#include "vari/concept.h"

namespace vari
{

template < typename Deleter, typename... Ts >
class _uvptr;

template < typename Deleter, typename... Ts >
class _uvref;

template < typename... Ts >
class _vref;

template < typename... Ts >
class _vptr;

template < typename... Ts >
class _vval;

template < typename... Ts >
class _vopt;

}  // namespace vari
