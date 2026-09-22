#version 300 es
precision mediump float;

// GLES3 port of client/data/shaders/invisibility-frag.glsl (texture2D -> texture, varying -> in,
// gl_FragColor -> an explicit out). The vertex stage's "pos" varying is named v_pos here since
// GLSL ES reserves nothing special about "pos" - kept distinct only for clarity between the two
// files.

uniform sampler2D texturemap;
uniform sampler2D gradientmap;

uniform float fadeThreshold;

const float fadeRange = 0.25;

in vec3 eye;
in vec2 uv;
in vec3 nrm;
in vec4 v_pos;
out vec4 o_color;

void main()
{
   float fade = texture(gradientmap, uv).r;

   float alpha = 0.0;
   if (fade < fadeThreshold)
      alpha = 0.0;
   else if (fade < fadeThreshold + fadeRange)
      alpha = (fade - fadeThreshold) / fadeRange;
   else
      alpha = 1.0 + (fade - fadeThreshold - fadeRange) / fadeRange;

   float factor = 0.0;
   if (alpha > 1.0 && alpha < 2.0)
   {
      if (alpha < 1.5)
         factor = (alpha - 1.0) * 2.0;
      else
         factor = 1.0 - (alpha - 1.5) * 2.0;
      alpha = 1.0;
   }

   vec2 glassUv = v_pos.xy / v_pos.w * 0.5 + 0.5 + nrm.xy * 0.02;
   vec4 tex = texture(texturemap, glassUv) * (1.0 + factor * 4.0);

   o_color = vec4(tex.xyz, alpha);
}
