uniform float gamma;
varying vec2 uv;
uniform sampler2D texture;

void main()
{
   const float invGamma= 1.0 / 2.2;

   vec4 col= texture2D(texture, uv);

   float x= pow(col.x, invGamma);
   float y= pow(col.y, invGamma);
   float z= pow(col.z, invGamma);
   
   x= pow(x, gamma);
   y= pow(y, gamma);
   z= pow(z, gamma);

   gl_FragColor= vec4(x,y,z,col.w);
}
