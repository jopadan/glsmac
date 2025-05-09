#include "BottomBarBase.h"

#include "engine/Engine.h"

#include "types/texture/Texture.h"
#include "ui_legacy/object/Surface.h"
#include "ui_legacy/object/Label.h"
#include "ui_legacy/UI.h"
#include "graphics/Graphics.h"
#include "game/frontend/ui_legacy/popup/base_popup/BasePopup.h"

namespace game {
namespace frontend {
namespace ui_legacy {

BottomBarBase::BottomBarBase( Game* game )
	: UI( game, "BB" ) {
	//
}

void BottomBarBase::Create() {
	UI::Create();

	// frames

	NEW( m_frames.left, ::ui_legacy::object::Surface, "BBFrameLeft" );
	m_frames.left->SetZIndex( 0.4f ); // needed because it needs to be above its background because in pcx there's transparent line around one of borders
	AddChild( m_frames.left );

	NEW( m_frames.middle, ::ui_legacy::object::Surface, "BBFrameMiddle" );
	AddChild( m_frames.middle );

	NEW( m_frames.right, ::ui_legacy::object::Surface, "BBFrameRight" );
	m_frames.right->SetZIndex( 0.3f ); // same as above (but needs to go under left panel if interface is shrunk)
	AddChild( m_frames.right );

	// background

	struct bg_coords_t {
		const ::ui_legacy::alignment_t alignment;
		const types::Vec2< size_t > position;
		const types::Vec2< size_t > size;
		const float zindex; // needed in case game window is shrunk to the point where frame starts to overlap other side
	};
	const std::vector< bg_coords_t > bg_coords = {
		{ ::ui_legacy::ALIGN_TOP | ::ui_legacy::ALIGN_LEFT,  { 4,   57 }, { 241, 196 }, 0.35f }, // left side part
		{ ::ui_legacy::ALIGN_TOP | ::ui_legacy::ALIGN_RIGHT, { 4,   57 }, { 241, 196 }, 0.25f }, // right side part
		{ ::ui_legacy::ALIGN_BOTTOM,                         { 245, 5 },  { 0,   62 },  0.2f }, // bottom part
		{ ::ui_legacy::ALIGN_TOP,                            { 261, 67 }, { 0,   105 }, 0.2f }, // middle part
	};
	for ( auto& b : bg_coords ) {
		NEWV( background, ::ui_legacy::object::Surface, "BBFrameBackground" );
		background->SetAlign( b.alignment );
		if ( b.alignment & ::ui_legacy::ALIGN_LEFT ) {
			background->SetLeft( b.position.x );
		}
		else if ( b.alignment & ::ui_legacy::ALIGN_RIGHT ) {
			background->SetRight( b.position.x );
		}
		if ( b.alignment & ::ui_legacy::ALIGN_TOP ) {
			background->SetTop( b.position.y );
		}
		else if ( b.alignment & ::ui_legacy::ALIGN_BOTTOM ) {
			background->SetBottom( b.position.y );
		}
		if ( b.size.x ) {
			background->SetWidth( b.size.x );
		}
		else {
			background->SetLeft( b.position.x );
			background->SetRight( b.position.x );
		}
		background->SetHeight( b.size.y );
		background->SetZIndex( b.zindex );
		m_backgrounds.push_back( background );
		AddChild( background );
	}
}

void BottomBarBase::Destroy() {

	for ( auto& b : m_backgrounds ) {
		RemoveChild( b );
	}

	RemoveChild( m_frames.left );
	RemoveChild( m_frames.middle );
	RemoveChild( m_frames.right );

	UI::Destroy();
}

}
}
}
