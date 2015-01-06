#version 420
layout(location = 0) in vec3 position;
layout(location = 2) in vec3 texCoord;
uniform sampler2D terrain;
uniform mat4 patchMatrix;

void main(void){
    vec2 texcoord = position.xy;
    /*float height = 1.0f;//texture(terrain, texcoord).a;
    vec4 displaced = vec4(
        position.x, position.y,
        position.z, 1.0);*/
    gl_Position = patchMatrix * vec4(position,1.0);//displaced;
}