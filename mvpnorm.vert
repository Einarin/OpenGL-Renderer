#version 330
in vec3 in_Position;
in vec3 in_Normal;
in vec3 in_Tangent;
out vec3 normal;
out vec3 tangent;
out vec3 bitan;

void main(void)
{
	normal = normalize(in_Normal);
	tangent = normalize(in_Tangent);
	bitan = cross(normal,tangent);
	gl_Position = vec4(in_Position.xyz,1.0);
}