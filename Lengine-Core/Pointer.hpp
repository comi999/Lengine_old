#pragma once
#include <memory>

template < typename T >
using Unique = std::unique_ptr< T >;

template < typename T >
using Shared = std::shared_ptr< T >;