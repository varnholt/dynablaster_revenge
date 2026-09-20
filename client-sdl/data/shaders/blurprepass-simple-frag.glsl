#version 300 es
precision mediump float;

// GLES3 port of client/data/shaders/blurprepass-simple-frag.glsl - simple downsampling filter as
// pre-pass for the gauss filter. Sampler renamed texture -> texture0, same reason as blur-frag.glsl.

uniform float texelOffsetX;
uniform float texelOffsetY;
in vec2 uv;
uniform sampler2D texture0;
uniform float clampU;
uniform float clampV;

out vec4 o_color;

// normalized filter coefficients
const int filterSize= 4;

void main()
{
   float kernel[filterSize];
   kernel[0]= 0.285375189654228;
   kernel[1]= 0.222250421348901;
   kernel[2]= 0.104983661619388;
   kernel[3]= 0.030078322204597;

   // uv-step to next pixel in source image
   vec2 texelOffset= vec2( texelOffsetX, texelOffsetY );

   vec4 col= texture(texture0, uv) * kernel[0];

   vec2 pos= uv;
   vec2 neg= uv;
   for (int i=1; i<filterSize; i++)
   {
      pos = min( pos + texelOffset, vec2(clampU, clampV) );
      neg -= texelOffset;

      vec4 c1= texture(texture0, pos);
      vec4 c2= texture(texture0, neg);

      col += (c1 + c2) * kernel[i];
   }

   o_color= col;
}
