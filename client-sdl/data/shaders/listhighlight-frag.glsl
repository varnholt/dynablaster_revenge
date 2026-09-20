#version 300 es
precision mediump float;

// GLES3 port of client/data/shaders/listhighlight-frag.glsl (texture2D -> texture, varying ->
// in, gl_FragColor -> an explicit out, gl_Color.a -> the "rowAlpha" uniform - see the vert
// shader for why).

uniform sampler2D textureClamp;
uniform sampler2D textureHighlight;
uniform float rowAlpha;

in vec2 uvClamp;
in vec2 uvHighlight;
out vec4 o_color;

void main()
{
   // texture that eliminates all color outside the table's boundaries
   vec4 clampTextureColor = texture(textureClamp, uvClamp);

   // texture of the current row
   vec4 highlightTextureColor = texture(textureHighlight, uvHighlight);

   o_color = vec4(
      highlightTextureColor.rgb,
      clampTextureColor.a * highlightTextureColor.a * rowAlpha
   );
}
