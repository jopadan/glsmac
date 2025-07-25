#pragma once

#include <unordered_map>
#include <functional>
#include <vector>

#include "gse/GCWrappable.h"

#include "Types.h"

#include "gse/value/Object.h"
#include "util/Clamper.h"
#include "types/Vec2.h"
#include "types/mesh/Types.h"

namespace input {
class Event;
}

namespace gse::context {
class Context;
}

namespace gc {
class Object;
}

namespace scene {
class Scene;
}

namespace ui {

namespace geometry {
class Geometry;
}

namespace dom {
class Root;
class Object;
class Focusable;
}

class Class;

CLASS( UI, gse::GCWrappable )

	UI( GSE_CALLABLE );
	~UI();

	void Iterate();

	WRAPDEFS_PTR( UI );

	scene::Scene* const m_scene = nullptr;
	gc::Space* const m_gc_space = nullptr;
	gse::context::Context* const m_ctx = nullptr;

	const types::mesh::coord_t ClampX( const coord_t& x ) const;
	const types::mesh::coord_t ClampY( const coord_t& y ) const;
	const types::Vec2< types::mesh::coord_t > ClampXY( const types::Vec2< coord_t >& xy ) const;

	ui::Class* const GetClass( const std::string& name ) const;

	const types::Vec2< ssize_t >& GetLastMousePosition() const;

	void Destroy( GSE_CALLABLE );

	void GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) override;

	typedef std::function< void( GSE_CALLABLE ) > gse_func_t;

	void WithGSE( const gse_func_t& f );
	dom::Root* const GetRoot() const;

	void AddFocusable( dom::Focusable* const element );
	void RemoveFocusable( dom::Focusable* const element );
	void Focus( dom::Focusable* const element );
	void Defocus( dom::Focusable* const element );
	void FocusNext();

	typedef std::function< const bool( GSE_CALLABLE, const input::Event& event ) > global_handler_t;
	const size_t AddGlobalHandler( const global_handler_t& global_handler );
	void RemoveGlobalHandler( const size_t handler_id );

private:
	friend class dom::Object;
	typedef std::function< void() > f_iterable_t;
	void AddIterable( const dom::Object* const obj, const f_iterable_t& f );
	void RemoveIterable( const dom::Object* const obj );
	void SetGlobalSingleton( GSE_CALLABLE, dom::Object* const object, const std::function< void() >& f_on_deglobalize = nullptr );
	void RemoveGlobalSingleton( GSE_CALLABLE, dom::Object* const object );

private:

	dom::Root* m_root;

	struct {
		util::Clamper< float > x;
		util::Clamper< float > y;
	} m_clamp = {};

	types::Vec2< ssize_t > m_last_mouse_position = {};

	std::unordered_map< std::string, ui::Class* > m_classes = {};

	void Resize();

	std::unordered_map< const dom::Object*, f_iterable_t > m_iterables = {};

	std::vector< dom::Focusable* > m_focusable_elements = {};
	std::unordered_map< dom::Focusable*, size_t > m_focusable_elements_idx = {};
	dom::Focusable* m_focused_element = nullptr;

	dom::Object* m_global_singleton = nullptr;
	std::function< void() > m_f_global_singleton_on_deglobalize = nullptr;

	size_t m_next_global_handler_id = 1;
	std::map< size_t, global_handler_t > m_global_handlers = {};

private:
	friend class dom::Object;
	gc::Space* const GetGCSpace() const;

};

}
