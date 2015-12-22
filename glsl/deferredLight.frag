#version 430 core
in vec2 texCoords;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 cameraWorldPosition;
uniform mat4 screenToWorld;

uniform sampler2D albedos;
uniform sampler2D normals;
uniform sampler2D depthTex;
uniform sampler2D worldTex;
uniform samplerCube environment;

out vec4 fragColor;

const float pi = 3.14159265;


//specular

//Implements the Microfacet BRDF
//          F(1,h)*G(l,v,h)*D(h)
// f(1,v) = --------------------
//           4*dot(n,l)*dot(n,v)

//The half vector is the angle of surfaces that reflect
//  the light source at the view angle
//  h in Microfacet model
vec3 halfVector(vec3 light, vec3 normal){
    return normalize(light + normal);
}

//Schlick Approximation of Fresnel
// F(l,h) in Microfacet model
vec3 fresnelSchlick(vec3 specularRGB, vec3 light, vec3 halfVec){
    return specularRGB + (1.0-specularRGB)*pow(1.0-dot(light,halfVec),5);
}

// D(h) is the microfacet Normal Distribution
//  generally gaussian-like
//  uses roughness parameter low roughness means most facets close to n
float microfacetDistribution(vec3 normal, vec3 halfVec, float roughness){
    return ((roughness + 2)/(2*pi)) * pow(max(dot(normal,halfVec),0.0),roughness);
}
float ggxDistribution(vec3 normal, vec3 halfVec, float roughness){
    float alpha = roughness * roughness;
    float a2 = alpha * alpha;
    float dnh = dot(normal,halfVec);
    float denom = dnh * dnh * (a2 - 1)+1;
    return a2 / (denom * denom);
}

// G(1,v,h) chance that a microfacet of the given orientation
//  is shadowed and/or masked
//  usually driven by roughness parameter
float simpleGeometry(){
//super simple to start
    return 1;
}
float kelemenSzirmayKalosGeometry(vec3 lightDir,vec3 halfVec){
    float denom = dot(lightDir,halfVec);
    return 1.f/(denom*denom);
}

//mapping to provide good results for roughness
// roughIn is 0-1 value from texutre, maxRough is largest value
float calcRoughness(float maxRough,float roughIn){
    return pow(maxRough,roughIn);
}

vec3 specular(vec3 lightDir, vec3 normal, vec3 viewDir, vec3 specularRGB, float roughness){
    vec3 halfVec = halfVector(lightDir,viewDir);
    float microfacet = microfacetDistribution(normal,halfVec,roughness);
    vec3 fresnel = fresnelSchlick(specularRGB,lightDir,halfVec);
    float geometry = kelemenSzirmayKalosGeometry(lightDir,halfVec);
    return 0.25 * geometry * microfacet * fresnel;
}

//diffuse
// punctual light reflection equation
//  L0(v) = pi*f(lc,v) * lightRadiance(dot(n,lightCenter))

//light falls off with the square of the distance
float lightFalloff(float lightDistance, float lightRadius){
    float lightNumerator = min(1 - pow(lightDistance/lightRadius,4),1.0);
    return (lightNumerator * lightNumerator)/((lightDistance*lightDistance)+1);
}

float mip_map_level(in vec3 texture_coordinate)
{
    // The OpenGL Graphics System: A Specification 4.2
    //  - chapter 3.9.11, equation 3.21
    vec3  dx_vtc        = dFdx(texture_coordinate);
    vec3  dy_vtc        = dFdy(texture_coordinate);
    float delta_max_sqr = max(dot(dx_vtc, dx_vtc), dot(dy_vtc, dy_vtc));

    //return max(0.0, 0.5 * log2(delta_max_sqr) - 1.0); // == log2(sqrt(delta_max_sqr));
    return 0.5 * log2(delta_max_sqr); // == log2(sqrt(delta_max_sqr));
}

void main(void){
    //need to renormalize since components are linearly interpolated separately
    vec4 normalData = texture(normals,texCoords);
    vec3 normal = normalData.rgb;
    float roughness = normalData.a;
    vec4 albedoData = texture(albedos,texCoords);
    vec3 materialColor = albedoData.rgb;
    float metalness = albedoData.a;
    if(length(normal) == 0){
      if(length(materialColor) == 0){
        discard;
      } else {
        fragColor = vec4(materialColor,1.0);
        return;
      }
    }
    float depth = texture(depthTex,texCoords).r;
    vec4 screenCoord4 = vec4(vec3(texCoords,depth)*2.0 - 1.0,1.0);
    //determine world position
    vec4 worldCoord4 = screenToWorld * screenCoord4;
    vec3 worldPos = worldCoord4.xyz/worldCoord4.w;
    //vec3 worldPos2 = texture(worldTex,texCoords).rgb;
    //vec3 worldDiff = abs(worldPos - worldPos2)

    vec3 lightdiff = lightPosition - worldPos;
    vec3 lightDirection = normalize(lightdiff);
    float lightDistance = length(lightdiff);
    vec3 eyeDirection = normalize(cameraWorldPosition - worldPos);
    vec3 reflectVec = reflect(eyeDirection,normal);
    //spherical Approximation
    float radius = 1.0;
    vec3 centerToRay = dot(lightDirection,reflectVec)*reflectVec - lightDirection;
    vec3 closestPoint = lightDirection + centerToRay * min(radius/abs(centerToRay),1.0);
    vec3 specularLightDirection = normalize(closestPoint);

    //calculate lambertian diffuse term, metals have no diffuse
    vec3 diffuse = mix(materialColor.rgb,vec3(0.0),metalness);
    //calculate specular color, 0.004 is a good enough specular for all dielectrics
    vec3 specColor = mix(vec3(0.04),materialColor.rgb,metalness);
    //float specRoughness = min(0.0,roughness - radius/(2.0*lightDistance));
    float mappedRough = calcRoughness(100000.0,roughness);
    //diffuse = diffuse / materialData.a;
    //calculate blinn-phong specular term
    //vec3 halfVec = normalize(normal + lightDirection);
    //float specularAngle = max(dot(halfVec,normal),0.0);
    //float specular = pow(specularAngle, 16.0); //constant controls the roughness of the material
    float specCorrection = (3.14159*radius*radius)/(2.0*lightDistance);

    float falloff = lightFalloff(lightDistance,1000.0);
    //vec3 lighting = lightColor * (specular + diffuse) / falloff;
    vec3 lighting = falloff * lightColor * max(dot(normal,lightDirection),0.0) * (diffuse + specular(lightDirection,normal,eyeDirection,specColor,mappedRough)/specCorrection);
    //add some ambient lighting by approximating a light shining straight down
    //vec3 skyDir = vec3(0.0,1.0,0.0);
    //lighting += lightColor * 0.000001 /* max(dot(normal,skyDir),0.0)*/ * (diffuse + specular(skyDir,skyDir,skyDir,specColor,mappedRough));
    //lighting += 0.001 * materialColor.rgb;

    float lod = textureQueryLevels(environment) * (1.0 - roughness);
    vec3 envSample = textureLod(environment,reflectVec,lod).rgb;
    lighting += specColor * envSample;
    //Use HDR with simple tonmapping
    //This needs to be done after lighting is accumulated,
    //  because light adds linearly in real life
    //vec3 toneMapped = lighting/(lighting+1.0);
    //write our output
    gl_FragDepth = depth;
    fragColor = vec4(lighting,1.0);
}
