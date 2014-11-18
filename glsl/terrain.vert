#version 420
in vec4 position;
in vec4 texCoord;
uniform sampler2D terrain;

void main(void){
    vec2 texcoord = position.xy;
    float height = 1.0f;//texture(terrain, texcoord).a;
    vec4 displaced = vec4(
        position.x, position.y,
        height, 1.0);
    gl_Position = displaced;
}