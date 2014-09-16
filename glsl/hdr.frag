#version 330
in vec2 texCoord;
out vec4 FragColor;
uniform sampler2D tex;

void main(void){
	vec4 frag = texture(tex,texCoord);
	
	vec3 toneMapped = frag.rgb/(frag.rgb+1.0);
	
	vec3 gammaCorrect = pow(toneMapped,vec3(1.0/2.2));
	
	FragColor = vec4(gammaCorrect,1.0);
}