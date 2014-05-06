#version 330
in vec3 in_Position;
in vec3 in_Normal;
in vec3 in_Tangent;
in vec3 in_TexCoords;
out vec4 texCoords;
out vec4 normal;
out vec4 tangent;
out vec4 bitan;
out vec4 eyevec;
out vec4 lightvec;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec4 camera;
uniform vec4 light;

void main(void)
{
	normal = vec4(normalize(transpose(inverse(mat3(modelMatrix))) * in_Normal.xyz),1.0);
	tangent = vec4(normalize(transpose(inverse(mat3(modelMatrix))) * in_Tangent.xyz),1.0);
	bitan = vec4(cross(normal.xyz,tangent.xyz),1.0);
	texCoords = vec4(in_TexCoords.xyz,1.0);
	vec4 position = modelMatrix * vec4(in_Position.xyz,1.0);
	//position += normal * crater(in_TexCoords);
	eyevec = camera - position;
	lightvec = light + position;
	gl_Position = projMatrix * viewMatrix * position;
}