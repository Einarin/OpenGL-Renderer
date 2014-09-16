// Fragment Shader – file "minimal.frag"

#version 330 core

//precision highp float; // needed only for version 1.30

//uniform sampler2D framedata;
//uniform vec2 framesize;
//uniform int rendermode;

in  vec4 ex_Color;
out vec4 out_Color;

void main(void)
{
	//float intensity;
	
	//vec2 texCoord = vec2(ex_Color.s,(ex_Color.t/ex_Color.q));

	//intensity = texture2D(framedata,texCoord).r;\

	//if(abs(intensity-0.5) < 0.15) discard;
	//intensity += 0.1;
	//intensity = clamp((intensity*1.11111111)-0.1,0.0,1.0);
        //vec3 color = vec3(intensity);//vec3(intensity * intensity) * vec3(0.95,1.0,1.0);
        //vec3 color = clamp(2.0*vec3(intensity-0.5,pow(abs(intensity-0.5),3),pow(0.5-intensity*0.5,2)),0.0,1.0);
	out_Color = vec4(ex_Color);
}
