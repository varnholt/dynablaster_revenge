#version 300 es
precision mediump float;

// GLES3 port of client/data/shaders/fontoutlines-frag.glsl (texture2D -> texture, varying ->
// in/uniform, gl_FragColor -> an explicit out). "color" was a varying fed by gl_Color in the
// legacy vertex shader; since every real call site sets one constant color for the whole draw
// (see BitmapFont::setColor()), it's a uniform here instead.

in vec2 uv;
uniform sampler2D distanceMap;
uniform vec4 outlineColor;
uniform vec4 color;

uniform float aaRadius;
uniform float outlineRadius;
uniform float threshold;
uniform float sampleOffset;

out vec4 o_color;

float text;
float outline;

void sampleGlyph(vec2 sampleUv)
{
   // get distance from character
   // <= 0.0 is inside
   // > 0.0 is outside
   float dist= 0.5 - texture(distanceMap, sampleUv).a;

   if (dist <= threshold)
   {
      text += 1.0;
      outline += 1.0;
   }
   else if (dist < threshold+aaRadius)
   {
      float alpha= 1.0 - (dist - threshold) / aaRadius;
      text += alpha;
      outline += 1.0;
   }
   else if (dist < threshold+outlineRadius)
   {
      float alpha= 1.0 - (dist - threshold) / outlineRadius;
      outline += alpha;
   }
}

void main()
{
   text= 0.0;
   outline= 0.0;

   sampleGlyph(uv + vec2(-sampleOffset*0.61, -sampleOffset*0.35));
   sampleGlyph(uv + vec2( sampleOffset*0.35, -sampleOffset*0.61));
   sampleGlyph(uv + vec2( sampleOffset*0.61,  sampleOffset*0.35));
   sampleGlyph(uv + vec2(-sampleOffset*0.35,  sampleOffset*0.61));

   text *= 0.25;
   outline *= 0.25;

   float alpha= (text + outline * outlineColor.w) * color.w;

   alpha= min(alpha, 1.0);

   // the original desktop-GL shader divides by `alpha` unconditionally here, which is a
   // pre-existing divide-by-zero for every pixel outside the glyph's outline radius (the vast
   // majority of each character's quad, where text==outline==0.0 so alpha==0.0). Desktop GL
   // apparently tolerated the resulting NaN/Inf silently, but this GLES3/mediump driver
   // propagates it through blending and corrupts the destination pixel instead of leaving it
   // untouched - the practical symptom was every dynamically-drawn text string (list items,
   // anything not baked into the PSD background image) rendering completely invisible. Guard
   // the division; the RGB value is meaningless anyway when alpha is 0.
   o_color = vec4(
      alpha > 0.0
         ? (outlineColor.xyz * (1.0-text) + color.xyz * text) / alpha
         : vec3(0.0),
      alpha
   );
}
