#include "Async.h"

#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/value/Exception.h"
#include "gse/value/Int.h"
#include "gse/value/Undefined.h"
#include "gse/GSE.h"
#include "gse/Async.h"

namespace gse {
namespace builtins {

void Async::AddToContext( gc::Space* const gc_space, context::Context* ctx, ExecutionPointer& ep ) {

	ctx->CreateBuiltin( "async", NATIVE_CALL() {
		N_EXPECT_ARGS( 2 );
		N_GETVALUE( ms, 0, Int );

		const auto timer_id = ctx->GetGSE()->GetAsync()->StartTimer( ms, arguments.at(1 ), GSE_CALL_NOGC );

		const gse::value::object_properties_t properties = {
			{
				"stop",
				// recursive NATIVE_CALL doesn't work
				new gse::callable::Native( gc_space, ctx, [ timer_id ]( GSE_CALLABLE, const gse::value::function_arguments_t& arguments ) {
					N_EXPECT_ARGS( 0 );
					if ( !ctx->GetGSE()->GetAsync()->StopTimer( timer_id ) ) {
						GSE_ERROR( EC.OPERATION_FAILED, "Timer is already stopped" );
					}
					return VALUE( value::Undefined );
				} ),
			}
		};

		return VALUEEXT( value::Object, GSE_CALL, properties, "async" );
	} ), ep );

}

}
}
