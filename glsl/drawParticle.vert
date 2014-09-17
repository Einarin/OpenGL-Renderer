// Vertex Shader – file "minimal.vert"
#version 330

in  vec3 in_Position;
in  vec3 in_Velocity;
in  vec4 in_Color;
//out vec4 ex_Color;
out vec3 up;
out vec3 right;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec3 cameraPosition;

void main(void)
{
	//ex_Color.rgba = in_Color.rgba;
	//gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0);
	vec4 worldPos = modelMatrix * vec4(in_Position, 1.0);
	vec3 look = normalize(cameraPosition-worldPos.xyz);
	right = cross(vec3(0.,1.,0.),look);
	up = cross(look, right);
	//scale = modelMatrix * vec4(1.0);
	gl_Position = worldPos;
}
