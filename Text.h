#pragma once
#include "freetype-gl\freetype-gl.h"
#include "freetype-gl\font-manager.h"
#include "freetype-gl\vertex-buffer.h"
#include "glincludes.h"
#include <vector>
#include <string>
#include <cstdlib>

namespace gl {

	class TextRenderable{
		friend TextManager;
	protected:
		struct vertex_t {
			float x, y, z;
			float u, v;
			float r, g, b, a;
			float shift, gamma;
		};
		std::string text;
		vertex_buffer_t* buffer;
		texture_font_t * font;
		TextRenderable(texture_font_t * fnt, vertex_buffer_t* vbuf) : font(fnt),buffer(vbuf)
		{}
	public:
		~TextRenderable()
		{
			vertex_buffer_delete(buffer);
		}
		glm::vec2 addText(std::string text,glm::vec2 pos,glm::vec4 color){
			wchar_t* wstr = new wchar_t[text.size()];
			mbstowcs(wstr,text.c_str(),text.size());
			texture_font_load_glyphs(font,wstr);
			for(int i=0;i<(int)text.size();i++){
				texture_glyph_t *glyph = texture_font_get_glyph( font, text[i] );
				if(glyph != NULL){
					int kerning = 0;
					if( i > 0)
					{
						kerning = texture_glyph_get_kerning( glyph, text[i-1] );
					}
					pos.x += kerning;
					int x0  = (int)( pos.x + glyph->offset_x );
					int y0  = (int)( pos.y + glyph->offset_y );
					int x1  = (int)( x0 + glyph->width );
					int y1  = (int)( y0 - glyph->height );
					float s0 = glyph->s0;
					float t0 = glyph->t0;
					float s1 = glyph->s1;
					float t1 = glyph->t1;
					GLuint index = buffer->vertices->size;
					GLuint indices[] = {index, index+1, index+2,
										index, index+2, index+3};
					vertex_t vertices[] = { { x0,y0,0,  s0,t0,  color.r,color.g,color.b,color.a, 0,1 },
											{ x0,y1,0,  s0,t1,  color.r,color.g,color.b,color.a, 0,1 },
											{ x1,y1,0,  s1,t1,  color.r,color.g,color.b,color.a, 0,1 },
											{ x1,y0,0,  s1,t0,  color.r,color.g,color.b,color.a, 0,1 } };
					vertex_buffer_push_back_indices( buffer, indices, 6 );
					vertex_buffer_push_back_vertices( buffer, vertices, 4 );
					pos.x += glyph->advance_x;
				}
			}
			return pos;
		}
		void draw(){

		}
	};

	class TextManager{
	protected:
		font_manager_t * manager;
		std::vector<texture_font_t> fonts;
	public:
		TextManager() : manager(font_manager_new( 512, 512, 3 ))
		{}
		TextRenderable * getTextRenderable(std::string font, float size){
			texture_font_t * font = font_manager_get_from_filename(manager,font.c_str(),size);
			vertex_buffer_t * buffer = vertex_buffer_new("vertex:3f,tex_coord:2f,color:4f,ashift:1f,agamma:1f");
			return new TextRenderable(font,buffer);
		}

	};

} //namespace gl