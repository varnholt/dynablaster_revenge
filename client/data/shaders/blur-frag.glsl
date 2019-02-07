// gauss filter with variable radius

uniform float texelOffsetX;
uniform float texelOffsetY;
uniform float clampU;
uniform float clampV;
uniform float radius;
uniform sampler2D texture;
uniform float kernel[32];
varying vec2 uv;

void main()
{
   // uv-step to next pixel in source image
   vec2 texelOffset= vec2( texelOffsetX, texelOffsetY );

   // center element
   vec4 col = texture2D(texture, uv) * kernel[0];

   // process [1..radius] and [-1..-radius]
   vec2 pos= uv;
   vec2 neg= uv;
   for (int i=1; i< int(radius); i++)
   {
      pos = min( pos + texelOffset, vec2(clampU, clampV) );
      neg -= texelOffset;

      vec4 c1= texture2D(texture, pos);
      vec4 c2= texture2D(texture, neg);

      col += (c1 + c2) * kernel[i];
   }

   gl_FragColor= col;
}
