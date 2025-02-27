#include "Space.h"

#include <thread>
#include <chrono>

#include "engine/Engine.h"
#include "GC.h"
#include "Root.h"
#include "util/Time.h"

namespace gc {

Space::Space( Root* const root_object )
	: m_root_object( root_object ) {
	ASSERT_NOLOG( root_object, "root object is null" );
	g_engine->GetGC()->AddSpace( this );
}

Space::~Space() {
	g_engine->GetGC()->RemoveSpace( this );
	m_collect_mutex.lock(); // wait for any ongoing collects to finish
	m_is_destroying = true;
	m_collect_mutex.unlock();

	// collect until there's nothing to collect
	GC_LOG( "Destroying remaining objects" );
	{
		m_objects_mutex.lock();
		while ( !m_objects.empty() ) {
			GC_LOG( std::to_string( m_objects.size() ) + " objects are still reachable" );
			m_objects_mutex.unlock();
			if ( m_is_accumulating ) {
				Log( "WARNING: space is destroying but still accumulating" );
			}
			while ( Collect() ) {}
			m_objects_mutex.lock();
			if ( !m_objects.empty() ) {
				Log( "WARNING: collect finished but objects still not empty" );
			}
		}
		m_objects_mutex.unlock();
	}
	GC_LOG( "All objects have been destroyed." );
}

void Space::Add( Object* object ) {
	ASSERT_NOLOG( !m_is_destroying, "space is destroying" );
	if ( !m_is_accumulating ) {
		ASSERT_NOLOG( m_is_accumulating, "not accumulating" );
	}
	//GC_LOG( "Adding object: " + std::to_string( (unsigned long)object ) );
	ASSERT( m_accumulated_objects.find( object ) == m_accumulated_objects.end(), "object " + std::to_string( (unsigned long)object ) + " already exists" );
	m_accumulated_objects.insert( object );
}

void Space::Accumulate( const std::function< void() >& f ) {
	if ( m_is_accumulating ) { // recursive accumulate, nothing to do
		f();
	}
	else { // top accumulate, need to do full logic
		const auto& cleanup = [ this ]() {
			m_is_accumulating = false;
			if ( !m_accumulated_objects.empty() ) {
				std::lock_guard< std::mutex > guard2( m_objects_mutex );
				GC_LOG( "Accumulated " + std::to_string( m_accumulated_objects.size() ) + " objects" );
				m_objects.insert( m_accumulated_objects.begin(), m_accumulated_objects.end() );
				m_accumulated_objects.clear();
			}
		};
		std::lock_guard< std::mutex > guard( m_accumulation_mutex );
		ASSERT_NOLOG( m_accumulated_objects.empty(), "accumulated objects not empty" );
		m_is_accumulating = true;
		try {
			f();
		}
		catch ( const std::exception& e ) {
			cleanup();
			throw;
		}
		cleanup();
	}
}

const bool Space::Collect() {
	std::lock_guard< std::mutex > guard( m_collect_mutex ); // allow only one collection at same space at same time
	ASSERT( m_reachable_objects_tmp.empty(), "reachable objects tmp not empty" );

	GC_DEBUG_LOCK();
	GC_DEBUG_BEGIN( "Root" );
	m_root_object->GetReachableObjects( m_reachable_objects_tmp );
	GC_DEBUG_END();
	GC_DEBUG_UNLOCK();

	bool anything_removed = false;
	std::unordered_set< Object* > removed_objects = {};
	{
		std::lock_guard< std::mutex > guard2( m_objects_mutex );
		for ( const auto& object : m_objects ) {
			const auto& it = m_reachable_objects_tmp.find( object );
			if ( it == m_reachable_objects_tmp.end() ) {
				ASSERT( removed_objects.find( object ) == removed_objects.end(), "object " + std::to_string( (unsigned long)object ) + " was already removed" );
#if defined( DEBUG ) || defined( FASTDEBUG )
				GC_LOG( "Destroying unreachable object: " + std::to_string( (unsigned long)object ) );
#endif
				delete object;
				anything_removed = true;
				removed_objects.insert( object );
			}
		}
		GC_LOG( "Kept " + std::to_string( m_reachable_objects_tmp.size() ) + " reachable objects, removed " + std::to_string( removed_objects.size() ) + " unreachable" );

		m_reachable_objects_tmp.clear();

		if ( anything_removed ) {
			for ( const auto& object : removed_objects ) {
				ASSERT( m_objects.find( object ) != m_objects.end(), "object to be removed not found" );
				m_objects.erase( object );
			}
		}
	}

	return anything_removed;
}

}
