#version 330
in vec3 in_Position;
in vec3 in_Normal;
out vec4 normal;

void main(void)
{
	normal = vec4(normalize(in_Normal),0.0);
	gl_Position = vec4(in_Position.xyz,1.0);
}