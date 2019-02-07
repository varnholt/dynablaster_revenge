varying vec4 color;
varying vec2 uvClamp;
varying vec2 uvHighlight;
uniform sampler2D textureClamp;
uniform sampler2D textureHighlight;

void main()
{
   // texture that eliminates all color outside the table's boundaries
   vec4 clampTextureColor = texture2D(textureClamp, uvClamp);

   // texture of the current row
   vec4 highlightTextureColor = texture2D(textureHighlight, uvHighlight);
   
   gl_FragColor = 
      vec4(
         highlightTextureColor.r,
         highlightTextureColor.g,
         highlightTextureColor.b,
         clampTextureColor.a * highlightTextureColor.a * color.a
      );
}


