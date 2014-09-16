#version 330
uniform sampler2D diffuseTex;
uniform vec3 ambient;
uniform vec3 specular;
uniform float shininess;

in vec4 texCoords;
in vec4 normal;
in vec4 tangent;
in vec4 bitan;
in vec4 eyevec;
in vec4 lightvec;
out vec4 FragColor;

void main( void )
{	
	vec3 norm = normalize(normal.xyz);
	vec3 tang = normalize(tangent.xyz);
	vec3 eye = normalize(eyevec.xyz);
	vec3 light = normalize(lightvec.xyz);
  //vec3 ambient = vec3(0.1);
  vec4 tex = texture2D(diffuseTex,vec2(texCoords.s,1.0-texCoords.t));
  vec3 diffuse = tex.rgb;
  //vec3 specular = vec3(0.8);
  float diff = max(-dot(light,norm),0);
  vec3 reflect = 2* dot(light,norm) * norm - light;
  float spec = max(pow(-dot(reflect,eye),shininess),0);
  vec3 color;
  if(gl_FrontFacing){
	color = vec3(0.,0.,1.);
}else{
	color = vec3(1.,0.,0.);
	}
	//float disp = texCoords.w*0.33333;
	//abs(dot(normalize(texCoords.rgb),norm))
	
	color = ambient + (100.0/length(lightvec.xyz))*(diff * diffuse + spec * specular);//vec3(1.0-disp,0.0,disp);//vec3(abs(tan.x));//vec3(0.8,0.4,0.8);
	//color = *vec3();
	float theta = abs(dot(eye,norm));
	float scale = 1.0;
	float frequency = 1.0;

	vec3 albedo;// = lava();
	albedo = vec3(pow(theta,2),pow(theta,3.25),pow(theta,8));
	//texture2D(diffuseTex,texCoords.st).rgb
	FragColor = vec4(color,1.0);//vec3(dc+0.15)//vec4(angle,maxd, 0.5+gl_FragCoord.z,1.0);//vec4(color*vec3(snoise(vec3(texCoords.xy,texCoords.z+time))+1.0)*0.5,1.0);//angle * specular + diffuse;
}