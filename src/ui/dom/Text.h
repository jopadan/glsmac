#pragma once

#include "Drawable.h"

namespace scene::actor {
class Text;
}

namespace ui {

namespace geometry {
class Text;
}

namespace dom {

class Text : public Drawable {
public:
	Text( DOM_ARGS );

	void SetText( const std::string& text );

protected:
	virtual ~Text();

private:
	std::string m_text = "";
	scene::actor::Text* m_actor = nullptr;
	std::string m_fontname = "";
	uint16_t m_fontsize = 32;

	void UpdateFont();
};

}
};
