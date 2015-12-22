#pragma once
#include "glincludes.h"
#include "Texture2.h"
#include "Shader.h"
#include "Geometry.h"

class VidCap {
protected:
	struct CapParams {
		int * mTargetBuf;
		int mWidth;
		int mHeight;
	};
	CapParams m_params;
	gl::Tex2D m_tex;
	gl::ShaderRef m_shader;
	gl::Billboard m_bb;
	bool m_ready = false;
public:
	VidCap() {
		m_params = { nullptr, 0, 0 };
	}
	~VidCap() {
		delete[] m_params.mTargetBuf;
	}
	bool init(int width, int height);
	gl::Tex2D getAndBindTex();
	int* getBuff() {
		return m_params.mTargetBuf;
	}
	void draw();
};