#version 330 core

/**
 * @see https://github.com/zz92118/Learn-Opengl-zz/blob/b3cbc576082c45e51abf0af1e723fbefb4178b41/8.guest/2022/7.area_lights/2.multiple_area_lights/7.multi_area_light.fs
 */

in vec3 vertexPosition;     // Position from the VS.
in vec3 vertexNormal;       // Normal from the VS.
in vec3 vertexColor;        // Color from the VS.
in vec2 vertexUV;           // UV coordinates from the VS.
in vec3 vertexTangent;     // Tangents from the VS.
in vec3 vertexBitangent;   // Bitangents from the VS.

// Defines the DS for light areas.
struct Light {
    float intensity;
    vec3 color;
    vec3 points[4];
};

// Defines the DS for materials.
struct Material {
    sampler2D diffuse;      // The texture that controls the base color.
    sampler2D normalmap;    // The texture that controls the normals.
    sampler2D specular;    // The texture that controls the roughness of the material.
    float roughness;         // Controls the specular of the material.
    float specular_mult;
};

struct MaterialAlt {
    vec3 diffuse;           // The texture that controls the base color.
    vec3 normalmap;         // The texture that controls the normals.
    vec3 specular;    // The texture that controls the roughness of the material.
    float roughness;         // Controls the specular of the material.
};

uniform mat4 Model;            // Imports the model matrix.
uniform mat4 View;            // Imports the View matrix.
uniform mat4 Projection;    // Imports the projection matrix.
uniform Light mainLight;            // The LTC lights.
uniform Material material;          // The material controling the object.
uniform MaterialAlt materialalt;    // The material controling the object.
uniform vec3 cameraPosition;        // Position of the camera.
uniform bool useAlt;                // Whether to use alt or regular material.
uniform float alpha;
uniform float beta;
uniform bool dust;
uniform float Csheen;
uniform int sheenType;
uniform sampler2D LTC1;             // For inverse M
uniform sampler2D LTC2;             // GGX norm, fresnel, 0(unused), sphere
uniform sampler2D SHEENCOEFFS;      // The sheen lookup table

out vec4 outColor;          // Outputs color in RGBA.

const float LUT_SIZE  = 64.0;                           // LTC Lookup table size.
const float LUT_SCALE = (LUT_SIZE - 1.0) / LUT_SIZE;    // How much is the lookup table scaled.
const float LUT_BIAS  = 0.5 / LUT_SIZE;                 // The bias this LTC presents.
const float gamma = 2.2;                                // The magnitude to use in the gamma correction.
const float PI = 3.1415926535897932384626433832795;

/**
 * In Real-Time Area Lighting: a Journey from Research to Production the authors
 * propose this function because they mention that acos causes artifacts due
 * to float precision.
 */
vec3 integrateEdge(vec3 v1, vec3 v2) {
    
    float x = dot(v1, v2);
    float y = abs(x);
    
    // This is described by them to get the approximations.
    float a = 0.8543985 + (0.4965155 + 0.0145206 * y) * y;
    float b = 3.4175940 + (4.1616724 + y) * y;
    
    float v = a / b;
    
    float theta = 0.0;
    if (x > 0.0) {
        theta = v;
    } else {
        theta = 0.5 * inversesqrt(max(1.0 - x * x, 1e-7)) - v;
    }
    
     return cross(v1, v2) * theta;

}

/**
 * Here the real LTC is computed.
 */
vec3 evaluateLTC(vec3 N, vec3 V, vec3 P, mat3 minv, vec3 points[4]) {
    
    // Build the orthonormal.
    vec3 t1 = normalize(V - N * dot(V, N));
    vec3 t2 = cross(N, t1);

    // Rotate area light as (t1, t2, N).
    minv = minv * transpose(mat3(t1, t2, N));
        
    // Transform the polygon from LTC to origin.
    vec3 L[4];
    L[0] = minv * (points[0] - P);
    L[1] = minv * (points[1] - P);
    L[2] = minv * (points[2] - P);
    L[3] = minv * (points[3] - P);

    // Apply the conditional horizon-clipping.
    // To do it, check if the point is behind the light.
    vec3 dir = points[0] - P; // LTC space
    vec3 lightNormal = cross(points[1] - points[0], points[3] - points[0]);
    bool behind = (dot(dir, lightNormal) < 0.0);

    // Normalise the cosines.
    L[0] = normalize(L[0]);
    L[1] = normalize(L[1]);
    L[2] = normalize(L[2]);
    L[3] = normalize(L[3]);

    // Apply the approx integration to these.
    vec3 vsum = vec3(0.0);
    vsum += integrateEdge(L[0], L[1]);
    vsum += integrateEdge(L[1], L[2]);
    vsum += integrateEdge(L[2], L[3]);
    vsum += integrateEdge(L[3], L[0]);

    // Form factor of the polygon in direction vsum.
    float len = length(vsum);
    
    // Check if it is behind the light.
    float z = vsum.z / len;
    // if (behind)
    //    z = -z;

    // Obtain the right cosine indexes.
    vec2 uv = vec2(z * 0.5f + 0.5f, len);
    uv = uv * LUT_SCALE + LUT_BIAS;

    // Fetch the form factor for horizon clipping
    float scale = texture(LTC2, uv).w;

    float sum = len * scale;

    // Outgoing radiance.
    return vec3(sum, sum, sum);
    
}

/**
 * Some texture maps are not stored as non-linear color spaces.
 * This function transforms them.
 */
vec3 powVec3(vec3 v, float p) {
    
    return vec3(pow(v.x, p), pow(v.y, p), pow(v.z, p));
    
}

/**
 * This function transforms a color to a linear value using the
 * gamma value for correction.
 */
vec3 toLinear(vec3 v) {
    
    return powVec3(v, gamma);
    
}

/**
 * Transforms a vec3 to a color using the gamma correction.
 */
vec3 toSRGB(vec3 v) {
    
    return powVec3(v, 1.0 / gamma);
    
}

/**
 * Calculates the azimuthal angle.
 */
float phi(vec3 v) {
    
    float p = atan(v.y, v.x);
    
    if (p < 0) {
        p += 2 * PI;
    }
    
    return p;
    
}

/**
 * Rotates the vector v around the axis given a certain angle.
 */
vec3 rotateVector(vec3 v, vec3 axis, float angle) {
    
    float s = sin(angle);
    float c = cos(angle);
    
    return v * c + axis * dot(v, axis) * (1.f - c) + s * cross(axis, v);
    
}

/**
 * Fetch the LTC coefficients in a 32x32 lookup table.
 */
vec3 fetchCoeffs(vec3 wo, float cosThetaO) {
    
    // Compute table indices and interpolation factors.
    float row = max(0.0, min(alpha, 1.0));
    float col = max(0.0, min(cosThetaO, 1.0));

    return texture(SHEENCOEFFS, vec2(col, row)).xyz;
    
}

/**
 * Evaluate the LTC distribution in its default coordinate system.
 */
float evalLTCSheen(vec3 wi, vec3 ltcCoeffs, vec3 N) {

    float aInv = ltcCoeffs[0];
    float bInv = ltcCoeffs[1];
    
    vec3 wiOrg = vec3(aInv * wi.x + bInv * wi.z, aInv * wi.y, wi.z);
    
    float len = length(wiOrg);
    //wiOrg = wiOrg / len;

    float det = aInv * aInv;
    float jacobian = det / (len * len * len);
    
    float cosThetaIOrg = clamp(dot(N, wiOrg), 0.0f, 1.0f);

    return cosThetaIOrg / PI * jacobian;
    
}

/**
 * The sheen layer we are going to use.
 */
vec3 sheenModel(vec3 worldPosition, vec3 worldNormal) {
    
    // Get the light position by getting its center.
    vec3 lightPosition = mainLight.points[0] +
                         mainLight.points[1] +
                         mainLight.points[2] +
                         mainLight.points[3];
    lightPosition /= 4.0;
        
    // Calculate the view direction and the light direction.
    vec3 wo = normalize(cameraPosition - worldPosition);
    vec3 wi = normalize(worldPosition - lightPosition);

    vec3 N = normalize(worldNormal);
    
    // Calculate its cosTheta values.
    float cosThetaO = clamp(dot(N, wo), 0.0, 1.0);
    float cosThetaI = clamp(dot(N, wi), 0.0, 1.0);
    
    // Rotate coordinate frame to align with incident direction wo.
    float phiStd = phi(wo);
    vec3 wiStd = rotateVector(wi, vec3(0.0, 0.0, 1.0), -phiStd);
    
    // Evaluate LTC distribution in aligned coordinates.
    vec3 ltcCoeffs = fetchCoeffs(wo, cosThetaO);
    float value = evalLTCSheen(wiStd, ltcCoeffs, N);

    // Consider the overall reflectance `R` and the artist-specified sheen scale.
    float R = ltcCoeffs[2];
    value *= R * Csheen;
    
    float res = value; // cosThetaI;
    res = clamp(res, 0.0, 1.0);

    return vec3(res, res, res);
        
}

/**
 * A continuous function f(a,b) where f(0,b)=0, and f(1,b)=1 and the the
 * exponentially of the values are controlled by b.
 */
float scaleFloat(float a, float b) {
    
    return (1 - exp(-b * a)) / (1 - exp(-b));
    
}

vec3 cosineSheen(vec3 worldPosition, vec3 worldNormal) {
        
    // Calculate the view direction and the light direction.
    vec3 worldCam = (inverse(transpose(View)) * vec4(cameraPosition, 1.0)).xyz;
    vec3 wo = normalize(worldCam - worldPosition);
    
    vec3 N = abs(normalize(worldNormal));
    
    wo = vec3(0.0, 0.0, 1.0);
    
    // Calculate its cosTheta values.
    float cosThetaO = dot(N, wo);
    
    // Obtain the angle from 0 to 1.
    // Now we will get the sheen using alpha by limiting what is considered sheen.
    // Now we are only considering as sheen what is in the range 1-alpha - 1.0;
    float sheen = 1.0 - scaleFloat(cosThetaO, beta);
    
    return vec3(sheen, sheen, sheen);
        
}

void main() {
    
    // Selec the right elements for the materials.
    vec3 diffuse_val;
    vec3 specular_val;
    vec3 normals_val;
    float roughness_val;
    if (useAlt) {
        diffuse_val = materialalt.diffuse;
        specular_val = materialalt.specular;
        normals_val = materialalt.normalmap;
        roughness_val = materialalt.roughness;
        specular_val = toLinear(specular_val);
    } else {
        diffuse_val = texture(material.diffuse, vertexUV).xyz;
        specular_val = texture(material.specular, vertexUV).xyz * material.specular_mult;
        normals_val = texture(material.normalmap, vertexUV).xyz;
        roughness_val = material.roughness;
        specular_val = toLinear(specular_val);
    }
    
    // Get the matrix to convert stuff to tangent space.
    mat3 toTangentSpace = mat3(normalize(vertexTangent), normalize(vertexBitangent), normalize(vertexNormal));
    
    // It was passed in world space.
    vec3 worldPosition = vertexPosition;
    
    // We just transformed them in world space.
    vec3 worldNormal = normalize(toTangentSpace * normals_val.xyz);
    
    // SHEEN MODEL.
    if (dust) {
        worldNormal = normalize(vertexNormal);
    }
    
    // Control those normals that are backwards.
    if (gl_FrontFacing) {
        worldNormal = -worldNormal;
    }
    
    vec3 result = vec3(0.0f);

    vec3 N = normalize(worldNormal);
    vec3 V = normalize(cameraPosition - worldPosition);
    vec3 P = worldPosition;
    float dotNV = clamp(dot(N, V), 0.0f, 1.0f);

    // Use roughness and sqrt(1-cos_theta) to sample M_texture
    vec2 uv = vec2(roughness_val, sqrt(1.0f - dotNV));
    uv = uv * LUT_SCALE + LUT_BIAS;

    // Get 4 parameters for inverse_M
    vec4 t1 = texture(LTC1, uv);

    // Get 2 parameters for Fresnel calculation
    vec4 t2 = texture(LTC2, uv);

    mat3 Minv = mat3(
        vec3(t1.x, 0, t1.y),
        vec3(  0,  1,    0),
        vec3(t1.z, 0, t1.w)
    );

    // Evaluate LTC shading
    vec3 diffuse = evaluateLTC(N, V, P, mat3(1), mainLight.points);
    vec3 specular = evaluateLTC(N, V, P, Minv, mainLight.points);

    // GGX BRDF shadowing and Fresnel
    // t2.x: shadowedF90 (F90 normally it should be 1.0)
    // t2.y: Smith function for Geometric Attenuation Term, it is dot(V or L, H).
    specular *= specular_val * t2.x + (1.0f - specular_val) * t2.y;

    // Add contribution
    result += mainLight.color * mainLight.intensity * (specular + diffuse_val * diffuse);
    result = toSRGB(result);
    
    // SHEEN MODEL.
    if (useAlt) {
        
        if (sheenType == 1) {
            
            // Apply the sheen model.
            vec3 sheenLayer = sheenModel(worldPosition, worldNormal);
            result += sheenLayer;
            
        } else if (sheenType == 2) {
                        
            vec3 sheenLayer = cosineSheen(worldPosition, worldNormal);
            
            result += sheenLayer * (Csheen / 10.0);
            
        }
        
    }
    
    // SHEEN MODEL.
    if (dust) {
        
        if (sheenType == 1) {
            
            // Apply the sheen model.
            vec3 sheenLayer = sheenModel(worldPosition, worldNormal);
            
            // Get how up the normal is.
            float howUp = clamp(-worldNormal.y, 0.0, 1.0);
            
            result += sheenLayer * howUp;
                        
        } else if (sheenType == 2) {
                        
            vec3 sheenLayer = cosineSheen(worldPosition, worldNormal);
            
            // Get how up the normal is.
            float howUp = clamp(-worldNormal.y, 0.0, 1.0);

            result += sheenLayer * (Csheen / 10.0) * howUp;
                        
        }
        
    }
    
    outColor = vec4(result, 1.0f);

}
