#version 430 core

in vec3 vs_WorldNormal;
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
}
