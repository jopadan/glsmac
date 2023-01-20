#pragma once

#include <vector>

#include "UIObject.h"

#include "ui/event/UIEvent.h"

namespace ui {
namespace object {

class UIContainer : public UIObject {
public:
	UIContainer( const string& class_name = "" );
	
	void AddChild( UIObject *object );
	void RemoveChild( UIObject *object );

	virtual void Create();
	virtual void Destroy();
	virtual void Iterate();
	virtual void Realign();
	virtual void Redraw();

	void UpdateZIndex();
	
	void SetOverflow( const overflow_t overflow );
	void ProcessEvent( event::UIEvent* event );
	
	void AddStyleModifier( const Style::modifier_t modifier );
	void RemoveStyleModifier( const Style::modifier_t modifier );

protected:
	virtual void ApplyStyle();
	virtual void ReloadStyle();
	
	void SetEventContexts( event_context_t contexts );
	void SetOverriddenEventContexts( event_context_t contexts );
	
	std::vector<UIObject *> m_child_objects = {};
	
	const string Subclass( const string& class_name ) const;
	
private:
	void CreateChild( UIObject *object );
	void DestroyChild( UIObject *object );
};

} /* namespace object */
} /* namespace ui */
