uniform float intensity;
uniform sampler2D texturemap;
varying vec2 uv;

void main()
{
   vec4 col= texture2D(texturemap, uv);
   
   float grey = col.r * 0.3 + col.g * 0.59 + col.b * 0.11;
   
   float invIntensity = 1.0 - intensity;
   float weightedGrey = intensity * grey;

   vec4 mixed = vec4(
      weightedGrey + invIntensity * col.r, 
      weightedGrey + invIntensity * col.g, 
      weightedGrey + invIntensity * col.b, 
      col.a
   );
   
   gl_FragColor = mixed;
}
