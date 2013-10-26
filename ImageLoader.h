#pragma once
#include <string>

namespace gl{

class GlTexture2D;

bool loadPngToGlTex2D(std::string filepath, gl::GlTexture2D* tex);
bool loadGlTex2dFromPngBytestream(gl::GlTexture2D* tex, char* bytes, int length);

}