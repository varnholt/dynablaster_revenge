#version 300 es
precision highp float;

// GLES3 port of client/data/shaders/deathinitparams-frag.glsl - seeds each particle's per-particle
// fade rate / flowfield-sample offset (col.x = fade, col.yz = offset, read back by
// deathflowfield-frag.glsl). texture2D -> texture, gl_FragColor -> o_color, otherwise unchanged.

uniform sampler2D depthmap;
uniform vec2 center;
uniform mat4 invProj;

in vec2 pos;

out vec4 o_color;

float frands(vec2 p, float minimum, float maximum)
{
   float d = dot(p.xy, vec2(12.9898, 78.233));
   float n = fract(sin(d) * 43758.5453);
   return minimum + n * (maximum - minimum);
}

void main()
{
   float z = texture(depthmap, (pos + 1.0) * 0.5).r;

   vec4 v = invProj * vec4(pos.x, pos.y, z * 2.0 - 1.0, 1.0);
   float t = 1.0 / v.w;
   vec3 p = v.xyz * t;

   vec4 a = vec4(
      frands(p.xy, 0.003, 0.005),
      0.5 + (p.y - center.y) * 0.5 + frands(p.yz, -0.05, 0.05),
      0.1 + (p.x - center.x) * 0.5 + frands(p.zx, -0.05, 0.05),
      0.0
   );

   o_color = a;
}
