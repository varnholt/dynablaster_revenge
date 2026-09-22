#version 300 es
precision mediump float;

// GLES3 port of client/data/shaders/blockmaterial-frag.glsl (texture2D -> texture, varying -> in,
// gl_FragColor -> an explicit out).

uniform sampler2D specularmap;
uniform sampler2D diffusemap;
uniform sampler2D shadowmap;
uniform sampler2D texturemap;
uniform sampler2D ambientmap;

in vec3 eye;
in vec3 nrm;
in vec2 uv1;
in vec2 uv2;
in vec2 uv3;
in float faceShadow;
out vec4 o_color;

vec4 textureSphere(sampler2D map, vec3 dir)
{
   vec3 t = dir + vec3(0.0, 0.0, 1.0);
   float m = 0.5 / sqrt(dot(t, t));
   vec2 uv = vec2(0.5 + t.x * m, 0.5 - t.y * m);
   return texture(map, uv);
}

void main()
{
   vec3 n = normalize(nrm);
   vec3 e = normalize(eye);
   vec3 refl = e - n * 2.0 * dot(n, e);

   vec4 diff = textureSphere(diffusemap, n);
   vec4 spec = textureSphere(specularmap, refl);
   vec4 tex = texture(texturemap, uv1);
   vec4 ambi = texture(ambientmap, uv2);

   vec4 shad = texture(shadowmap, uv3);

   shad = shad * faceShadow + vec4(1.0, 1.0, 1.0, 1.0) * (1.0 - faceShadow);

   vec4 final = (ambi.r * diff * tex * shad) + (spec * tex.a);

   o_color = vec4(final.rgb, 1.0);
}
