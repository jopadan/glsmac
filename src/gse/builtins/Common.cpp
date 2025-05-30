#include "Common.h"

#include "gse/GSE.h"
#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/value/Int.h"
#include "gse/value/String.h"
#include "gse/value/Array.h"
#include "gse/value/Bool.h"
#include "gse/value/Undefined.h"
#include "gse/value/Object.h"

namespace gse {
namespace builtins {

void Common::AddToContext( gc::Space* const gc_space, context::Context* ctx, ExecutionPointer& ep ) {

	ctx->CreateBuiltin( "typeof", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETPTR( v, 0 );
		return VALUE( value::String,, v ? v->GetTypeString() : "Undefined" );
	} ), ep );

	ctx->CreateBuiltin( "classof", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETPTR( v, 0 );
		if ( v && v->type == Value::T_OBJECT ) {
			return VALUE( value::String,, ( ( value::Object*)v )->object_class );
		}
		return VALUE( value::Undefined );
	} ), ep );

	ctx->CreateBuiltin( "sizeof", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETPTR( v, 0 );
		size_t size = 0;
		if ( !v ) {
			GSE_ERROR( EC.OPERATION_NOT_SUPPORTED, "Could not get size of Undefined" );
		}
		switch ( v->type ) {
			case Value::T_ARRAY: {
				size = ((value::Array*)v)->value.size();
				break;
			}
			default:
				GSE_ERROR( EC.OPERATION_NOT_SUPPORTED, "Could not get size of " + v->GetTypeString() + ": " + v->ToString() );
		}
		return VALUE( value::Int,, size );
	} ), ep );

	ctx->CreateBuiltin("is_defined", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETPTR( v, 0 );
		if ( !v ) {
			return VALUE( gse::value::Bool,, false );
		}
		switch ( v->type ) {
			case Value::T_UNDEFINED:
				return VALUE( gse::value::Bool,, false );
			default:
				return VALUE( gse::value::Bool,, true );
		}
	} ), ep );

	ctx->CreateBuiltin( "is_empty", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETPTR( v, 0 );
		bool is_empty = true;
		if ( v ) {
			switch ( v->type ) {
				case Value::T_STRING: {
					is_empty = ( (value::String*)v )->value.empty();
					break;
				}
				case Value::T_ARRAY: {
					is_empty = ( (value::Array*)v )->value.empty();
					break;
				}
				case Value::T_OBJECT: {
					is_empty = ( (value::Object*)v )->value.empty();
					break;
				}
				default:
					GSE_ERROR( EC.OPERATION_NOT_SUPPORTED, "Could not get size of " + v->GetTypeString() + ": " + v->ToString() );
			}
		}
		return VALUE( value::Bool,, is_empty );
	} ), ep );

	ctx->CreateBuiltin( "clone", NATIVE_CALL()
	{
		N_EXPECT_ARGS( 1 );
		const auto& v = arguments.at(0);
		switch ( v->type ) {
			case Value::T_OBJECT:
			case Value::T_ARRAY:
				return v->Clone();
			default:
				GSE_ERROR( EC.OPERATION_NOT_SUPPORTED, "Cloning of type " + v->GetTypeString() + " is not supported" );
		}
	} ), ep );

	ctx->CreateBuiltin( "undefined", VALUE( value::Undefined ), ep );
}

}
}
