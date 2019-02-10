#version 120

// gauss filter with variable radius

uniform sampler2D texture;
uniform vec2 texelOffset;
uniform float radius;
uniform float kernel[32];

varying vec2 uv;

void main()
{
   vec2 pos= uv;
   vec2 neg= uv;

   // center element
   vec4 col = texture2D(texture, pos);

   float alpha = col.a * kernel[0];

   // process [1..radius] and [-1..-radius]
   for (int i=1; i< int(radius); i++)
   {
      neg -= texelOffset;

      float a2= texture2D(texture, neg).a;

      alpha += a2 * kernel[i];
   }

   for (int i=1; i< int(radius); i+=2)
   {
      pos += texelOffset;

      float a1= texture2D(texture, pos).a;

      alpha += a1 * kernel[i];
   }

   col= (  col
         + texture2D(texture, uv+texelOffset)
         + texture2D(texture, uv-texelOffset)
        ) * 0.33333;

//   alpha= max(alpha, col.a);
   alpha = alpha * 3.0;

   gl_FragColor= vec4(col.r*alpha, col.r*alpha*0.5, 0.0, alpha);
}
