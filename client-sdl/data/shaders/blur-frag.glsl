#version 300 es
precision mediump float;

// GLES3 port of client/data/shaders/blur-frag.glsl - gauss filter with variable radius.
// The sampler uniform is renamed texture -> texture0: GLSL ES 3.00 reserves `texture` as the
// built-in sampling function name, so a uniform of that exact name no longer compiles.

uniform float texelOffsetX;
uniform float texelOffsetY;
uniform float clampU;
uniform float clampV;
uniform float radius;
uniform sampler2D texture0;
uniform float kernel[32];

in vec2 uv;

out vec4 o_color;

void main()
{
   // uv-step to next pixel in source image
   vec2 texelOffset= vec2( texelOffsetX, texelOffsetY );

   // center element
   vec4 col = texture(texture0, uv) * kernel[0];

   // process [1..radius] and [-1..-radius]
   vec2 pos= uv;
   vec2 neg= uv;
   for (int i=1; i< int(radius); i++)
   {
      pos = min( pos + texelOffset, vec2(clampU, clampV) );
      neg -= texelOffset;

      vec4 c1= texture(texture0, pos);
      vec4 c2= texture(texture0, neg);

      col += (c1 + c2) * kernel[i];
   }

   o_color= col;
}
