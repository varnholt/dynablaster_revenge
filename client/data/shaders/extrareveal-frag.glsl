uniform sampler2D texturemap;
uniform vec4 color;
varying vec2 uv1;
varying vec2 uv2;

void main()
{
   vec4 col1= texture2D(texturemap, uv1);
   vec4 col2= texture2D(texturemap, uv2);
   
   gl_FragColor = vec4( (col1.rgb + col2.rgb) * 0.5, color.a);
}
