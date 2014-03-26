#version 330
in vec3 in_Position;
in vec3 in_Normal;
in vec3 in_Tangent;
in vec3 in_TexCoords;
out vec4 texCoords;
out vec4 normal;
out vec4 eyevec;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec4 camera;

void main(void)
{
	normal = vec4(normalize(transpose(inverse(mat3(modelMatrix))) * in_Normal),1.0);
	texCoords = vec4(in_TexCoords,1.0);
	vec4 position = modelMatrix * vec4(in_Position,1.0);
	eyevec = camera - position;
	gl_Position = projMatrix * viewMatrix * position;
}