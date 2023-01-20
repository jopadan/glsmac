#pragma once

/* Clickable button */

#include "Panel.h"
#include "Label.h"

namespace ui {
namespace object {

CHILD_CLASS( Button, Panel )

	Button( const string& class_name = "" );

	virtual void Create();
	virtual void Destroy();
	
	void SetLabel( const string& text );
	const string& GetLabel() const;
	void SetTextAlign( alignment_t alignment );

protected:
	virtual void ApplyStyle();
	
	bool OnMouseOver( const UIEvent::event_data_t* data );
	bool OnMouseOut( const UIEvent::event_data_t* data );
	bool OnMouseDown( const UIEvent::event_data_t* data );
	bool OnMouseUp( const UIEvent::event_data_t* data );
	
	string m_label_text = "";
	alignment_t m_label_alignment = ALIGN_CENTER;
	Label* m_label = nullptr;
	
	bool m_is_clicking = false; // mouseup at different position after mousedown is still counted as click, as long as it's inside button
};

} /* namespace object */
} /* namespace ui */
