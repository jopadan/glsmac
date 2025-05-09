#include "Popup.h"

#include "game/frontend/Game.h"
#include "ui_legacy/object/Surface.h"
#include "game/frontend/ui_legacy/Section.h"

namespace game {
namespace frontend {
namespace ui_legacy {
namespace popup {

Popup::Popup( Game* game )
	: ::ui_legacy::object::Popup( "WP" )
	, m_game( game ) {
	SetAlign( ::ui_legacy::ALIGN_HCENTER | ::ui_legacy::ALIGN_BOTTOM );
}

void Popup::Create() {
	::ui_legacy::object::Popup::Create();

	NEW( m_body, Section, m_game, "", "WP" );
	m_body->m_config.no_outer_border = m_config.no_outer_border;
	m_body->m_config.no_inner_border = m_config.no_inner_border;
	if ( !m_title_text.empty() ) {
		m_body->SetTitleText( m_title_text );
	}
	AddChild( m_body );

#define x( _k, _n ) {\
        NEW( m_side_frames._k, ::ui_legacy::object::Surface, "WP" _n "Frame" ); \
        AddChild( m_side_frames._k ); \
    }
	x( left_left, "LeftLeft" );
	x( left_right, "LeftRight" );
	x( right_right, "RightRight" );
	x( right_left, "RightLeft" );
#undef x

	// slide up
	const auto start_at = m_game->GetBottomBarMiddleHeight() - GetHeight();
	SetBottom( start_at );
	m_slide.Scroll( start_at, start_at + GetHeight(), SLIDE_DURATION_MS );
}

void Popup::Align() {

	// don't let popup top go outside of window
	// TODO: add 'max_height' to styles
	::ui_legacy::object::Popup::SetHeight( std::min< coord_t >( m_original_height, m_game->GetViewportHeight() ) );

	::ui_legacy::object::Popup::Align();
}

void Popup::Iterate() {
	::ui_legacy::object::Popup::Iterate();

	bool has_ticked = false;
	while ( m_slide.HasTicked() ) {
		has_ticked = true;
		SetBottom( m_slide.GetPosition() );
	}
	if ( !has_ticked && m_is_closing ) {
		// slide down finished
		Close();
	}
}

void Popup::Destroy() {
	RemoveChild( m_body );

#define x( _k ) RemoveChild( m_side_frames._k )
	x( left_left );
	x( left_right );
	x( right_right );
	x( right_left );
#undef x

	::ui_legacy::object::Popup::Destroy();
}

void Popup::ProcessEvent( ::ui_legacy::event::UIEvent* event ) {
	if ( !m_slide.IsRunning() ) { // ignore events during slide
		::ui_legacy::object::Popup::ProcessEvent( event );
	}
}

bool Popup::MaybeClose() {
	if ( m_is_closing ) {
		if ( !m_slide.IsRunning() ) { // ready to close if slide down finished
			return ::ui_legacy::object::Popup::MaybeClose();
		}
	}
	else {
		// slide down
		m_is_closing = true;
		m_slide.Scroll( GetBottom(), m_game->GetBottomBarMiddleHeight() - GetHeight(), SLIDE_DURATION_MS );
		PlayCloseSound();
	}
	return false; // not ready to close yet
}

void Popup::SetHeight( const coord_t px ) {
	m_original_height = px;
	::ui_legacy::object::Popup::SetHeight( px );
}

void Popup::SetTitleText( const std::string& title_text ) {
	if ( m_title_text != title_text ) {
		m_title_text = title_text;
		if ( m_body ) {
			m_body->SetTitleText( m_title_text );
		}
	}
}

void Popup::CloseNow() {
	m_slide.Stop();
	m_is_closing = true;
	Close();
}

}
}
}
}
