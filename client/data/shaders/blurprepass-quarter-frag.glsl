// simple downsampling filter as pre-pass for gauss filter
uniform float texelOffsetX;
uniform float texelOffsetY;
varying vec2 uv;
uniform sampler2D texture;
uniform float clampU;
uniform float clampV;

// normalized filter coefficients
const int filterSize= 6;
const float kernel[filterSize]= {
    0.157787883994348,
    0.147777119354262,
    0.120737535633764,
    0.0844217775331092,
    0.0482655605151599,
    0.0199040649665314
};

/*
0.111703360985505,
0.236476025527881,
0.303641226973228,
0.236476025527881,
0.111703360985505,
*/

vec4 getSample(vec2 uv)
{
   uv.x= min(uv.x, clampU);
   uv.y= min(uv.y, clampV);

   return texture2D(texture, uv);
}

void main()
{
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
