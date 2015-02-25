#version 330
layout(triangles) in;
layout(line_strip, max_vertices = 18) out;
//layout(triangle_strip, max_vertices = 3) out;
in vec4 normal[];
in vec4 tangent[];
in vec4 bitan[];
out vec4 color;
uniform float normalLength;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec4 light;

void main()
{
	mat4 iproj = inverse(projMatrix * viewMatrix);
	for(int i=0; i < 3; i++)
	{
		vec4 worldpos = iproj * gl_in[i].gl_Position;
		//Normal - Blue
		color = vec4(0.0,0.0,1.0,1.0);
		gl_Position = gl_in[i].gl_Position;
		//gl_Position = projMatrix * viewMatrix * worldpos;//worldPos[i];
		EmitVertex();
		gl_Position = projMatrix * viewMatrix * (worldpos + normalLength * vec4(normal[i].xyz,0.));
		EmitVertex();
		EndPrimitive();
		//Tangent - Red
		color = vec4(1.0,0.0,0.0,1.0);
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
		gl_Position = projMatrix * viewMatrix * (worldpos + normalLength * vec4(tangent[i].xyz,0.));
		EmitVertex();
		EndPrimitive();
		//Bitangent - Green
		color = vec4(0.0,1.0,0.0,1.0);
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
		gl_Position = projMatrix * viewMatrix * (worldpos + normalLength * vec4(bitan[i].xyz,0.));
		EmitVertex();
		EndPrimitive();
	}
	//EndPrimitive();
}