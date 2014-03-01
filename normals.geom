#version 330
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;
//layout(triangle_strip, max_vertices = 3) out;

in vec4 normal[];
uniform float normalLength;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main()
{
	for(int i=0; i < 3; i++)
	{
		gl_Position = projMatrix * viewMatrix * modelMatrix * gl_in[i].gl_Position;
		EmitVertex();
		gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(gl_in[i].gl_Position.xyz + normalLength * normal[i].xyz,1.0);
		EmitVertex();
		EndPrimitive();
	}
	//EndPrimitive();
}