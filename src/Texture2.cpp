#include "Texture2.h"
#include "Shader.h"
#include "Geometry.h"
#include "ThreadPool.h"
#include <iostream>

namespace gl {

	void textureBindingDebugCheck(GLenum target, GLuint id, const char* file, int line) {
		GLint bound_id;
		if (id == 0) {
			std::cout << "TEXTURE NOT ALLOCATED IN " << file << ":" << line << "\n";
		}
		GLenum binding = 0;
		switch(target){
		case GL_TEXTURE_2D:
			binding = GL_TEXTURE_BINDING_2D;
			break;
		case GL_TEXTURE_CUBE_MAP:
			binding = GL_TEXTURE_BINDING_CUBE_MAP;
			break;
		default:
			std::cout << "FFS WHAT IS THIS: " << target << "\n";
			binding = 0;
		}
		glGetIntegerv(binding, &bound_id);
		if (id != bound_id) {
				std::cout << "TEXTURE NOT BOUND IN " << file << ":" << line << "\n";
		}
	}

	void Texture2D::draw(){
		static int setup = 0;
		static Billboard bb;
		static ShaderRef shader;
		if (setup){
			shader->bind();
		}
		else {
			shader = Shader::Allocate();
			auto vs = ShaderStage::Allocate(GL_VERTEX_SHADER);
			auto fs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
			vs->compile("#version 330\n"
				"in vec2 position;\n"
				"out vec2 texCoord;\n"
				"void main(void){\n"
				"texCoord = 0.5*(position+1.0);\n"
				"gl_Position = vec4(position,0.0,1.0);\n"
				"}\n");
			fs->compile("#version 330\n"
				"in vec2 texCoord;\n"
				"out vec4 FragColor;\n"
				"uniform sampler2D tex;\n"
				"void main(void){\n"
				"vec4 frag = texture(tex,texCoord);\n"
				"FragColor = vec4(frag.rgb,0.5);\n"
				"}\n");
			shader->attachStage(vs);
			shader->attachStage(fs);
			shader->link();
			shader->bind();
			glUniform1i(shader->getUniformLocation("tex"), 0);
			bb.init();
			bb.download();
			setup = 1;
		}
		glActiveTexture(GL_TEXTURE0);
		bind();
		bb.draw();
	}

} //namespace gl