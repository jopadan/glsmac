#pragma once

#include "Brush.h"

namespace game {
namespace world {
	class World;
namespace map_editor {
namespace brush {

CLASS( Dot, Brush )
	
	Dot( World* world );
	
	const MapEditor::tiles_t Draw( map::Tile* center_tile );
	
};
	
}
}
}
}
