#version 330
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec4 vs_texCoords[];
in vec4 vs_normal[];
in vec4 vs_tangent[];
in vec4 vs_bitan[];
in vec4 vs_eyevec[];
in vec4 vs_lightvec[];

smooth out vec4 texCoords;
smooth out vec4 normal;
smooth out vec4 tangent;
smooth out vec4 bitan;
smooth out vec4 eyevec;
smooth out vec4 lightvec;

void main()
{
	vec3 face_normal = normalize (
            cross (gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz,
                gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz));
				
	for(int i=0; i < 3; i++)
	{
		normal = vec4(face_normal,1.0);
		tangent = vs_tangent[i];
		bitan = vs_bitan[i];
		texCoords = vs_texCoords[i];
		eyevec = vs_eyevec[i];
		lightvec = vs_lightvec[i];
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}