#version 330
in vec3 in_Position;
in vec3 in_Normal;
in vec3 in_Tangent;
in vec3 in_TexCoords;
in int boneid;
in float boneWeight;
out vec4 texCoords;
out vec4 normal;
out vec4 tangent;
out vec4 bitan;
out vec4 eyevec;
out vec4 lightvec;
out vec4 worldPos;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec4 camera;
uniform vec4 light;
uniform mat4 bones[10];

void main(void)
{
	mat4 boneMat = bones[boneid];
	mat4 localMat = modelMatrix * boneMat;
	normal = vec4(normalize(transpose(inverse(mat3(localMat))) * in_Normal.xyz),1.0);
	tangent = vec4(normalize(transpose(inverse(mat3(localMat))) * in_Tangent.xyz),1.0);
	bitan = vec4(cross(normal.xyz,tangent.xyz),1.0);
	texCoords = vec4(in_TexCoords.xyz,1.0);
	vec4 position = localMat * vec4(in_Position.xyz,1.0);
	worldPos = position;
	//position += normal * crater(in_TexCoords);
	eyevec = camera - position;
	lightvec = light + position;
	gl_Position = projMatrix * viewMatrix * position;
}
