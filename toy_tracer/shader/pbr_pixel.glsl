#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec2 Motion;
in vec2 TexCoord;
in vec3 posWorld;
in vec3 normalWorld;
in vec4 tangentWorld;
in float zValuePos;
in vec2 offset2PrevFrame;
#define POINT_LIGHT_NUM 4
#define NUM_CASCADE_SHADOW 4
#define DEPTH_SEARCH_NUM  6
#define SHADOW_BIAS 0.0015

// material parameters
uniform sampler2D albedoSampler; // vec3
uniform sampler2D mrSampler; // g: roughness value | b: metalness
uniform sampler2D emissionSampler; // vec3
uniform sampler2D normalSampler; // initially 0-1
uniform sampler2D aoSampler;
uniform sampler2DArray depthSampler;
uniform float far;
uniform vec3 albedoFactor;
uniform vec3 emissiveFactor;
uniform float metallicFactor;
uniform float roughFactor;

uniform mat4 world2camPrev;
uniform mat4 world2cam;
uniform mat4 cam2ndc;

uniform vec3 globalEmission = vec3(0.0, 0.0, 0.0);
float bloomThreashold = 1.0;
// lights
struct PointLight {
    vec3 pos;
    vec3 irradiance;
    bool spot;
    bool directional;
    vec3 direction;
    float cosAngle;
    float size;

};
uniform PointLight pointLights[POINT_LIGHT_NUM];
// z ranges of CSM blocks
uniform float zpartition[NUM_CASCADE_SHADOW-1];
uniform mat4 world2lightndc[NUM_CASCADE_SHADOW];
uniform vec3 lightfrustumSize[NUM_CASCADE_SHADOW]; // width, height, length
//uniform mat4 world2lightview[NUM_CASCADE_SHADOW];
uniform vec3 camPos;

const float PI = 3.14159265359;
uniform vec2 PossionDistribution[16] = vec2[](
    vec2(-0.94201624, -0.39906216),
    vec2( 0.94558609, -0.76890725 ),
    vec2(-0.094184101, -0.92938870),
    vec2( 0.34495938, 0.29387760),
    vec2(-0.91588581, 0.45771432),
    vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543, 0.27676845),
    vec2( 0.97484398, 0.75648379),
    vec2( 0.44323325, -0.97511554),
    vec2( 0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023),
    vec2( 0.79197514, 0.19090188),
    vec2(-0.24188840, 0.99706507),
    vec2(-0.81409955, 0.91437590),
    vec2( 0.19984126, 0.78641367),
    vec2( 0.14383161, -0.14100790)
                              );

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec2 spin(vec2 dir, float random) {
    return vec2(cos(random) * dir.x - sin(random) * dir.y,
               sin(random) * dir.x - cos(random) * dir.y);
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 calcShadow(int lightIdx, float distance) {
    // determine which shadow
    int k = 0;
    for(k=0;k<NUM_CASCADE_SHADOW-1;k++) {
        if(zValuePos < zpartition[k])
            break;
    }
    vec3 color;
    if(k==0)
        color = vec3(1, 0, 0);
    if(k==1)
        color = vec3(0, 1, 0);
    if(k==2)
        color = vec3(0, 0, 1);
    float kFloat = (k + 0.5) / NUM_CASCADE_SHADOW;
    
    vec3 ndcPos = vec3(world2lightndc[k] * vec4(posWorld, 1.0));
    vec2 texPos = vec2(ndcPos.x/2 + 0.5, ndcPos.y/2 + 0.5);
    
    
    float closetDepth = texture(depthSampler, vec3(texPos, k)).r;
    float currentDepth;
    if(pointLights[lightIdx].directional)
        currentDepth = (ndcPos.z + 1) / 2; // map to [0, 1]
    else
        // for point lights, use distance
        currentDepth = distance; // Still is absolute value!
    // search the block which subtends the area light
    float searchWidthUV = currentDepth * lightfrustumSize[k].z * pointLights[lightIdx].size / lightfrustumSize[k].x;
    float averageDepth = 0;
    bool totalOccluded = true;
    bool totalPass = true;
    int numOccluded = 0;
    float r = rand(texPos);
    for(int i = 0; i < 8; i++) {
        vec2 p = searchWidthUV * spin(PossionDistribution[i], r) + texPos;
        float currNearDepth = texture(depthSampler, vec3(p, k)).r;
        if(currNearDepth >= currentDepth - SHADOW_BIAS)
            // pass
            totalOccluded = false;
        else {
            totalPass = false;
            numOccluded++;
            averageDepth += currNearDepth;
        }
    }
    if(totalOccluded)
        return vec3(0);
    if(totalPass)
        return vec3(1);
    // compute PCF range
    averageDepth /= numOccluded;
    float actualPenumbraSize = max(currentDepth - averageDepth, 0) * lightfrustumSize[k].z * pointLights[lightIdx].size;
    //actualPenumbraSize = 1.0;
    vec2 actualPenumbraSizeUV = vec2(actualPenumbraSize/lightfrustumSize[k].x, actualPenumbraSize/lightfrustumSize[k].y);
    float shadow = 0;
    for(int i=0;i<16;i++) {
        vec2 p = actualPenumbraSizeUV * spin(PossionDistribution[i], r) + texPos;
        float d = texture(depthSampler, vec3(p, k)).r;
        if(d >= currentDepth - SHADOW_BIAS)
            // pass
            shadow += 1.0;
    }
    shadow /= 16;
    return vec3(shadow);
}

vec3 addDirectLight(vec3 wi, vec3 normal, vec3 albedo, float roughness, float metallic, float ao)
{
    // add point lights
    vec3 Lo = vec3(0.0);
    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = mix(vec3(0.03), albedo, metallic);
    for(int i = 0; i < 1; i++)
    {
        vec3 Lraw = pointLights[i].pos - posWorld;
        vec3 L;
        vec3 shadow;
        if(pointLights[i].directional)
            L = -normalize(pointLights[i].direction);
        else
            L = normalize(Lraw);
        float distance = length(pointLights[i].pos - posWorld);
        if(i==0) {
            shadow = calcShadow(i, distance);
        }
            
        // calculate per-light radiance
        if(pointLights[i].directional && dot(-L, pointLights[i].direction) < pointLights[i].cosAngle)
            continue; // skip
        vec3 H = normalize(wi + L);
        
        float attenuation;
        if(pointLights[i].directional)
            attenuation = 1.0;
        else
            attenuation = 1.0 / (distance * distance);
        vec3 radiance = pointLights[i].irradiance * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(normal, H, roughness);   
        float G   = GeometrySmith(normal, wi, L, roughness);      
        vec3 F    = fresnelSchlick(clamp(dot(H, wi), 0.0, 1.0), F0);
           
        vec3 nominator    = NDF * G * F; 
        float denominator = 4 * max(dot(normal, wi), 0.0) * max(dot(normal, L), 0.0);
        vec3 specular = nominator / max(denominator, 0.001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(normal, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL * shadow;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
        //Lo = shadow;
    }
    return Lo;
}

void main()
{		
    vec3 Nt = vec3(texture(normalSampler, TexCoord));
    Nt = normalize(2*Nt+vec3(-1.0));
    vec3 bitangent = cross(normalWorld, tangentWorld.xyz);
    vec3 N = normalize( Nt.x * tangentWorld.xyz + Nt.y * bitangent + Nt.z * normalWorld);
    //N = normalize(normalWorld);
    vec3 V = normalize(camPos - posWorld);

    
    vec4 albedoA = texture(albedoSampler, TexCoord).rgba;
    vec3 albedo = vec3(albedoA.rgb * albedoFactor);
    if(albedoA.a < 0.1)
        discard;
    float metallic = texture(mrSampler, TexCoord).b * metallicFactor;
    float roughness = texture(mrSampler, TexCoord).g * roughFactor;
    float ao = texture(aoSampler, TexCoord).r;
    // ao is invalid now, so use 1 instead
    ao = 1;
    // reflectance equation
    vec3 Lo = addDirectLight(V, N, albedo, roughness, metallic, ao);
    vec3 Le;
    if(globalEmission == vec3(0.0,0.0,0.0))
        Le = vec3(texture(emissionSampler, TexCoord).rgb * emissiveFactor);
    else
        Le = globalEmission;
    
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo + Le;
    FragColor = vec4(color, 1.0);
    //EmitColor = vec4(Le, 1.0);
    //EmitColor = vec4(0);
    mat4 cam2ndc0 = cam2ndc;
    cam2ndc0[0][2] = cam2ndc0[1][2] = 0.0;
    vec4 lastNDCPosition = cam2ndc0 * world2camPrev * vec4(posWorld, 1.0);
    vec4 currNDCPosition = cam2ndc0 * world2cam * vec4(posWorld, 1.0);
    vec2 mmm = lastNDCPosition.xy/lastNDCPosition.w - currNDCPosition.xy/currNDCPosition.w;
    mmm *= 0.5;
    Motion = mmm;
    //FragColor = vec4(pointLights[0].irradiance, 0);
    //FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}