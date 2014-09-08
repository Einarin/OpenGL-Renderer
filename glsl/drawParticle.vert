// Vertex Shader – file "minimal.vert"

#version 130

in  vec3 in_Position;
in  vec3 in_Velocity;
in  vec4 in_Color;
out vec4 ex_Color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main(void)
{
	ex_Color.rgba = in_Color.rgba;
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0);
}
