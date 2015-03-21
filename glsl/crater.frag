#version 330
in vec3 texcoord;
out vec4 FragColor;

/*vec4 FAST32_hash_3D_Cell( vec3 gridcell )	//	generates 4 different random numbers for the single given cell point
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

//	convert a 0.0->1.0 sample to a -1.0->1.0 sample weighted towards the extremes
vec4 Cellular_weight_samples( vec4 samples )
{
    samples = samples * 2.0 - 1.0;
    //return (1.0 - samples * samples) * sign(samples);	// square
    return (samples * samples * samples) - sign(samples);	// cubic (even more variance)
}

//
//	Cellular3D Deriv
//	Cellular3D noise with derivatives
//	returns vec3( value, xderiv, yderiv )
//
vec4 Cellular3D_Deriv(vec3 P, out vec4 v2)
{
    //	establish our grid cell and unit position
    vec3 Pi = floor(P);
    vec3 Pf = P - Pi;

    //	calculate the hash.
    //	( various hashing methods listed in order of speed )
    vec4 hash_x0, hash_y0, hash_z0, hash_x1, hash_y1, hash_z1;
    FAST32_hash_3D( Pi, hash_x0, hash_y0, hash_z0, hash_x1, hash_y1, hash_z1 );
    //SGPP_hash_3D( Pi, hash_x0, hash_y0, hash_z0, hash_x1, hash_y1, hash_z1 );

    //	generate the 8 random points
    //	restrict the random point offset to eliminate artifacts
    //	we'll improve the variance of the noise by pushing the points to the extremes of the jitter window
    const float JITTER_WINDOW = 0.166666666;	// 0.166666666 will guarentee no artifacts. It is the intersection on x of graphs f(x)=( (0.5 + (0.5-x))^2 + 2*((0.5-x)^2) ) and f(x)=( 2 * (( 0.5 + x )^2) + x * x )
    hash_x0 = Cellular_weight_samples( hash_x0 ) * JITTER_WINDOW + vec4(0.0, 1.0, 0.0, 1.0);
    hash_y0 = Cellular_weight_samples( hash_y0 ) * JITTER_WINDOW + vec4(0.0, 0.0, 1.0, 1.0);
    hash_x1 = Cellular_weight_samples( hash_x1 ) * JITTER_WINDOW + vec4(0.0, 1.0, 0.0, 1.0);
    hash_y1 = Cellular_weight_samples( hash_y1 ) * JITTER_WINDOW + vec4(0.0, 0.0, 1.0, 1.0);
    hash_z0 = Cellular_weight_samples( hash_z0 ) * JITTER_WINDOW + vec4(0.0, 0.0, 0.0, 0.0);
    hash_z1 = Cellular_weight_samples( hash_z1 ) * JITTER_WINDOW + vec4(1.0, 1.0, 1.0, 1.0);

    //	return the closest squared distance ( + derivs )
    //  thanks to Jonathan Dupuy for the initial implementation
    vec4 dx1 = Pf.xxxx - hash_x0;
    vec4 dy1 = Pf.yyyy - hash_y0;
    vec4 dz1 = Pf.zzzz - hash_z0;
    vec4 dx2 = Pf.xxxx - hash_x1;
    vec4 dy2 = Pf.yyyy - hash_y1;
    vec4 dz2 = Pf.zzzz - hash_z1;
    vec4 d1 = dx1 * dx1 + dy1 * dy1 + dz1 * dz1;
    vec4 d2 = dx2 * dx2 + dy2 * dy2 + dz2 * dz2;
    vec4 r1 = d1.x < d1.y ? vec4( d1.x, dx1.x, dy1.x, dz1.x ) : vec4( d1.y, dx1.y, dy1.y, dz1.y );
    vec4 r2 = d1.z < d1.w ? vec4( d1.z, dx1.z, dy1.z, dz1.z ) : vec4( d1.w, dx1.w, dy1.w, dz1.w );
    vec4 r3 = d2.x < d2.y ? vec4( d2.x, dx2.x, dy2.x, dz2.x ) : vec4( d2.y, dx2.y, dy2.y, dz2.y );
    vec4 r4 = d2.z < d2.w ? vec4( d2.z, dx2.z, dy2.z, dz2.z ) : vec4( d2.w, dx2.w, dy2.w, dz2.w );
    vec4 t1 = r1.x < r2.x ? r1 : r2;
    vec4 t2 = r3.x < r4.x ? r3 : r4;
	//	scale return value from 0.0->1.333333 to 0.0->1.0  	(2/3)^2 * 3  == (12/9) == 1.333333
	vec4 v1 = ( t1.x < t2.x ? t1 : t2 ) * vec4( 1.0, vec3( 2.0 ) ) * ( 9.0 / 12.0 );
	v2 = ( t1.x < t2.x ? t2 : t1 ) * vec4( 1.0, vec3( 2.0 ) ) * ( 9.0 / 12.0 );
    return 	v1;
}

//
//	Falloff defined in XSquared
//	( smoothly decrease from 1.0 to 0.0 as xsq increases from 0.0 to 1.0 )
//	http://briansharpe.wordpress.com/2011/11/14/two-useful-interpolation-functions-for-noise-development/
//
float Falloff_Xsq_C1( float xsq ) { xsq = 1.0 - xsq; return xsq*xsq; }	// ( 1.0 - x*x )^2   ( Used by Humus for lighting falloff in Just Cause 2.  GPUPro 1 )
float Falloff_Xsq_C2( float xsq ) { xsq = 1.0 - xsq; return xsq*xsq*xsq; }	// ( 1.0 - x*x )^3.   NOTE: 2nd derivative is 0.0 at x=1.0, but non-zero at x=0.0
vec4 Falloff_Xsq_C2( vec4 xsq ) { xsq = 1.0 - xsq; return xsq*xsq*xsq; }

//
//	Cellular Noise 3D
//	Based off Stefan Gustavson's work at http://www.itn.liu.se/~stegu/GLSL-cellular
//	http://briansharpe.files.wordpress.com/2011/12/cellularsample.jpg
//
//	Speed up by using 2x2x2 search window instead of 3x3x3
//	produces range of 0.0->1.0
//
vec3 Cellular3D_Pos(vec3 P)
{
    //	establish our grid cell and unit position
    vec3 Pi = floor(P);
    vec3 Pf = P - Pi;

    //	calculate the hash.
    //	( various hashing methods listed in order of speed )
    vec4 hash_x0, hash_y0, hash_z0, hash_x1, hash_y1, hash_z1;
    FAST32_hash_3D( Pi, hash_x0, hash_y0, hash_z0, hash_x1, hash_y1, hash_z1 );
    //SGPP_hash_3D( Pi, hash_x0, hash_y0, hash_z0, hash_x1, hash_y1, hash_z1 );

    //	generate the 8 random points
#if 1
    //	restrict the random point offset to eliminate artifacts
    //	we'll improve the variance of the noise by pushing the points to the extremes of the jitter window
    const float JITTER_WINDOW = 0.166666666;	// 0.166666666 will guarentee no artifacts. It is the intersection on x of graphs f(x)=( (0.5 + (0.5-x))^2 + 2*((0.5-x)^2) ) and f(x)=( 2 * (( 0.5 + x )^2) + x * x )
    hash_x0 = Cellular_weight_samples( hash_x0 ) * JITTER_WINDOW + vec4(0.0, 1.0, 0.0, 1.0);
    hash_y0 = Cellular_weight_samples( hash_y0 ) * JITTER_WINDOW + vec4(0.0, 0.0, 1.0, 1.0);
    hash_x1 = Cellular_weight_samples( hash_x1 ) * JITTER_WINDOW + vec4(0.0, 1.0, 0.0, 1.0);
    hash_y1 = Cellular_weight_samples( hash_y1 ) * JITTER_WINDOW + vec4(0.0, 0.0, 1.0, 1.0);
    hash_z0 = Cellular_weight_samples( hash_z0 ) * JITTER_WINDOW + vec4(0.0, 0.0, 0.0, 0.0);
    hash_z1 = Cellular_weight_samples( hash_z1 ) * JITTER_WINDOW + vec4(1.0, 1.0, 1.0, 1.0);
#else
    //	non-weighted jitter window.  jitter window of 0.4 will give results similar to Stefans original implementation
    //	nicer looking, faster, but has minor artifacts.  ( discontinuities in signal )
    const float JITTER_WINDOW = 0.4;
    hash_x0 = hash_x0 * JITTER_WINDOW * 2.0 + vec4(-JITTER_WINDOW, 1.0-JITTER_WINDOW, -JITTER_WINDOW, 1.0-JITTER_WINDOW);
    hash_y0 = hash_y0 * JITTER_WINDOW * 2.0 + vec4(-JITTER_WINDOW, -JITTER_WINDOW, 1.0-JITTER_WINDOW, 1.0-JITTER_WINDOW);
    hash_x1 = hash_x1 * JITTER_WINDOW * 2.0 + vec4(-JITTER_WINDOW, 1.0-JITTER_WINDOW, -JITTER_WINDOW, 1.0-JITTER_WINDOW);
    hash_y1 = hash_y1 * JITTER_WINDOW * 2.0 + vec4(-JITTER_WINDOW, -JITTER_WINDOW, 1.0-JITTER_WINDOW, 1.0-JITTER_WINDOW);
    hash_z0 = hash_z0 * JITTER_WINDOW * 2.0 + vec4(-JITTER_WINDOW, -JITTER_WINDOW, -JITTER_WINDOW, -JITTER_WINDOW);
    hash_z1 = hash_z1 * JITTER_WINDOW * 2.0 + vec4(1.0-JITTER_WINDOW, 1.0-JITTER_WINDOW, 1.0-JITTER_WINDOW, 1.0-JITTER_WINDOW);
#endif

    //	return the closest point
    vec3 dx1 = Pf.xyz - hash_x0.xyz;
    vec3 dy1 = Pf.xyz - hash_y0.xyz;
    vec3 dz1 = Pf.xyz - hash_z0.xyz;
    vec3 dx2 = Pf.xyz - hash_x1.xyz;
    vec3 dy2 = Pf.xyz - hash_y1.xyz;
    vec3 dz2 = Pf.xyz - hash_z1.xyz;
    vec4 d1 = length(dx1) < length(dy1) ? vec4(hash_x0.xyz,length(dx1)) : vec4(hash_y0.xyz,length(dy1));
    vec4 d2 = length(dz1) < length(dx2) ? vec4(hash_z0.xyz,length(dz1)) : vec4(hash_x1.xyz,length(dx2));
	vec4 d3 = length(dy2) < length(dz2) ? vec4(hash_y1.xyz,length(dy2)) : vec4(hash_z1.xyz,length(dz2));
    d1 = d1.w < d2.w ? d1 : d2;
	d1 = d1.w < d3.w ? d1 : d3;
    return d1.xyz;	//	scale return value from 0.0->1.333333 to 0.0->1.0  	(2/3)^2 * 3  == (12/9) == 1.333333
}

//
//	Stars2D
//	http://briansharpe.files.wordpress.com/2011/12/starssample.jpg
//
//	procedural texture for creating a starry background.  ( looks good when combined with a nebula/space-like colour texture )
//	NOTE:  Any serious game implementation should hard-code these parameter values for efficiency.
//
//	Return value range of 0.0->1.0
//
float Stars3D(	vec3 P,
                float probability_threshold,		//	probability a star will be drawn  ( 0.0->1.0 )
                float max_dimness,					//	the maximal dimness of a star ( 0.0->1.0   0.0 = all stars bright,  1.0 = maximum variation )
                float two_over_radius )				//	fixed radius for the stars.  radius range is 0.0->1.0  shader requires 2.0/radius as input.
{
    //	establish our grid cell and unit position
    vec3 Pi = floor(P);
    vec3 Pf = P - Pi;

    //	calculate the hash.
    //	( various hashing methods listed in order of speed )
    vec4 hash = FAST32_hash_3D_Cell( Pi );
    //vec4 hash = FAST32_hash_2D( Pi * 2.0 );		//	Need to multiply by 2.0 here because we want to use all 4 corners once per cell.  No sharing with other cells.  It helps if the hash function has an odd domain.
    //vec4 hash = BBS_hash_2D( Pi * 2.0 );
    //vec4 hash = SGPP_hash_2D( Pi * 2.0 );
    //vec4 hash = BBS_hash_hq_2D( Pi * 2.0 );

    //	user variables
    float VALUE = 1.0 - max_dimness * hash.z;

    //	calc the noise and return
    Pf *= two_over_radius;
    Pf -= ( two_over_radius - 1.0 );
    Pf += hash.xyz * ( two_over_radius - 2.0 );
    return ( hash.w < probability_threshold ) ? ( Falloff_Xsq_C1( min( dot( Pf, Pf ), 1.0 ) ) * VALUE ) : 0.0;
}

float craterDist(vec3 point){
	float x = 2.0*length(point);
	return clamp(abs(x),0.0,1.0);
}

float craterfunc(float x)
{
	x = x  *  10.9314;
	return 0.00001* (x*x*x*x) - 0.00055 * (x*x*x) + 0.00729*x*x - 0.02252*x - 0.0493;
	//return 0.00109314 * pow(x,4) - 0.00601227 * pow(x,3) + 0.0796899*x*x - 0.246175*x - 0.538918;
}

float craterRadialDerivative(float x)
{
	return (0.000437256 * pow(x,3)) - (0.01803681 * pow(x,2)) + (0.159398 * x) - 0.246175;
}

float craterRadialNegInvDerivative(float x)
{
	return -2286.99 / ( pow(x,3) - (41.25 * pow(x,2)) + (364.4999 * x) - 563);
}*/

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

float craterfunc(float x)
{
	x = x  *  10.9314;
	return 0.00001* (x*x*x*x) - 0.00055 * (x*x*x) + 0.00729*x*x - 0.02252*x - 0.0493;
	//return 0.00109314 * pow(x,4) - 0.00601227 * pow(x,3) + 0.0796899*x*x - 0.246175*x - 0.538918;
}

float crater(vec3 coord){
	vec4 rand = FAST32_hash_3D_Cell(floor(coord)+0.5);

	//vec3 val = Cellular3D_Pos(coord);
	//val.x = 1.0-abs(distance(coord,10*val));
	//val = coord+val;//0.1;
	vec2 bounds = min(rand.xy,vec2(1.0)-rand.xy);
	float bound = min(bounds.x,bounds.y);
	float depth;
	float dist;
	if(bound < 0.1){
		//no crater here
		dist = 1.0;
		depth = 0.0;
	} else {
		dist = length(rand.xy-fract(coord.xy));
		dist *= 1.0/bound;
		//dist = min(dist,1.0);
		depth = craterfunc(min(dist,1.0));
		depth = mix(1.0,depth,max(min(dist-1.0,0.0),1.0));
	}
	depth *= 10.0;
	depth += 0.5;
	return depth;
}

void main(void){
/*vec3 coord = vec3(10.0*texCoord.xy,1.0);
	vec4 rand = FAST32_hash_3D_Cell(floor(coord)+0.5);

	vec3 val = Cellular3D_Pos(coord);
	//val.x = 1.0-abs(distance(coord,10*val));
	//val = coord+val;//0.1;
	vec2 bounds = min(rand.xy,vec2(1.0)-rand.xy);
	float bound = min(bounds.x,bounds.y);
	float depth;
	float dist;
	if(bound < 0.1){
		//no crater here
		dist = 1.0;
		depth = 0.0;
	} else {
		dist = length(rand.xy-fract(coord.xy));
		dist *= 1.0/bound;
		//dist = min(dist,1.0);
		depth = craterfunc(min(dist,1.0));
		//depth = mix(1.0,depth,max(min(dist-1.0,0.0),1.0));
	}
	//depth *= 5.0;
	depth += 0.5;
	FragColor = vec4(vec3(depth),1.0);*/
	vec3 light = vec3(2.0,-2.0,2.0);
	float height = 5.0f + crater(texcoord);
	//FragColor = vec4(height,abs(dFdx(height)),abs(dFdy(height)),1.0);
	vec3 normal = cross(normalize(vec3(1.0,0.0,dFdx(height))),
			normalize(vec3(0.0,1.0,dFdy(height))));
	FragColor = vec4(normalize(normal),1.0);
}