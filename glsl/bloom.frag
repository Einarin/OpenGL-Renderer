#version 330
in vec2 texCoord;
out vec4 FragColor;
uniform sampler2D tex;

void main(void){
	//vec4 frag = texture(tex,texCoord);
	vec3 bloom = vec3(0.0);
	for(int x=1;x<4;x++){
		for(int y=1;y<4;y++){
			vec2 dC = vec2(float(x)/1024.0,float(y)/1024.0);
			vec4 frag = texture(tex,texCoord+dC);
			bloom = max(frag.rgb-1.0,bloom);
			frag = texture(tex,texCoord-dC);
			bloom = max(frag.rgb-1.0,bloom);
		}
	}
	FragColor = vec4(bloom,1.0);
}