#version 300 es
precision mediump float;

// GLES3 port of client/data/shaders/destruction-frag.glsl (texture2D -> texture, varying -> in,
// gl_FragColor -> an explicit out).

uniform sampler2D texturemap;
uniform sampler2D diffusemap;
uniform sampler2D specularmap;
uniform sampler2D shadowmap;

in vec3 eye;
in vec2 uv;
in vec3 nrm;
in vec2 uv3;
in float faceShadow;
out vec4 o_color;

vec4 textureSphere(sampler2D map, vec3 dir)
{
   vec3 t = dir + vec3(0.0, 0.0, 1.0);
   float m = 0.5 / sqrt(dot(t, t));
   t = vec3(0.5, 0.5, 0.5) + vec3(t.x, -t.y, t.z) * m;
   return texture(map, t.xy);
}

void main()
{
   vec3 n = normalize(nrm);
   vec3 e = normalize(eye);
   vec3 refl = e - n * 2.0 * dot(n, e);

   vec4 spec = textureSphere(specularmap, refl);
   vec4 diff = textureSphere(diffusemap, n);
   vec4 tex = texture(texturemap, uv);

   vec4 shad = texture(shadowmap, uv3);

   // blend shadow map with facing factor
   // 1.0 -> full shadow map
   // 0.0 -> completely blending to white
   shad = shad * faceShadow + vec4(1.0, 1.0, 1.0, 1.0) * (1.0 - faceShadow);

   vec4 final = (diff * tex * shad) + (spec * tex.a);

   o_color = vec4(final.rgb, 1.0);
}
