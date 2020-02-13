#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 EmitColor;
in vec2 TexCoord;
in vec3 posWorld;
in vec3 normalWorld;
in vec4 tangentWorld;
#define POINT_LIGHT_NUM 4
in float depth[POINT_LIGHT_NUM];

// material parameters
uniform sampler2D albedoSampler; // vec3
uniform sampler2D mrSampler; // g: roughness value | b: metalness
uniform sampler2D emissionSampler; // vec3
uniform sampler2D normalSampler; // initially 0-1
uniform sampler2D aoSampler;
uniform samplerCube depthSampler;
uniform float far;

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

};
uniform PointLight pointLights[POINT_LIGHT_NUM];

uniform vec3 camPos;

const float PI = 3.14159265359;


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

vec3 addDirectLight(vec3 wi, vec3 normal, vec3 albedo, float roughness, float metallic, float ao)
{
    // add point lights
    vec3 Lo = vec3(0.0);
    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = mix(vec3(0.03), albedo, metallic);
    for(int i = 0; i < POINT_LIGHT_NUM; i++)
    {
        vec3 Lraw = pointLights[i].pos - posWorld;
        vec3 L;
        if(pointLights[i].directional)
            L = -normalize(pointLights[i].direction);
        else
            L = normalize(Lraw);
        float distance = length(pointLights[i].pos - posWorld);
        float maxDepth = texture(depthSampler, -Lraw).r;
        maxDepth *= far;
        if(i==0 ) {
            float light_distance;
            if(pointLights[i].directional)
                light_distance = length(dot(pointLights[i].direction, -Lraw));
            else
                light_distance = distance;
            if(maxDepth < light_distance - 0.15)
            // occluded
                continue;
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
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
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
    vec3 albedo = vec3(albedoA);
    if(albedoA.a < 0.1)
        discard;
    float metallic = texture(mrSampler, TexCoord).b;
    float roughness = texture(mrSampler, TexCoord).g;
    float ao = texture(aoSampler, TexCoord).r;
    // ao is invalid now, so use 1 instead
    ao = 1;
    // reflectance equation
    vec3 Lo = addDirectLight(V, N, albedo, roughness, metallic, ao);
    vec3 Le;
    if(globalEmission == vec3(0.0,0.0,0.0))
        Le = vec3(texture(emissionSampler, TexCoord));
    else
        Le = globalEmission;
    
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo + Le;
    float brightness =  dot(color, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > bloomThreashold)
        Le += color;
    FragColor = vec4(color, 1.0);
    EmitColor = vec4(Le, 1.0);
    //FragColor = vec4(pointLights[0].irradiance, 0);
    //FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}