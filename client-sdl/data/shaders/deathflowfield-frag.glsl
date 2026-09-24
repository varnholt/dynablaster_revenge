#version 300 es
precision highp float;

// GLES3 port of client/data/shaders/deathflowfield-frag.glsl - this is the per-frame particle
// position update pass (a GPGPU technique: reads the previous position/param textures, writes
// the next frame's positions to o_color, which gets attached to a GL_RGBA32F render target).
// highp precision matters here - this is a simulation feedback loop running over many frames, and
// mediump would visibly drift. texture2D -> texture, gl_FragColor -> o_color, otherwise unchanged.

uniform sampler2D vertexPosTexture;
uniform sampler2D vertexColTexture;
uniform sampler2D flowfieldTexture;

uniform vec3 center;
uniform float fieldScale;
uniform float timeDelta;

in vec2 currentPos;

out vec4 o_color;

void main()
{
   vec4 pos = texture(vertexPosTexture, currentPos);
   vec4 col = texture(vertexColTexture, currentPos);

   float fade = col.x;
   vec2 offset = col.yz;

   // position in flowfield relative to center (with scale & offset to vary the flow effect)
   vec2 uv = (pos.xz - center.xz) * fieldScale + offset;
   vec4 dir = texture(flowfieldTexture, uv) - 0.5;

   dir.x = dir.x * 2.0;
   dir.y = dir.y - 0.1;
   dir.z = dir.z * 1.0;

   pos.xyz += dir.xyz * timeDelta * 0.05;

   pos.w -= col.x * timeDelta;
   if (pos.w < 0.0) pos.w = 0.0;

   o_color = pos;
}
