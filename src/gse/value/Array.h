#pragma once

#include <vector>
#include <optional>

#include "Types.h"

namespace gse {

class Wrappable;

namespace value {

class Array : public Value {
public:

	static const type_t GetType() { return Value::T_ARRAY; }

	Array( array_elements_t initial_value = {} );

	Value* const Get( const size_t index ) const;
	Value* const GetSubArray( const std::optional< size_t > from, const std::optional< size_t > to ) const;
	void Set( const size_t index, Value* const new_value );
	const void SetSubArray( const std::optional< size_t > from, const std::optional< size_t > to, Value* const new_subarray );
	void Append( Value* const new_value );

	Value* const GetRef( const size_t index );
	Value* const GetRangeRef( const std::optional< size_t > from, const std::optional< size_t > to );

	array_elements_t value = {};

	static Value* const FromVector( const std::vector< Wrappable* >* data, const bool dynamic = false ); // be careful

private:
	void ValidateFromTo( const std::optional< size_t >& from, const std::optional< size_t >& to ) const;
};

}
}