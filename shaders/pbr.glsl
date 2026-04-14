
const float kCRIOULO_PI = 3.14159265359;

struct SurfaceProperties {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    vec3 normal;
    vec3 worldPosition;
};

// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anyways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap(vec3 pNormal, vec2 pTextChoord, vec3 pWorldPosition)
{
    vec3 tangentNormal = pNormal.xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(pWorldPosition);
    vec3 Q2  = dFdy(pWorldPosition);
    vec2 st1 = dFdx(pTextChoord);
    vec2 st2 = dFdy(pTextChoord);

    vec3 N   = normalize(pNormal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}


float crioulo_distribution_ggx(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = kCRIOULO_PI * denom * denom;

    return nom / denom;
}


float crioulo_geometry_schlick_ggx(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}


float crioulo_geometry_smith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = crioulo_geometry_schlick_ggx(NdotV, roughness);
    float ggx1 = crioulo_geometry_schlick_ggx(NdotL, roughness);

    return ggx1 * ggx2;
}


vec3 crioulo_fresnel_schlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


vec3 crioulo_pbr(SurfaceProperties pProperties, int pLightCount, vec3 pLightPositions[4], vec3 pLightColors[4], vec3 pCameraPosition)
{
    vec3 V = normalize(pCameraPosition - pProperties.worldPosition);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, pProperties.albedo, pProperties.metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < pLightCount; ++i)
    {
        // calculate per-light radiance
        vec3 L = normalize(pLightPositions[i] - pProperties.worldPosition);
        vec3 H = normalize(pProperties.normal + L);
        float distance = length(pLightPositions[i] - pCameraPosition);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = pLightColors[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = crioulo_distribution_ggx(pProperties.normal, H, pProperties.roughness);   
        float G   = crioulo_geometry_smith(pProperties.normal, V, L, pProperties.roughness);      
        vec3 F    = crioulo_fresnel_schlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(pProperties.normal, V), 0.0) * max(dot(pProperties.normal, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - pProperties.metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(pProperties.normal, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * pProperties.albedo / kCRIOULO_PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * pProperties.albedo * pProperties.ao;
    
    vec3 color = ambient + Lo;
    return color;
}
