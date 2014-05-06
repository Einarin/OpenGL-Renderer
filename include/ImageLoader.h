#pragma once
#include <string>
#include "glm/glm.hpp"

namespace gl{

class GlTexture2D;

bool loadPngToGlTex2D(std::string filepath, gl::GlTexture2D* tex);
bool loadGlTex2dFromPngBytestream(gl::GlTexture2D* tex, char* bytes, int length);
bool imageDataFromPngFile(std::string filepath, glm::ivec2* size, char** data, int* bufflen);
}