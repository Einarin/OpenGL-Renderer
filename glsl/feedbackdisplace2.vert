#version 330
in vec3 in_Position;
in vec3 in_Normal;
in vec3 in_Tangent;
in vec3 in_TexCoords;
out vec3 position;
out vec3 normal;
out vec3 tangent;
out vec3 texCoords;
uniform int levels;
uniform vec3 seed;

uniform mat4 transformMatrix;

vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec4 mod289(vec4 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec4 permute(vec4 x) { return mod289(((x*34.0)+1.0)*x); }
vec4 taylorInvSqrt(vec4 r) { return 1.792843 - 0.853735 * r; }

vec4 FAST32_hash_3D_Cell( vec3 gridcell )	//	generates 4 different random numbers for the single given cell point
{
    //    gridcell is assumed to be an integer coordinate

    //	TODO: 	these constants need tweaked to find the best possible noise.
    //			probably requires some kind of brute force computational searching or something....
    const vec2 OFFSET = vec2( 50.0, 161.0 );
    const float DOMAIN = 69.0;
    const vec4 SOMELARGEFLOATS = vec4( 635.298681, 682.357502, 668.926525, 588.255119 );
    const vec4 ZINC = vec4( 48.500388, 65.294118, 63.934599, 63.279683 );

    //	truncate the domain
    gridcell.xyz = gridcell - floor(gridcell * ( 1.0 / DOMAIN )) * DOMAIN;
    gridcell.xy += OFFSET.xy;
    gridcell.xy *= gridcell.xy;
    return fract( ( gridcell.x * gridcell.y ) * ( 1.0 / ( SOMELARGEFLOATS + gridcell.zzzz * ZINC ) ) );
}
void FAST32_hash_3D( vec3 gridcell, out vec4 lowz_hash, out vec4 highz_hash )	//	generates a random number for each of the 8 cell corners
{
    //    gridcell is assumed to be an integer coordinate

    //	TODO: 	these constants need tweaked to find the best possible noise.
    //			probably requires some kind of brute force computational searching or something....
    const vec2 OFFSET = vec2( 50.0, 161.0 );
    const float DOMAIN = 69.0;
    const float SOMELARGEFLOAT = 635.298681;
    const float ZINC = 48.500388;

    //	truncate the domain
    gridcell.xyz = gridcell.xyz - floor(gridcell.xyz * ( 1.0 / DOMAIN )) * DOMAIN;
    vec3 gridcell_inc1 = step( gridcell, vec3( DOMAIN - 1.5 ) ) * ( gridcell + 1.0 );

    //	calculate the noise
    vec4 P = vec4( gridcell.xy, gridcell_inc1.xy ) + OFFSET.xyxy;
    P *= P;
    P = P.xzxz * P.yyww;
    highz_hash.xy = vec2( 1.0 / ( SOMELARGEFLOAT + vec2( gridcell.z, gridcell_inc1.z ) * ZINC ) );
    lowz_hash = fract( P * highz_hash.xxxx );
    highz_hash = fract( P * highz_hash.yyyy );
}
void FAST32_hash_3D( 	vec3 gridcell,
                        vec3 v1_mask,		//	user definable v1 and v2.  ( 0's and 1's )
                        vec3 v2_mask,
                        out vec4 hash_0,
                        out vec4 hash_1,
                        out vec4 hash_2	)		//	generates 3 random numbers for each of the 4 3D cell corners.  cell corners:  v0=0,0,0  v3=1,1,1  the other two are user definable
{
    //    gridcell is assumed to be an integer coordinate

    //	TODO: 	these constants need tweaked to find the best possible noise.
    //			probably requires some kind of brute force computational searching or something....
    const vec2 OFFSET = vec2( 50.0, 161.0 );
    const float DOMAIN = 69.0;
    const vec3 SOMELARGEFLOATS = vec3( 635.298681, 682.357502, 668.926525 );
    const vec3 ZINC = vec3( 48.500388, 65.294118, 63.934599 );

    //	truncate the domain
    gridcell.xyz = gridcell.xyz - floor(gridcell.xyz * ( 1.0 / DOMAIN )) * DOMAIN;
    vec3 gridcell_inc1 = step( gridcell, vec3( DOMAIN - 1.5 ) ) * ( gridcell + 1.0 );

    //	compute x*x*y*y for the 4 corners
    vec4 P = vec4( gridcell.xy, gridcell_inc1.xy ) + OFFSET.xyxy;
    P *= P;
    vec4 V1xy_V2xy = mix( P.xyxy, P.zwzw, vec4( v1_mask.xy, v2_mask.xy ) );		//	apply mask for v1 and v2
    P = vec4( P.x, V1xy_V2xy.xz, P.z ) * vec4( P.y, V1xy_V2xy.yw, P.w );

    //	get the lowz and highz mods
    vec3 lowz_mods = vec3( 1.0 / ( SOMELARGEFLOATS.xyz + gridcell.zzz * ZINC.xyz ) );
    vec3 highz_mods = vec3( 1.0 / ( SOMELARGEFLOATS.xyz + gridcell_inc1.zzz * ZINC.xyz ) );

    //	apply mask for v1 and v2 mod values
    v1_mask = ( v1_mask.z < 0.5 ) ? lowz_mods : highz_mods;
    v2_mask = ( v2_mask.z < 0.5 ) ? lowz_mods : highz_mods;

    //	compute the final hash
    hash_0 = fract( P * vec4( lowz_mods.x, v1_mask.x, v2_mask.x, highz_mods.x ) );
    hash_1 = fract( P * vec4( lowz_mods.y, v1_mask.y, v2_mask.y, highz_mods.y ) );
    hash_2 = fract( P * vec4( lowz_mods.z, v1_mask.z, v2_mask.z, highz_mods.z ) );
}
vec4 FAST32_hash_3D( 	vec3 gridcell,
                        vec3 v1_mask,		//	user definable v1 and v2.  ( 0's and 1's )
                        vec3 v2_mask )		//	generates 1 random number for each of the 4 3D cell corners.  cell corners:  v0=0,0,0  v3=1,1,1  the other two are user definable
{
    //    gridcell is assumed to be an integer coordinate

    //	TODO: 	these constants need tweaked to find the best possible noise.
    //			probably requires some kind of brute force computational searching or something....
    const vec2 OFFSET = vec2( 50.0, 161.0 );
    const float DOMAIN = 69.0;
    const float SOMELARGEFLOAT = 635.298681;
    const float ZINC = 48.500388;

    //	truncate the domain
    gridcell.xyz = gridcell.xyz - floor(gridcell.xyz * ( 1.0 / DOMAIN )) * DOMAIN;
    vec3 gridcell_inc1 = step( gridcell, vec3( DOMAIN - 1.5 ) ) * ( gridcell + 1.0 );

    //	compute x*x*y*y for the 4 corners
    vec4 P = vec4( gridcell.xy, gridcell_inc1.xy ) + OFFSET.xyxy;
    P *= P;
    vec4 V1xy_V2xy = mix( P.xyxy, P.zwzw, vec4( v1_mask.xy, v2_mask.xy ) );		//	apply mask for v1 and v2
    P = vec4( P.x, V1xy_V2xy.xz, P.z ) * vec4( P.y, V1xy_V2xy.yw, P.w );

    //	get the z mod vals
    vec2 V1z_V2z = vec2( v1_mask.z < 0.5 ? gridcell.z : gridcell_inc1.z, v2_mask.z < 0.5 ? gridcell.z : gridcell_inc1.z );
    vec4 mod_vals = vec4( 1.0 / ( SOMELARGEFLOAT + vec4( gridcell.z, V1z_V2z, gridcell_inc1.z ) * ZINC ) );

    //	compute the final hash
    return fract( P * mod_vals );
}
void FAST32_hash_3D( 	vec3 gridcell,
                        out vec4 lowz_hash_0,
                        out vec4 lowz_hash_1,
                        out vec4 lowz_hash_2,
                        out vec4 highz_hash_0,
                        out vec4 highz_hash_1,
                        out vec4 highz_hash_2	)		//	generates 3 random numbers for each of the 8 cell corners
{
    //    gridcell is assumed to be an integer coordinate

    //	TODO: 	these constants need tweaked to find the best possible noise.
    //			probably requires some kind of brute force computational searching or something....
    const vec2 OFFSET = vec2( 50.0, 161.0 );
    const float DOMAIN = 69.0;
    const vec3 SOMELARGEFLOATS = vec3( 635.298681, 682.357502, 668.926525 );
    const vec3 ZINC = vec3( 48.500388, 65.294118, 63.934599 );

    //	truncate the domain
    gridcell.xyz = gridcell.xyz - floor(gridcell.xyz * ( 1.0 / DOMAIN )) * DOMAIN;
    vec3 gridcell_inc1 = step( gridcell, vec3( DOMAIN - 1.5 ) ) * ( gridcell + 1.0 );

    //	calculate the noise
    vec4 P = vec4( gridcell.xy, gridcell_inc1.xy ) + OFFSET.xyxy;
    P *= P;
    P = P.xzxz * P.yyww;
    vec3 lowz_mod = vec3( 1.0 / ( SOMELARGEFLOATS.xyz + gridcell.zzz * ZINC.xyz ) );
    vec3 highz_mod = vec3( 1.0 / ( SOMELARGEFLOATS.xyz + gridcell_inc1.zzz * ZINC.xyz ) );
    lowz_hash_0 = fract( P * lowz_mod.xxxx );
    highz_hash_0 = fract( P * highz_mod.xxxx );
    lowz_hash_1 = fract( P * lowz_mod.yyyy );
    highz_hash_1 = fract( P * highz_mod.yyyy );
    lowz_hash_2 = fract( P * lowz_mod.zzzz );
    highz_hash_2 = fract( P * highz_mod.zzzz );
}
void FAST32_hash_3D( 	vec3 gridcell,
                        out vec4 lowz_hash_0,
                        out vec4 lowz_hash_1,
                        out vec4 lowz_hash_2,
                        out vec4 lowz_hash_3,
                        out vec4 highz_hash_0,
                        out vec4 highz_hash_1,
                        out vec4 highz_hash_2,
                        out vec4 highz_hash_3	)		//	generates 4 random numbers for each of the 8 cell corners
{
    //    gridcell is assumed to be an integer coordinate

    //	TODO: 	these constants need tweaked to find the best possible noise.
    //			probably requires some kind of brute force computational searching or something....
    const vec2 OFFSET = vec2( 50.0, 161.0 );
    const float DOMAIN = 69.0;
    const vec4 SOMELARGEFLOATS = vec4( 635.298681, 682.357502, 668.926525, 588.255119 );
    const vec4 ZINC = vec4( 48.500388, 65.294118, 63.934599, 63.279683 );

    //	truncate the domain
    gridcell.xyz = gridcell.xyz - floor(gridcell.xyz * ( 1.0 / DOMAIN )) * DOMAIN;
    vec3 gridcell_inc1 = step( gridcell, vec3( DOMAIN - 1.5 ) ) * ( gridcell + 1.0 );

    //	calculate the noise
    vec4 P = vec4( gridcell.xy, gridcell_inc1.xy ) + OFFSET.xyxy;
    P *= P;
    P = P.xzxz * P.yyww;
    lowz_hash_3.xyzw = vec4( 1.0 / ( SOMELARGEFLOATS.xyzw + gridcell.zzzz * ZINC.xyzw ) );
    highz_hash_3.xyzw = vec4( 1.0 / ( SOMELARGEFLOATS.xyzw + gridcell_inc1.zzzz * ZINC.xyzw ) );
    lowz_hash_0 = fract( P * lowz_hash_3.xxxx );
    highz_hash_0 = fract( P * highz_hash_3.xxxx );
    lowz_hash_1 = fract( P * lowz_hash_3.yyyy );
    highz_hash_1 = fract( P * highz_hash_3.yyyy );
    lowz_hash_2 = fract( P * lowz_hash_3.zzzz );
    highz_hash_2 = fract( P * highz_hash_3.zzzz );
    lowz_hash_3 = fract( P * lowz_hash_3.wwww );
    highz_hash_3 = fract( P * highz_hash_3.wwww );
}

//
//	Given an arbitrary 3D point this calculates the 4 vectors from the corners of the simplex pyramid to the point
//	It also returns the integer grid index information for the corners
//
void Simplex3D_GetCornerVectors( 	vec3 P,					//	input point
                                    out vec3 Pi,			//	integer grid index for the origin
                                    out vec3 Pi_1,			//	offsets for the 2nd and 3rd corners.  ( the 4th = Pi + 1.0 )
                                    out vec3 Pi_2,
                                    out vec4 v1234_x,		//	vectors from the 4 corners to the intput point
                                    out vec4 v1234_y,
                                    out vec4 v1234_z )
{
    //
    //	Simplex math from Stefan Gustavson's and Ian McEwan's work at...
    //	http://github.com/ashima/webgl-noise
    //

    //	simplex math constants
    const float SKEWFACTOR = 1.0/3.0;
    const float UNSKEWFACTOR = 1.0/6.0;
    const float SIMPLEX_CORNER_POS = 0.5;
    const float SIMPLEX_PYRAMID_HEIGHT = 0.70710678118654752440084436210485;	// sqrt( 0.5 )	height of simplex pyramid.

    P *= SIMPLEX_PYRAMID_HEIGHT;		// scale space so we can have an approx feature size of 1.0  ( optional )

    //	Find the vectors to the corners of our simplex pyramid
    Pi = floor( P + dot( P, vec3( SKEWFACTOR) ) );
    vec3 x0 = P - Pi + dot(Pi, vec3( UNSKEWFACTOR ) );
    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    Pi_1 = min( g.xyz, l.zxy );
    Pi_2 = max( g.xyz, l.zxy );
    vec3 x1 = x0 - Pi_1 + UNSKEWFACTOR;
    vec3 x2 = x0 - Pi_2 + SKEWFACTOR;
    vec3 x3 = x0 - SIMPLEX_CORNER_POS;

    //	pack them into a parallel-friendly arrangement
    v1234_x = vec4( x0.x, x1.x, x2.x, x3.x );
    v1234_y = vec4( x0.y, x1.y, x2.y, x3.y );
    v1234_z = vec4( x0.z, x1.z, x2.z, x3.z );
}

//
//	SimplexPerlin3D_Deriv
//	SimplexPerlin3D noise with derivatives
//	returns vec3( value, xderiv, yderiv, zderiv )
//
vec4 SimplexPerlin3D_Deriv(vec3 P)
{
    //	calculate the simplex vector and index math
    vec3 Pi;
    vec3 Pi_1;
    vec3 Pi_2;
    vec4 v1234_x;
    vec4 v1234_y;
    vec4 v1234_z;
    Simplex3D_GetCornerVectors( P, Pi, Pi_1, Pi_2, v1234_x, v1234_y, v1234_z );

    //	generate the random vectors
    //	( various hashing methods listed in order of speed )
    vec4 hash_0;
    vec4 hash_1;
    vec4 hash_2;
    FAST32_hash_3D( Pi, Pi_1, Pi_2, hash_0, hash_1, hash_2 );
    //SGPP_hash_3D( Pi, Pi_1, Pi_2, hash_0, hash_1, hash_2 );
    hash_0 -= 0.49999;
    hash_1 -= 0.49999;
    hash_2 -= 0.49999;

    //	normalize random gradient vectors
    vec4 norm = inversesqrt( hash_0 * hash_0 + hash_1 * hash_1 + hash_2 * hash_2 );
    hash_0 *= norm;
    hash_1 *= norm;
    hash_2 *= norm;

    //	evaluate gradients
    vec4 grad_results = hash_0 * v1234_x + hash_1 * v1234_y + hash_2 * v1234_z;

    //	evaluate the surflet f(x)=(0.5-x*x)^3
    vec4 m = v1234_x * v1234_x + v1234_y * v1234_y + v1234_z * v1234_z;
    m = max(0.5 - m, 0.0);		//	The 0.5 here is SIMPLEX_PYRAMID_HEIGHT^2
    vec4 m2 = m*m;
    vec4 m3 = m*m2;

    //	calc the deriv
    vec4 temp = -6.0 * m2 * grad_results;
    float xderiv = dot( temp, v1234_x ) + dot( m3, hash_0 );
    float yderiv = dot( temp, v1234_y ) + dot( m3, hash_1 );
    float zderiv = dot( temp, v1234_z ) + dot( m3, hash_2 );

    const float FINAL_NORMALIZATION = 37.837227241611314102871574478976;	//	scales the final result to a strict 1.0->-1.0 range

    //	sum with the surflet and return
    return vec4( dot( m3, grad_results ), xderiv, yderiv, zderiv ) * FINAL_NORMALIZATION;
}

//
//	Calculate the weights for the 3D simplex surflet
//
vec4 Simplex3D_GetSurfletWeights( 	vec4 v1234_x,
                                    vec4 v1234_y,
                                    vec4 v1234_z )
{
    //	perlins original implementation uses the surlet falloff formula of (0.6-x*x)^4.
    //	This is buggy as it can cause discontinuities along simplex faces.  (0.5-x*x)^3 solves this and gives an almost identical curve

    //	evaluate surflet. f(x)=(0.5-x*x)^3
    vec4 surflet_weights = v1234_x * v1234_x + v1234_y * v1234_y + v1234_z * v1234_z;
    surflet_weights = max(0.5 - surflet_weights, 0.0);		//	0.5 here represents the closest distance (squared) of any simplex pyramid corner to any of its planes.  ie, SIMPLEX_PYRAMID_HEIGHT^2
    return surflet_weights*surflet_weights*surflet_weights;
}

//
//	SimplexPerlin3D  ( simplex gradient noise )
//	Perlin noise over a simplex (tetrahedron) grid
//	Return value range of -1.0->1.0
//	http://briansharpe.files.wordpress.com/2012/01/simplexperlinsample.jpg
//
//	Implementation originally based off Stefan Gustavson's and Ian McEwan's work at...
//	http://github.com/ashima/webgl-noise
//
float SimplexPerlin3D(vec3 P)
{
    //	calculate the simplex vector and index math
    vec3 Pi;
    vec3 Pi_1;
    vec3 Pi_2;
    vec4 v1234_x;
    vec4 v1234_y;
    vec4 v1234_z;
    Simplex3D_GetCornerVectors( P, Pi, Pi_1, Pi_2, v1234_x, v1234_y, v1234_z );

    //	generate the random vectors
    //	( various hashing methods listed in order of speed )
    vec4 hash_0;
    vec4 hash_1;
    vec4 hash_2;
    FAST32_hash_3D( Pi, Pi_1, Pi_2, hash_0, hash_1, hash_2 );
    //SGPP_hash_3D( Pi, Pi_1, Pi_2, hash_0, hash_1, hash_2 );
    hash_0 -= 0.49999;
    hash_1 -= 0.49999;
    hash_2 -= 0.49999;

    //	evaluate gradients
    vec4 grad_results = inversesqrt( hash_0 * hash_0 + hash_1 * hash_1 + hash_2 * hash_2 ) * ( hash_0 * v1234_x + hash_1 * v1234_y + hash_2 * v1234_z );

    //	Normalization factor to scale the final result to a strict 1.0->-1.0 range
    //	x = sqrt( 0.75 ) * 0.5
    //	NF = 1.0 / ( x * ( ( 0.5 – x*x ) ^ 3 ) * 2.0 )
    //	http://briansharpe.wordpress.com/2012/01/13/simplex-noise/#comment-36
    const float FINAL_NORMALIZATION = 37.837227241611314102871574478976;

    //	sum with the surflet and return
    return dot( Simplex3D_GetSurfletWeights( v1234_x, v1234_y, v1234_z ), grad_results ) * FINAL_NORMALIZATION;
}

float craterDist(vec3 point){
	float x = 2.0*distance(normalize(vec3(0.5,0.0,0.0)),normalize(in_Position.xyz));
	return clamp(abs(x),0.0,1.0);
}

float craterfunc(vec3 point)
{
	float x = craterDist(point) * 10.9314;
	return 0.00001* pow(x,4) - 0.00055 * pow(x,3) + 0.00729*x*x - 0.02252*x - 0.0493;
}

float crater(vec3 point){
	//vec4 num = FAST32_hash_3D_Cell(floor(point*10.0));
	return 4.0 * craterfunc(point);
}

float scale;
vec4 displace(vec3 point, vec3 seed){
	vec4 displacement = vec4(0.0);
	scale = 0.1;
	float frequency = 1.0;
	for(int i=0;i<levels;i++){
		vec4 result = SimplexPerlin3D_Deriv(frequency * (point+seed));
		displacement += scale * result;
		frequency *= 2;
		scale *= 0.5;
	}
	//crater(point) + 
	return displacement;//,craterDist());
}



void main(void)
{
	//texCoords = in_Position;
	mat3 normalMatrix = normalize(transpose(inverse(mat3(transformMatrix)));
	vec3 norm = normalize(in_Normal);
	tangent = normalize(in_Tangent);
	vec3 bitan = normalize(cross(norm,tangent));
	texCoords = in_Position;
	mat3 tbn = transpose(mat3(tangent,bitan,norm));
	vec4 pos = transformMatrix * vec4(in_Position.xyz,1.0);
	position = in_Position;
	//float scale = 0.1;////clamp(time,0.0,1.0);
	//float frequency = 1.0;
	//time *= 0.001;
	vec4 displacement;// = vec4(0.0,0.0,0.0,0.0);
	/*for(int i=0;i<5;i++){
		displacement.x += scale *(SimplexPerlin3D(frequency * position.xyz));
		displacement.y = scale * SimplexPerlin3D(frequency * (position.xyz+vec3(0.01,0.0,0.0)));
		displacement.z = scale * SimplexPerlin3D(frequency * (position.xyz+vec3(0.0,0.01,0.0)));
		displacement.w = scale * SimplexPerlin3D(frequency * (position.xyz+vec3(0.0,0.0,0.01)));
		//texCoords = SimplexPerlin3D_Deriv(frequency * position.xyz);
		//displacement += scale *texCoords.xyz;
		frequency *= 2;
		scale *= 0.5;
	}*/
	displacement = displace( in_Position.xyz,seed);
	const float dt = 0.000001;
	vec4 disptangent = displace( normalize(in_Position.xyz+dt*tangent),seed);
	vec4 dispbitan = displace( normalize(in_Position.xyz+dt*bitan),seed);
	vec4 dispnorm = displace( normalize(in_Position.xyz+dt*norm),seed);
	
	//vec3 df = vec3(disptangent.x,dispbitan.x,dispnorm.x)- displacement.x;
	
	//displacement.y = displace( normalize(in_Position.xyz+vec3(dt,0.0,0.0))+seed);
	//displacement.z = displace( normalize(in_Position.xyz+vec3(0.0,dt,0.0))+seed);
	//displacement.w = displace( normalize(in_Position.xyz+vec3(0.0,0.0,dt))+seed);
	//vec3 df = displacement.yzw - displacement.x;
	//df *= 1.0/dt;
	//df = normalize(df);
	//displacement = clamp(displacement,0.1,3.0);
	float localX = dot(tangent,displacement.yzw);
	float localY = dot(bitan,displacement.yzw);
	float localZ = dot(norm,displacement.yzw);
	vec3 df = normalize(vec3(localX,localY,localZ));
	position = pos.xyz + (norm * 2.0 * displacement.x);
	vec3 tanpos = (transformMatrix * vec4(normalize(in_Position.xyz+dt*tangent),1.0)).xyz+ (norm * 2.0 * disptangent.x);
	vec3 bitanpos = (transformMatrix * vec4(normalize(in_Position.xyz+dt*bitan),1.0)).xyz+ (norm * 2.0 * dispbitan.x);
	vec3 dtp = tanpos.xyz - position.xyz;
	vec3 dbp = bitanpos.xyz - position.xyz;
	//vec3 dtp = vec3(dt,0.0,disptangent.x - displacement.x);
	//vec3 dbp = vec3(0.0,dt,dispbitan.x - displacement.x);
	normal = normalize(cross(dtp,dbp));
	//normal = dot(normal,norm) > 0.0?normal:dot(normal,norm) < 0.0?-normal:norm;
	//texCoords.xyz = texCoords.yzw;
	//vec3 ccn = cross(cross(norm,-df),-df);
	//normal = tbn * displacement.yzw;//-df;
	//float x2 = pow(localX.x+localX.y+localX.z,2);
	//float y2 = pow(localY.x+localY.y+localY.z,2);
	//float z2 = max(1.0-x2-y2,0.0);
	//normal = sign(length(norm))*sqrt(vec3(x2,y2,z2));
	//texCoords.w = displacement.x;
	//texCoords.xy = displacement.yz;
	/*vec3 off1 = (position.xyz+0.01*tan.xyz) * displacement.y;
	off1 -= position.xyz;
	vec3 off2 = (position.xyz+0.01*bitan.xyz) * displacement.y;
	off2 -= position.xyz;*/
	//normal = viewMatrix * vec4(normalize(in_Normal.xyz),0.0);
	//normal.xyz = normalize(cross(off1,off2));
	//position = viewMatrix * position;
	//position = projMatrix * viewMatrix * position;//modelMatrix * vec4(in_Position.xyz,1.0); 
}