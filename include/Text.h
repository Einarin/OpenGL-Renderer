#pragma once

#include "glincludes.h"
#include "Shader.h"
#include <string>
#include <cstdlib>

//freetype-gl causes massive namespace pollution so we keep it out of the rest of the program
// we define new types here because C++ sucks >.>
extern "C" {
struct vertex_buffer_type;
struct texture_font_type;
struct texture_atlas_type;
struct font_manager_type;
}

namespace gl {
	class TextRendererMembers;
	class TextManagerMembers;
	class TextRenderer{
		friend class TextManager;
	protected:
		struct vertex_t {
			float x, y, z;
			float u, v;
			float r, g, b, a;
			float shift, gamma;
		};
		std::string text;
		vertex_buffer_type* buffer;
		texture_font_type * font;
		texture_atlas_type *atlas;
		std::shared_ptr<Shader> shader;
		TextRenderer(std::shared_ptr<Shader> shdr, texture_font_type * fnt, vertex_buffer_type* vbuf,texture_atlas_type *atls) : font(fnt),buffer(vbuf),shader(shdr),atlas(atls)
		{}
	public:
		~TextRenderer();
		glm::vec2 addText(std::string text,glm::vec2 pos,glm::vec4 color);
		void clearText();
		void loadAscii();
		void draw(glm::mat4 ortho);
	};

	class TextManager{
	protected:
		font_manager_type * manager;
		std::shared_ptr<Shader> shader;
		void buildShader(){
			std::shared_ptr<ShaderStage> vs = ShaderStage::Allocate(GL_VERTEX_SHADER);
			if(!vs->compile(
				"uniform mat4 projection;\n"
				"attribute vec3 vertex;\n"
				"attribute vec2 tex_coord;\n"
				"attribute vec4 color;\n"
				"void main()\n"
				"{\n"
					"gl_TexCoord[0].xy = tex_coord.xy;\n"
					"gl_FrontColor = color;\n"
					"gl_Position = projection * vec4(vertex,1.0);\n"
				"}\n")) DebugBreak();
			std::shared_ptr<ShaderStage> fs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
			if(!fs->compile(
				"uniform sampler2D texture;\n"
				"void main()\n"
				"{\n"
				"float a = texture2D(texture, gl_TexCoord[0].xy).a;\n"
				"gl_FragColor = vec4(gl_Color.rgb, gl_Color.a*a);\n"
				"}\n")) DebugBreak();
			shader->addAttrib("vertex",0);
			shader->addAttrib("tex_coord",1);
			shader->addAttrib("color",2);
			shader->attachStage(vs);
			shader->attachStage(fs);
			shader->link();
		}
	public:
		TextManager();
		~TextManager();
		void init()
		{
			buildShader();
		}
		TextRenderer * getTextRenderer(std::string font, float size);

	};

} //namespace gl
