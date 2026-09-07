// simple object fragment shader

// per-frame data
layout(std140)                  // standard layout
uniform SceneData {             // like a class name
    mat4 ProjFromWorld, WorldFromProj;
    vec4 LightDir;
};

// active textures
uniform sampler2D ColorTexture;
uniform sampler2D NormTexture;
uniform sampler2D PropsTexture; // r=roughness, g=AO, b=displacement

// input from vertex shader
in vec2 texcoord;
in vec3 normal;
in vec4 position;

// output to frame buffer
out vec4 fragColor;

void main() {
    // default output color
    fragColor = vec4(0,0,0,1);
    
    // solid color or from texture
    vec3 color = vec3(.5);
    if (textureSize(ColorTexture,0) != ivec2(1,1))
        color = texture(ColorTexture, texcoord).rgb;

    // un-bumped normal, or from texture
    vec3 norm = vec3(0,0,1);
    if (textureSize(NormTexture,0) != ivec2(1,1))
        norm = texture(NormTexture, texcoord).rgb * 2. - 1.;
    
    // default properties, or from texture
    vec3 props = vec3(0,0,1);
    if (textureSize(PropsTexture,0) != ivec2(1,1))
        props = texture(PropsTexture, texcoord).rgb;
    float specPower = exp2(13.*props.r);
    float D = props.g;
    float AO = props.b * 0.1;

    // compute tangents and bump normal by method from
    // https://jcgt.org/published/0009/03/04/
    vec3 P = position.xyz / position.w;     // surface position
    vec3 N = normalize(normal);             // surface normal
    mat2 dUVdXY = mat2(dFdx(texcoord), dFdy(texcoord));
    mat2 dXYdUV = inverse(dUVdXY);
    mat2x3 dPdXY = mat2x3(dFdx(P), dFdy(P));
    mat2x3 dPdUV = dPdXY * dXYdUV;
    vec3 T = normalize(cross(N,dPdUV[1]));
    vec3 B = normalize(cross(dPdUV[0],N));
    mat3 TBN = mat3(T,B,N);
    N = normalize(TBN * norm);
    

    // compute relevant vectors and dot products
    vec3 L = normalize(LightDir.xyz);       // light direction
    vec4 E = WorldFromProj * vec4(0,0,-1,0);// eye location
    vec3 V = normalize(E.xyz - P*E.w);      // view vector
    vec3 H = normalize(V+L);                // half vector
    float N_L = dot(N,L);
    float N_H = dot(N,H);
    float H_V = dot(H,V);
    
    // ambient with ambient occlusion, scaled by light intensity
    fragColor.rgb = color * AO * LightDir.a;
    
    // diffuse and specular
    float fres0 = 0.04;                     // incident Fresnel for IOR=1.5
    float diff = max(0., N_L);              // diffuse intensity
    float spec = pow(max(0., N_H),specPower) * (specPower * 0.125 + 0.25);
    float fres = fres0 + (1.-fres0) * pow(1.-max(0.,H_V),5.);
    fragColor.rgb += LightDir.a * diff * mix(color, vec3(1), fres);
}
