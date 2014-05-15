set(sources 
AsteroidRenderer.cpp    glCommon.cpp       RenderBufferObject.cpp
callbacks.cpp           ImageLoader.cpp    Shader.cpp
Camera.cpp              SkyBox.cpp		   VertexArrayObject.cpp
CoreShaders.cpp         MathUtilities.cpp  StarRenderer.cpp
DynamicPatchSphere.cpp  Mesh.cpp           Text.cpp
Model.cpp          Texture.cpp             AssetManager.cpp
FlatDrawable.cpp        noise.cpp          ThreadPool.cpp
FrameBufferObject.cpp   TransformFeedback.cpp	AABB.cpp
Geometry.cpp            Quaternion.cpp     VertexAttribBuilder.cpp)

add_library(gamelib ${sources})
