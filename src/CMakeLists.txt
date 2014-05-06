set(sources 
AsteroidRenderer.cpp    glCommon.cpp       RenderBufferObject.cpp
callbacks.cpp           ImageLoader.cpp    Shader.cpp
Camera.cpp              main.cpp           SkyBox.cpp
CoreShaders.cpp         MathUtilities.cpp  StarRenderer.cpp
DynamicPatchSphere.cpp  Mesh.cpp           Text.cpp
entrypoint.cpp          Model.cpp          Texture.cpp
FlatDrawable.cpp        noise.cpp          ThreadPool.cpp
FrameBufferObject.cpp   TransformFeedback.cpp
Geometry.cpp            Quaternion.cpp     VertexAttribBuilder.cpp)

add_executable(game ${sources})
