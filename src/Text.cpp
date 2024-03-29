#include "Text.h"
#include "../freetype-gl/freetype-gl.h"
#include "../freetype-gl/font-manager.h"
#include "../freetype-gl/vertex-buffer.h"
#include <glm/gtc/type_ptr.hpp>

namespace gl{

TextRenderer::~TextRenderer()
{
	vertex_buffer_delete((vertex_buffer_t*)buffer);
}

void TextRenderer::loadAscii(){
	wchar_t wstr[128];
	for(int i=0;i<128;i++){
		wstr[i] = (wchar_t)i;
	}
	texture_font_load_glyphs((texture_font_t*)font,wstr);
}

glm::vec2 TextRenderer::addText(std::string text,glm::vec2 pos,glm::vec4 color){
	if (vao = 0) {
		glGenVertexArrays(1, &vao);
	}
	glBindVertexArray(vao);
	wchar_t* wstr = new wchar_t[text.size()];
	mbstowcs(wstr,text.c_str(),text.size());
	//texture_font_load_glyphs((texture_font_t*)font,wstr);
	for(int i=0;i<(int)text.size();i++){
		texture_glyph_t *glyph = texture_font_get_glyph( (texture_font_t*)font, wstr[i] );
		if(glyph != NULL){
			float kerning = 0.f;
			if( i > 0)
			{
				kerning = texture_glyph_get_kerning( glyph, wstr[i-1] );
			}
			pos.x += kerning;
			float x0  = (int)( pos.x + glyph->offset_x );
			float y0  = (int)( pos.y + glyph->offset_y );
			float x1  = (int)( x0 + glyph->width );
			float y1  = (int)( y0 - glyph->height );
			float s0 = glyph->s0;
			float t0 = glyph->t0;
			float s1 = glyph->s1;
			float t1 = glyph->t1;
			GLuint index = ((vertex_buffer_t*)buffer)->vertices->size;
			GLuint indices[] = {index, index+1, index+2,
								index, index+2, index+3};
			vertex_t vertices[] = { { x0,y0,0.f,  s0,t0,  color.r,color.g,color.b,color.a, 0.f,1.f },
									{ x0,y1,0.f,  s0,t1,  color.r,color.g,color.b,color.a, 0.f,1.f },
									{ x1,y1,0.f,  s1,t1,  color.r,color.g,color.b,color.a, 0.f,1.f },
									{ x1,y0,0.f,  s1,t0,  color.r,color.g,color.b,color.a, 0.f,1.f } };
			vertex_buffer_push_back_indices( (vertex_buffer_t*)buffer, indices, 6 );
			vertex_buffer_push_back_vertices( (vertex_buffer_t*)buffer, vertices, 4 );
			pos.x += glyph->advance_x;
		}
	}
	delete[] wstr;
	glBindVertexArray(0);
	return pos;
}

void TextRenderer::clearText(){
	if (vao = 0) {
		glGenVertexArrays(1, &vao);
	}
	glBindVertexArray(vao);
	vertex_buffer_clear((vertex_buffer_t*)buffer);
	glBindVertexArray(0);
}

void TextRenderer::draw(glm::mat4 ortho){
	if (vao = 0) {
		glGenVertexArrays(1, &vao);
	}
	glBindVertexArray(vao);
	shader->bind();
	checkGlError("bind text shader");
	glUniformMatrix4fv(shader->getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(ortho));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture( GL_TEXTURE_2D, ((texture_atlas_t*)atlas)->id );
	checkGlError("draw text atlas");
	vertex_buffer_render( (vertex_buffer_t*)buffer, GL_TRIANGLES );
	checkGlError("draw text buffer");
	glBindVertexArray(0);
}

TextManager::TextManager() : manager((font_manager_type*)font_manager_new( 512, 512, 1 )),
							shader(Shader::Allocate())
{}

TextManager::~TextManager(){
	font_manager_delete((font_manager_t*)manager);
}

TextRenderer * TextManager::getTextRenderer(std::string fontfile, float size){
	texture_font_t * font = font_manager_get_from_filename((font_manager_t*)manager,fontfile.c_str(),size);
	vertex_buffer_t * buffer = vertex_buffer_new("vertex:3f,tex_coord:2f,color:4f,ashift:1f,agamma:1f");
	texture_atlas_t *atlas = ((font_manager_t*)manager)->atlas;
	return new TextRenderer(shader,(texture_font_type*)font,
		(vertex_buffer_type*)buffer,(texture_atlas_type*)atlas);
}

}//namespace gl
