uniform sampler2D texturemap; // color + specular (in alpha)
uniform sampler2D normalmap;  // normal + clouds (in alpha)

uniform vec3 moveCloud;

// varying vec3 normal;
varying vec2 uv;
varying vec3 lightDir;
varying vec3 viewDir;

vec4 specularColor= vec4(1.0, 0.8, 0.8, 1.0);

void main()
{
    // result color
    vec4 col = vec4(0.0, 0.0, 0.0, 0.0);

    // texture colors
    vec4 texColor= texture2D(texturemap, uv);

    float cloudColor= texture2D(normalmap, uv + moveCloud.xy).a;
    float cloudShadow= texture2D(normalmap, uv + moveCloud.xy + vec2(lightDir.x, -lightDir.y)*0.0025).a;

    // specular level
    float specularVal = texColor.a * 0.5 + 0.5;

    // less specular on clouds
    specularVal *= max(1.0 - cloudColor*cloudColor*1.5, 0.0);

    // normal from normalmap
    vec3 normal = texture2D(normalmap, uv).xyz * 2.0 - 1.0;

    // re-normalize light- and view-dir
    vec3 ld= normalize(lightDir);
    vec3 vd= normalize(viewDir);

    // diffuse intensity
    float intensity = dot(normal, ld) * 1.5;// - 0.5;
    intensity= max(intensity, 0.0);

    col += vec4(texColor.rgb, 1.0) * intensity;// * color;

    // reflection vector
    vec3 refl = vd - normal * 2.0 * dot(normal, vd);

    // specular
    float specular = dot(refl, ld);

    if (specular > 0.8)  // 0.8^30 = 0.0
    {
       float s = pow(specular, 30.0);

       col += specularColor * s * specularVal;
    }

    col= col*(1.0 - cloudShadow*0.5);

    float fresnel= 1.0 - vd.z*vd.z;
    if (fresnel > 0.5)
    {
        float f = pow(fresnel, 10.0);
        col = col * (1.0-f) + vec4(0.7, 0.9, 1.5, 1.0) * f;
    }

    col += cloudColor;

    gl_FragColor = col;
}

