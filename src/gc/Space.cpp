#include "Space.h"

#include "engine/Engine.h"
#include "GC.h"
#include "Object.h"
#include "Container.h"

namespace gc {

Space::Space() {
	g_engine->GetGC()->AddSpace( this );
}

Space::~Space() {
	g_engine->GetGC()->RemoveSpace( this );
	while ( Collect() ) {}
#if defined( DEBUG ) || defined( FASTDEBUG )
	{
		if ( !m_objects.empty() ) {
			Log( "WARNING: " + std::to_string( m_objects.size() ) + " objects are still reachable!" );
			Collect();
		}
		else {
			Log( "All objects have been destroyed." );
		}
	}
#endif
}

void Space::Add( Object* object ) {
	std::lock_guard< std::mutex > guard( m_objects_to_add_mutex );
	//Log( "Adding object: " + std::to_string( (unsigned long)object ) );
	ASSERT( m_objects_to_add.find( object ) == m_objects_to_add.end(), "object " + std::to_string( (unsigned long)object ) + " already pending addition" );
	m_objects_to_add.insert( object );
}

void Space::AddRoot( Container* object ) {
	{
		std::lock_guard< std::mutex > guard( m_root_objects_mutex );
		Log( "Adding root object: " + std::to_string( (unsigned long)object ) );
		ASSERT( m_root_objects.find( object ) == m_root_objects.end(), "root object " + std::to_string( (unsigned long)object ) + " already exists" );
		m_root_objects.insert( object );
	}
	Add( object );
}

void Space::RemoveRoot( Container* object ) {
	std::lock_guard< std::mutex > guard( m_root_objects_mutex );
	Log( "Removing root object: " + std::to_string( (unsigned long)object ) );
	ASSERT( m_root_objects.find( object ) != m_root_objects.end(), "root object " + std::to_string( (unsigned long)object ) + " not found" );
	m_root_objects.erase( object );
}

const bool Space::Collect() {
	std::lock_guard< std::mutex > guard( m_collect_mutex ); // allow only one collection at same space at same time

	{
		std::lock_guard< std::mutex > guard2( m_objects_to_add_mutex );
		if ( !m_objects_to_add.empty() ) {
#if defined( DEBUG ) || defined( FASTDEBUG )
			for ( const auto& object : m_objects_to_add ) {
				ASSERT( m_objects.find( object ) == m_objects.end(), "object " + std::to_string( (unsigned long)object ) + " already added" );
			}
#endif
			m_objects.insert( m_objects_to_add.begin(), m_objects_to_add.end() );
			m_objects_to_add.clear();
		}
	}

	if ( m_objects.empty() ) {
		ASSERT( m_objects_to_remove.empty(), "objects empty but removals pending" );
		return false;
	}

	m_objects_to_remove_mutex.lock();
	const auto objects_to_remove = m_objects_to_remove;
	m_objects_to_remove.clear();
	m_objects_to_remove_mutex.unlock();

	m_root_objects_mutex.lock();
	const auto root_objects = m_root_objects;
	m_root_objects_mutex.unlock();

	ASSERT( m_reachable_objects_tmp.empty(), "reachable objects tmp not empty" );
	for ( const auto* object : root_objects ) { // prevent deletion of root objects
		m_reachable_objects_tmp.insert( (Object*)object );
	}

#if defined( DEBUG ) || defined( FASTDEBUG )
	for ( const auto& object_to_remove : objects_to_remove ) {
		ASSERT( m_objects.find( object_to_remove ) != m_objects.end(), "object " + std::to_string( (unsigned long)object_to_remove ) + " is pending removal but not found" );
	}
#endif

	Log( "Collecting from " + std::to_string( root_objects.size() ) + " root objects" );
	for ( const auto& object : root_objects ) {
		object->CollectActiveObjects( m_reachable_objects_tmp );
	}
	Log( "Found " + std::to_string( m_reachable_objects_tmp.size() ) + " reachable objects, " + std::to_string( m_objects.size() - m_reachable_objects_tmp.size() ) + " unreachable" );

	bool anything_removed = false;
	std::unordered_set< Object* > removed_objects = {};
	for ( const auto& object : m_objects ) {
		const auto& it = m_reachable_objects_tmp.find( object );
		if ( it == m_reachable_objects_tmp.end() ) {
			//Log( "Destroying unreachable object: " + std::to_string( (unsigned long)object ) );
			delete object;
			anything_removed = true;
			removed_objects.insert( object );
		}
	}

	m_reachable_objects_tmp.clear();

	if ( anything_removed ) {

		for ( const auto& object : removed_objects ) {
			ASSERT( m_objects.find( object ) != m_objects.end(), "object to be removed not found" );
			m_objects.erase( object );
		}

	}

	return anything_removed;
}

}
