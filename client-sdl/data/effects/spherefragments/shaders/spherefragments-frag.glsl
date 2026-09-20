#version 300 es
precision mediump float;

// GLES3 port of client/data/effects/spherefragments/shaders/spherefragments-frag.glsl
// (texture2D -> texture, varying -> in, gl_FragColor -> an explicit out).

in vec4 color;
in vec2 uv;
in vec3 lightDir;
in vec3 viewDir;
in float fresnelAmount;

uniform sampler2D texturemap;
uniform sampler2D normalmap;
uniform sampler2D lavamap;

out vec4 o_color;

void main()
{
    // result color
    vec4 col = vec4(0.0, 0.0, 0.0, 0.0);

    // texture colors
    vec4 texColor1= texture(texturemap, uv);
    vec4 texColor2= texture(lavamap, uv);

    // specular level
    float specularMapVal = texColor1.a * 0.5 + 0.5;

    // normal from normalmap
    vec3 normal = texture(normalmap, uv).xyz * 2.0 - 1.0;

    // re-normalize light- and view-dir
    vec3 ld= normalize(lightDir);
    vec3 vd= normalize(viewDir);

    // diffuse intensity
    float intensity = dot(normal, ld) * 1.5 - 0.5;

    if (intensity > 0.0)
    {
       vec4 texCol= texColor1 * color.r + texColor2 * (1.0 - color.r);
       col += texCol * intensity;
    }

    // reflection vector
    vec3 refl = vd - normal * 2.0 * dot(normal, vd);

    // specular
    float specular = dot(refl, ld);

    if (specular > 0.8)  // 0.8^30 = 0.0
    {
       float s = pow(specular, 30.0) * specularMapVal;

       col += vec4(s,s,s,0.0);
    }

    float fresnel= 1.0 - vd.z*vd.z;
    if (fresnel > 0.1)
    {
        float f = pow(fresnel, 5.0) * fresnelAmount;
        col += vec4(0.7, 0.9, 1.5, 1.0) * f;
    }

    o_color = vec4(col.xyz, color.a);
}
