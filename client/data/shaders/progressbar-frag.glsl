uniform float progress;
uniform sampler2D texturemap;
varying vec2 uv;

void main()
{
   vec4 col= texture2D(texturemap, uv);
   
   vec4 clamped = vec4(
      col.r, 
      col.g, 
      col.b, 
      uv.x < progress ? col.a : 0.0
   );
   
   gl_FragColor = clamped;
}
