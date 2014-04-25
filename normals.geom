#version 330
layout(triangles) in;
layout(line_strip, max_vertices = 18) out;
//layout(triangle_strip, max_vertices = 3) out;

in vec3 normal[];
in vec3 tangent[];
in vec3 bitan[];
out vec4 color;
uniform float normalLength;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main()
{
	for(int i=0; i < 3; i++)
	{
		//Normal - Blue
		color = vec4(0.0,0.0,1.0,1.0);
		gl_Position = projMatrix * viewMatrix * modelMatrix * gl_in[i].gl_Position;
		EmitVertex();
		gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(gl_in[i].gl_Position.xyz + normalLength * normal[i],1.0);
		EmitVertex();
		EndPrimitive();
		//Tangent - Red
		color = vec4(1.0,0.0,0.0,1.0);
		gl_Position = projMatrix * viewMatrix * modelMatrix * gl_in[i].gl_Position;
		EmitVertex();
		gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(gl_in[i].gl_Position.xyz + normalLength * tangent[i],1.0);
		EmitVertex();
		EndPrimitive();
		//Bitangent - Green
		color = vec4(0.0,1.0,0.0,1.0);
		gl_Position = projMatrix * viewMatrix * modelMatrix * gl_in[i].gl_Position;
		EmitVertex();
		gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(gl_in[i].gl_Position.xyz + normalLength * bitan[i],1.0);
		EmitVertex();
		EndPrimitive();
	}
	//EndPrimitive();
}