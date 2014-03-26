#version 330
uniform sampler2D framedata;
uniform float time;
uniform int levels;
in vec4 eyevec;
in vec4 texCoords;
in vec4 normal;
out vec4 FragColor;

vec2 st = vec2(texCoords.s,texCoords.t);
vec2 dims = vec2(510.0,133.0);
vec2 one = 1.0/dims;

// Array- and textureless GLSL 3D simplex noise.
// Author : Ian McEwan, Ashima Arts. Version: 20110822
// Copyright (C) 2011 Ashima Arts. All rights reserved.
// Distributed under the MIT License. See LICENSE file.
// https://github.com/ashima/webgl-noise

vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec4 mod289(vec4 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec4 permute(vec4 x) { return mod289(((x*34.0)+1.0)*x); }
vec4 taylorInvSqrt(vec4 r) { return 1.792843 - 0.853735 * r; }

float snoise(vec3 v)
  {
  const vec2 C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4 D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i = floor(v + dot(v, C.yyy) );
  vec3 x0 = v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  vec3 x1 = x0 - i1 + C.xxx;
  vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
  vec3 x3 = x0 - D.yyy; // -1.0+3.0*C.x = -0.5 = -D.y

// Permutations
  i = mod289(i);
  vec4 p = permute( permute( permute(
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients: 7x7 points over a square, mapped onto an octahedron.
// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
  float n_ = 0.142857142857; // 1.0/7.0
  vec3 ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z * ns.z); // mod(p,7*7)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ ); // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.51 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 93.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
                                dot(p2,x2), dot(p3,x3) ) );
}

float aastep ( float threshold , float value ) {
  float afwidth = 0.7 * length ( vec2 ( dFdx ( value ), dFdy ( value )));
  return smoothstep ( threshold - afwidth , threshold + afwidth , value );
}

// Cellular noise ("Worley noise") in 2D in GLSL.
// Copyright (c) Stefan Gustavson 2011-04-19. All rights reserved.
// This code is released under the conditions of the MIT license.
// See LICENSE file for details.

// Permutation polynomial: (34x^2 + x) mod 289
vec3 permute(vec3 x) {
  return mod((34.0 * x + 1.0) * x, 289.0);
}
// Cellular noise, returning F1 and F2 in a vec2.
// Standard 3x3 search window for good F1 and F2 values
vec2 cellular(vec2 P) {
#define K 0.142857142857 // 1/7
#define Ko 0.428571428571 // 3/7
#define jitter 1.0 // Less gives more regular pattern
	vec2 Pi = mod(floor(P), 289.0);
 	vec2 Pf = fract(P);
	vec3 oi = vec3(-1.0, 0.0, 1.0);
	vec3 of = vec3(-0.5, 0.5, 1.5);
	vec3 px = permute(Pi.x + oi);
	vec3 p = permute(px.x + Pi.y + oi); // p11, p12, p13
	vec3 ox = fract(p*K) - Ko;
	vec3 oy = mod(floor(p*K),7.0)*K - Ko;
	vec3 dx = Pf.x + 0.5 + jitter*ox;
	vec3 dy = Pf.y - of + jitter*oy;
	vec3 d1 = dx * dx + dy * dy; // d11, d12 and d13, squared
	p = permute(px.y + Pi.y + oi); // p21, p22, p23
	ox = fract(p*K) - Ko;
	oy = mod(floor(p*K),7.0)*K - Ko;
	dx = Pf.x - 0.5 + jitter*ox;
	dy = Pf.y - of + jitter*oy;
	vec3 d2 = dx * dx + dy * dy; // d21, d22 and d23, squared
	p = permute(px.z + Pi.y + oi); // p31, p32, p33
	ox = fract(p*K) - Ko;
	oy = mod(floor(p*K),7.0)*K - Ko;
	dx = Pf.x - 1.5 + jitter*ox;
	dy = Pf.y - of + jitter*oy;
	vec3 d3 = dx * dx + dy * dy; // d31, d32 and d33, squared
	// Sort out the two smallest distances (F1, F2)
	vec3 d1a = min(d1, d2);
	d2 = max(d1, d2); // Swap to keep candidates for F2
	d2 = min(d2, d3); // neither F1 nor F2 are now in d3
	d1 = min(d1a, d2); // F1 is now in d1
	d2 = max(d1a, d2); // Swap to keep candidates for F2
	d1.xy = (d1.x < d1.y) ? d1.xy : d1.yx; // Swap if smaller
	d1.xz = (d1.x < d1.z) ? d1.xz : d1.zx; // F1 is in d1.x
	d1.yz = min(d1.yz, d2.yz); // F2 is now not in d2.yz
	d1.y = min(d1.y, d1.z); // nor in  d1.z
	d1.y = min(d1.y, d2.x); // F2 is in d1.y, we're done.
	return sqrt(d1.xy);
}

// Cellular noise, returning F1 and F2 in a vec2 and the
// 2D vectors to each of the two closest points in a vec4.
// Standard 3x3 search window for good F1 and F2 values.
void cellular(in vec2 P, out vec2 F, out vec4 d1d2) {
#define K 0.142857142857 // 1/7
#define Ko 0.428571428571 // 3/7
#define jitter 1.0 // Less gives more regular pattern
	vec2 Pi = mod(floor(P), 289.0);
 	vec2 Pf = fract(P);
	vec3 oi = vec3(-1.0, 0.0, 1.0);
	vec3 of = vec3(-0.5, 0.5, 1.5);
	vec3 px = permute(Pi.x + oi);
	vec3 p = permute(px.x + Pi.y + oi); // p11, p12, p13
	vec3 ox = fract(p*K) - Ko;
	vec3 oy = mod(floor(p*K),7.0)*K - Ko;
	vec3 d1x = Pf.x + 0.5 + jitter*ox;
	vec3 d1y = Pf.y - of + jitter*oy;
	vec3 d1 = d1x * d1x + d1y * d1y; // d11, d12 and d13, squared
	p = permute(px.y + Pi.y + oi); // p21, p22, p23
	ox = fract(p*K) - Ko;
	oy = mod(floor(p*K),7.0)*K - Ko;
	vec3 d2x = Pf.x - 0.5 + jitter*ox;
	vec3 d2y = Pf.y - of + jitter*oy;
	vec3 d2 = d2x * d2x + d2y * d2y; // d21, d22 and d23, squared
	p = permute(px.z + Pi.y + oi); // p31, p32, p33
	ox = fract(p*K) - Ko;
	oy = mod(floor(p*K),7.0)*K - Ko;
	vec3 d3x = Pf.x - 1.5 + jitter*ox;
	vec3 d3y = Pf.y - of + jitter*oy;
	vec3 d3 = d3x * d3x + d3y * d3y; // d31, d32 and d33, squared
	// Sort out the two smallest distances (F1, F2)
	// While also swapping dx and dy accordingly
	vec3 comp3 = step(d2, d1);
	vec3 d1a = mix(d1, d2, comp3);
	vec3 d1xa = mix(d1x, d2x, comp3);
	vec3 d1ya = mix(d1y, d2y, comp3);
	d2 = mix(d2, d1, comp3); // Swap to keep candidates for F2
	d2x = mix(d2x, d1x, comp3);
	d2y = mix(d2y, d1y, comp3);

	comp3 = step(d3, d2);
	d2 = mix(d2, d3, comp3); // neither F1 nor F2 are now in d3
	d2x = mix(d2x, d3x, comp3);
	d2y = mix(d2y, d3y, comp3);

	comp3 = step(d2, d1a);
	d1 = mix(d1a, d2, comp3); // F1 is now in d1
	d1x = mix(d1xa, d2x, comp3);
	d1y = mix(d1ya, d2y, comp3);
	d2 = mix(d2, d1a, comp3); // Swap to keep candidates for F2
	d2x = mix(d2x, d1xa, comp3);
	d2y = mix(d2y, d1ya, comp3);

	float comp1 = step(d1.y, d1.x);
	d1.xy = mix(d1.xy, d1.yx, comp1); // Swap if smaller
	d1x.xy = mix(d1x.xy, d1x.yx, comp1);
	d1y.xy = mix(d1y.xy, d1y.yx, comp1);

	comp1 = step(d1.z, d1.x);
	d1.xz = mix(d1.xz, d1.zx, comp1); // F1 is in d1.x
	d1x.xz = mix(d1x.xz, d1x.zx, comp1);
	d1y.xz = mix(d1y.xz, d1y.zx, comp1);

	vec2 comp2 = step(d2.yz, d1.yz);
	d1.yz = mix(d1.yz, d2.yz, comp2); // F2 is now not in d2.yz
	d1x.yz = mix(d1x.yz, d2x.yz, comp2);
	d1y.yz = mix(d1y.yz, d2y.yz, comp2);

	comp1 = step(d1.z, d1.y);
	d1.y = mix(d1.y, d1.z, comp1); // nor in  d1.z
	d1x.y = mix(d1x.y, d1x.z, comp1);
	d1y.y = mix(d1y.y, d1y.z, comp1);

	comp1 = step(d2.x, d1.y);
	d1.y = mix(d1.y, d2.x, comp1); // F2 is in d1.y, we're done.
	d1x.y = mix(d1x.y, d2x.x, comp1);
	d1y.y = mix(d1y.y, d2y.x, comp1);
	F = sqrt(d1.xy);
	d1d2 = vec4(d1x.x, d1y.x, d1x.y, d1y.y);
}

float scale;
float displace(vec3 point){
	float displacement = 0.0;
	scale = 0.1;
	float frequency = 1.0;
	for(int i=0;i<0;i++){
		displacement += scale *(snoise(frequency * point));
		frequency *= 2;
		scale *= 0.5;
	}
	return displacement;
}

vec3 lava()
{
	vec2 F;
  vec4 d1d2;
  //float it = st.x + 1.0;
  vec2 dt = vec2(time*0.1,0.0);
  float ft = dt.x*0.05;
  cellular(8.0*(st+dt), F, d1d2); // Returns vectors to points
  float pattern = length(F.xy);
  dt *= 1.5;
  cellular(16.0*(st+dt), F, d1d2); // Returns vectors to points
  pattern += 0.5 * length(F.xy);
  dt *= 1.5;
  cellular(32.0*(st+dt), F, d1d2); // Returns vectors to points
  pattern += 0.25 * length(F.xy);
  dt *= 1.5;
  cellular(64.0*(st+ft), F, d1d2); // Returns vectors to points
  pattern += sin(dt+(2.0*3.1415*st.x))*F.x;
  dt *= 1.5;
  float pattern2 = snoise(vec3(st+dt+ft,50.0));
  dt *= 1.5;
  //pattern += snoise(vec3(2.0*st+dt+ft,60.0));
  
  // Constant width lines, from the book "Advanced RenderMan"
  float thresh = 0.05 * (length(d1d2.xy - d1d2.zw))
    / (F.x + F.y);
  float f = F.y - F.x;
  f += (0.5 - cellular(64.0*st).y) * thresh;

  vec3 diffuse = pattern * vec3(0.8,0.25,0.0) + pattern2*vec3(0.5,0.0,0.0) + vec3(0.5,0.2,0.0);
  diffuse = mix(diffuse,vec3(0.5,0.2,0.0),10.0*clamp(abs(st.y-0.5)-0.4,0.0,0.1));
  return diffuse;
}

void main( void )
{	
	vec3 norm = normalize(normal.xyz);
	vec3 eye = normalize(eyevec.xyz);
	float theta = abs(dot(eye,norm));
	float noise = 0.0;
	float scale = 1.0;
	float frequency = 1.0;
	for(int i=0;i<levels;i++){
		noise += scale * snoise(frequency*texCoords.rgb+time*eye);
		scale *= 0.5;
		frequency *= 2;
	}
	float alpha = theta * mix(noise,1.0,theta);
	vec3 albedo;// = lava();
	albedo = vec3(pow(theta,2),pow(theta,3.25),pow(theta,8));
	FragColor = vec4(albedo,alpha);//vec3(dc+0.15)//vec4(angle,maxd, 0.5+gl_FragCoord.z,1.0);//vec4(color*vec3(snoise(vec3(texCoords.xy,texCoords.z+time))+1.0)*0.5,1.0);//angle * specular + diffuse;
}