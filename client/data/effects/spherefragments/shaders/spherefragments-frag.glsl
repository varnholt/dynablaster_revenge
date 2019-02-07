// varying vec3 normal;
varying vec4 color;
varying vec2 uv;
varying vec3 lightDir;
varying vec3 viewDir;
varying float fresnelAmount;

uniform sampler2D texturemap;
uniform sampler2D normalmap;
uniform sampler2D lavamap;

void main()
{
    // result color
    vec4 col = vec4(0.0, 0.0, 0.0, 0.0);

    // texture colors
    vec4 texColor1= texture2D(texturemap, uv);
    vec4 texColor2= texture2D(lavamap, uv);

    // specular level
    float specularMapVal = texColor1.a * 0.5 + 0.5;

    // normal from normalmap
    vec3 normal = texture2D(normalmap, uv).xyz * 2.0 - 1.0;

    // re-normalize light- and view-dir
    vec3 ld= normalize(lightDir);
    vec3 vd= normalize(viewDir);

    // diffuse intensity
    float intensity = dot(normal, ld) * 1.5 - 0.5;

    if (intensity > 0.0)
    {
       vec4 texCol= texColor1 * color.r + texColor2 * (1.0 - color.r);
       col += texCol * intensity;// * color;
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
        // TODO: fade fresnel when animating
        float f = pow(fresnel, 5.0) * fresnelAmount;
        col += vec4(0.7, 0.9, 1.5, 1.0) * f;
    }

    gl_FragColor = vec4(col.xyz, color.a);
//    gl_FragColor = vec4(vd.xyz, color.a);
}

