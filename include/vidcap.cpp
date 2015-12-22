#include "vidcap.h"
#include "escapi.h"
#include <iostream>

using namespace glm;

bool VidCap::init(int width, int height)
{
	int camCount = setupESCAPI();
	if (camCount == 0) {
		std::cout << "No Video Sources Found!\n";
	}
	m_params = { new int[width*height],width,height };
	if (!initCapture(0, (SimpleCapParams*)&m_params)) {
		std::cout << "Couldn't initialize capture!\n";
		return false;
	}
	doCapture(0);
	m_tex = gl::Texture2D::backedTexture(GL_BGRA, GL_SRGB8_ALPHA8, glm::ivec2(width, height), GL_UNSIGNED_BYTE);
	m_ready = true;

	m_shader = gl::Shader::Allocate();
	auto vs = gl::ShaderStage::Allocate(GL_VERTEX_SHADER);
	auto fs = gl::ShaderStage::Allocate(GL_FRAGMENT_SHADER);
	vs->compile("#version 330\n"
		"in vec2 position;\n"
		"out vec2 texCoord;\n"
		"void main(void){\n"
		"texCoord = 0.5*(position+1.0);\n"
		"texCoord.y = 1.0-texCoord.y;\n" //flip y axis so video is upright
		"gl_Position = vec4(position,0.0,1.0);\n"
		"}\n");
	fs->compile(SHADER(#version 330\n
		in vec2 texCoord;
		out vec4 FragColor;
		uniform sampler2D tex;
		uniform vec2 texStep;
		vec3 rgb2hsv(vec3 color) {
			mat3 transform = mat3(
				0.299,0.587,0.114,
				-0.16836,-0.331264,0.5,
				0.5,-0.418688,-0.081312
			);
			return color * transform;
		}
		vec3 hsv2rgb(vec3 hsv) {
			mat3 transform = mat3(
				1.0,0.0,1.4,
				1.0,-0.343,-0.711,
				1.0,1.765,0.0
			);
			return hsv * transform;
		}
		void main() {
			vec4 frag = texture(tex,texCoord);
			vec3 hsv = rgb2hsv(frag.rgb);
			vec3 left = rgb2hsv(texture(tex, texCoord - vec2(texStep.x, 0.0)).rgb);
			vec3 right = rgb2hsv(texture(tex, texCoord + vec2(texStep.x, 0.0)).rgb);
			vec3 up = rgb2hsv(texture(tex, texCoord - vec2(0.0, texStep.y)).rgb);
			vec3 down = rgb2hsv(texture(tex, texCoord + vec2(0.0,texStep.y)).rgb);
			float edge = abs(hsv.x - left.x)
				+ abs(hsv.x - right.x)
				+ abs(hsv.x - up.x)
				+ abs(hsv.x - down.x);
			edge = 5.0*max(edge-0.03, 0.0);
			float lum = hsv.x + edge;
			vec3 color = hsv2rgb(vec3(lum,hsv.yz));//0.7 * frag.rgb + 0.3 * hsv2rgb(vec3(edge));
			FragColor = vec4(color, 1.0);
		}\n));
	m_shader->attachStage(vs);
	m_shader->attachStage(fs);
	m_shader->link();
	m_shader->bind();
	glUniform1i(m_shader->getUniformLocation("tex"), 0);
	glUniform2f(m_shader->getUniformLocation("texStep"), 1.f/width, 1.f/height);
	m_bb.init();
	m_bb.download();
	return true;
}

gl::Tex2D VidCap::getAndBindTex() {
	if (m_ready) {
		if (isCaptureDone(0)) {
			m_tex->bind();
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_params.mWidth, m_params.mHeight, GL_BGRA, GL_UNSIGNED_BYTE, m_params.mTargetBuf);
			doCapture(0);
		}
		else {
			m_tex->bind();
		}
	}
	return m_tex;
}

void VidCap::draw()
{
	if (!m_ready) {
		return;
	}
	m_shader->bind();
	glActiveTexture(GL_TEXTURE0);
	getAndBindTex();
	m_bb.draw();
}
