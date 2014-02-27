#version 330
in vec3 in_Position;
in vec3 in_Normal;
in vec3 in_Tangent;
in vec3 in_TexCoords;
out vec4 texCoords;
out vec4 normal;
out vec4 tangent;
out vec4 eyevec;
out vec4 lightvec;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec4 camera;
uniform vec4 light;

void main()
{
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	normal = vec4(normalMatrix * in_Normal,1.0);
	tangent = vec4(normalMatrix * in_Tangent,1.0);
	texCoords = vec4(in_Position,1.0);
	vec4 position = modelMatrix * vec4(in_Position.xyz,1.0);
	eyevec = vec4(camera.xyz - position.xyz,1.0);
	lightvec = vec4(light.xyz + position.xyz,1.0);
	gl_Position = projectionMatrix * viewMatrix * position;
}