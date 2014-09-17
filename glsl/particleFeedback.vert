// Vertex Shader – file "feedback.vert"

#version 130

in  vec3 in_Position;
in  vec3 in_Velocity;
in  vec4 in_Color;
out vec3 ex_Position;
out vec3 ex_Velocity;
out vec4 ex_Color;

//uniform float aspect;
float aspect = 1.0;

//uniform vec3 attractors[20];

float snoise(vec2 v);

float rand(vec2 co){
    return snoise(co);//fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 randvel(vec3 v){
	return vec3(rand(v.yz),rand(v.xz),rand(v.xy));
}

float drift(vec2 co){
	return (rand(co) - 0.5)*0.0005;
}

vec3 gravity(vec3 attractor){
	vec3 d = (attractor - in_Position.xyz);
	vec3 d2 = d*d;
	vec3 result;
	return 2. * normalize(d)/(d2.x+d2.y+d2.z);
}

void main(void)
{
	/*ex_Color.r = in_Color.r + drift(in_Color.gr);
	ex_Color.g = in_Color.g + drift(in_Color.bg);
	ex_Color.b = in_Color.b + drift(in_Color.rb);
	ex_Color.a = 0.9;

	ex_Position.r = clamp(in_Position.r + drift(in_Position.gr),-1.,1.);
	ex_Position.g = clamp(in_Position.g + drift(in_Position.bg),-1.,1.);
	ex_Position.b = clamp(in_Position.b + drift(in_Position.rb),-1.,1.);*/

	/*ex_Color.r = in_Color.r;
	ex_Color.g = in_Color.g;
	ex_Color.b = in_Color.b;
	ex_Color.a = in_Color.a;*/
	
	vec3 accel = vec3(0.);
	/*for(uint i=uint(0);i<uint(20);i++)
	{
		accel += attractors[i].z*gravity(attractors[i].xy);
	}*/
	//accel += 0.01 * gravity(vec3(0.));
	const float maxAccel = 10.0;
	//accel = clamp(accel,vec3(-maxAccel),vec3(maxAccel));

	//vec2 rotation = vec2(0.);
	float bound = length(vec2(1.0,aspect));
	//float rspeed = (bound-clamp(length(in_Position.xyz),0.,bound));
	vec2 rotation = cross(in_Position.xyz,vec3(0.,0.,1.)).xy;
	vec2 attraction = -(in_Position.xy * 0.5);
	//rotation *= 0.01;
	
	//rotation -= normalize(in_Position.xy)*(0.1+abs(length(rotation)));

	ex_Velocity.xyz = (in_Velocity.xyz + accel);
	
	vec3 pos = in_Position.xyz + (0.00001 * ex_Velocity.xyz);
	if(abs(pos.x) > 1) {
		ex_Velocity.xyz = vec3(abs(ex_Velocity.x) * -sign(pos.x),ex_Velocity.yz) * randvel(ex_Velocity.xyz);
	}
	if(abs(pos.y) > 1) {
		//ex_Velocity.y = abs(ex_Velocity.y) * -sign(pos.y) + randvel(ex_Velocity.xyz);
		ex_Velocity.xyz = vec3(abs(ex_Velocity.y) * -sign(pos.y),ex_Velocity.yz) * randvel(ex_Velocity.xyz);
	}
	if(abs(pos.z) > 1) {
		ex_Velocity.z = abs(ex_Velocity.z) * -sign(pos.z) + randvel(ex_Velocity.xyz);
		ex_Velocity.xyz = vec3(abs(ex_Velocity.z) * -sign(pos.z),ex_Velocity.yz) * randvel(ex_Velocity.xyz);
	}
	//pos += 0.01 * mix(rotation,vec2(0.),clamp(length(in_Position.xy)/aspect,0.,1.));
	//ex_Position.xy = in_Position.xy + accel;
	ex_Position.xyz = clamp(pos.xyz,-1.,1.);

	/*ex_Color.r = abs(dot(normalize(ex_Velocity.xyz),vec3(1.,0.,0.))/3.1415);
	ex_Color.g = max(distance(ex_Position.xyz,vec3(0)),dot(normalize(ex_Velocity.xyz),vec3(-1.,-1.,0)));
	ex_Color.b = abs(dot(normalize(ex_Velocity.xyz),vec3(0.,1.,0.))/3.1415);
	ex_Color.a = in_Color.a;*/
	ex_Color = vec4(vec3(abs(pos.xyz)*length(ex_Velocity.xyz)/100),in_Color.a);
}

//
// Description : Array and textureless GLSL 2D simplex noise function.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
// 

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
  return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v)
  {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                     -0.577350269189626,  // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
// First corner
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);

// Other corners
  vec2 i1;
  //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  //i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

// Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
		+ i.x + vec3(0.0, i1.x, 1.0 ));

  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;

// Gradients: 41 points uniformly over a line, mapped onto a diamond.
// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

// Normalise gradients implicitly by scaling m
// Approximation of: m *= inversesqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

// Compute final noise value at P
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}