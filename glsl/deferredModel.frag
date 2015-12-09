#version 430 core

/*in vec3 vs_WorldNormal;
in vec3 vs_EyeVector;
in vec3 vs_WorldPosition;

uniform vec3 cameraWorldPosition;
uniform vec3 materialColor;
uniform float roughness;
uniform float metalness;

out vec4 fragData[3];

void main(void){
  vec3 normal = normalize(vs_WorldNormal);
  fragData[0] = vec4(materialColor,metalness);
  fragData[1] = vec4(normal,roughness);
  fragData[2] = vec4(vs_WorldPosition,1.0);
}*/

in vec4 texCoords;
in vec4 normal;
in vec4 tangent;
in vec4 bitan;
in vec4 eyevec;
in vec4 lightvec;
in vec4 worldPos;

uniform sampler2D diffuseTex;
uniform vec3 ambient;
uniform vec3 specular;
uniform float shininess;

out vec4 fragData[3];

void main(void){
  vec4 tex = texture2D(diffuseTex,vec2(texCoords.s,1.0-texCoords.t));
  float metalness = 0.0;
  float roughness = 0.5;
  fragData[0] = vec4(tex.rgb,metalness);
  fragData[1] = vec4(normalize(normal.xyz),roughness);
  fragData[2] = vec4(worldPos.xyz,1.0);
}
