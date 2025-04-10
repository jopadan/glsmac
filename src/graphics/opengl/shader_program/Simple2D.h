#pragma once

#include "ShaderProgram.h"

namespace graphics {
namespace opengl {

class Mesh;
class Text;
class FBO;
class Cache;

namespace shader_program {

CLASS( Simple2D, ShaderProgram )
	Simple2D()
		: ShaderProgram( TYPE_SIMPLE2D ) {};
protected:
	friend class opengl::Mesh;
	friend class opengl::Text;
	friend class opengl::FBO;
	friend class opengl::Cache;

	struct {
		GLuint flags;
		GLuint tint_color;
		GLuint position;
		GLuint texture;
		struct {
			GLuint min;
			GLuint max;
		} area_limits;
	} uniforms;

	struct {
		GLuint coord;
		GLuint tex_coord;
	} attributes;

	void AddShaders() override;
	void Initialize() override;
	void EnableAttributes() const override;
	void DisableAttributes() const override;
};

}
}
}
