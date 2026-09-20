#version 300 es
precision mediump float;

// GLES3 port of client/data/shaders/displacementmaterial-frag.glsl (texture2D -> texture,
// varying -> in, gl_FragColor -> an explicit out).

uniform sampler2D texturemap;
uniform sampler2D diffusemap;

in vec2 uv;
in vec3 nrm;
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
   vec4 col = texture(texturemap, uv);
   vec4 diff = textureSphere(diffusemap, nrm);

   o_color = col * diff;
}
