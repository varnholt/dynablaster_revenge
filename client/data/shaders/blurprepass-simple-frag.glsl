// simple downsampling filter as pre-pass for gauss filter
uniform float texelOffsetX;
uniform float texelOffsetY;
varying vec2 uv;
uniform sampler2D texture;
uniform float clampU;
uniform float clampV;

// normalized filter coefficients
const int filterSize= 4;
float kernel[filterSize];

/*
0.111703360985505,
0.236476025527881,
0.303641226973228,
0.236476025527881,
0.111703360985505,
*/

void main()
{
   kernel[0]= 0.285375189654228;
   kernel[1]= 0.222250421348901;
   kernel[2]= 0.104983661619388;
   kernel[3]= 0.030078322204597;

   // uv-step to next pixel in source image
   vec2 texelOffset= vec2( texelOffsetX, texelOffsetY );

   vec4 col= texture2D(texture, uv) * kernel[0];

   vec2 pos= uv;
   vec2 neg= uv;
   for (int i=1; i<filterSize; i++)
   {
      pos = min( pos + texelOffset, vec2(clampU, clampV) );
      neg -= texelOffset;

      vec4 c1= texture2D(texture, pos);
      vec4 c2= texture2D(texture, neg);

      col += (c1 + c2) * kernel[i];
   }

   gl_FragColor= col;
}
