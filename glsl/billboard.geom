#version 330
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 up[];
in vec3 right[];
out vec2 texCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main()
{
	vec3 worldPos = gl_in[0].gl_Position.xyz;
	texCoord = vec2(0.0,0.0);
	gl_Position = projMatrix * viewMatrix * vec4(worldPos + up[0] - right[0],1.0);
	EmitVertex();
	texCoord = vec2(0.0,1.0);
	gl_Position = projMatrix * viewMatrix * vec4(worldPos - up[0] - right[0],1.0);
	EmitVertex();
	texCoord = vec2(1.0,0.0);
	gl_Position = projMatrix * viewMatrix * vec4(worldPos + up[0] + right[0],1.0);
	EmitVertex();
	texCoord = vec2(1.0);
	gl_Position = projMatrix * viewMatrix * vec4(worldPos - up[0] + right[0],1.0);
	EmitVertex();
	EndPrimitive();
}
