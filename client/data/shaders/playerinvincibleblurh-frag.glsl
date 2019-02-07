// gauss filter with variable radius

uniform sampler2D texture;
uniform vec2 texelOffset;
uniform float radius;
uniform float kernel[32];

varying vec2 uv;

void main()
{
   // center element
   vec4 col = texture2D(texture, uv);

   float alpha = col.a * kernel[0];

   // process [1..radius] and [-1..-radius]
   vec2 pos= uv;
   vec2 neg= uv;
   for (int i=1; i< int(radius); i++)
   {
      pos += texelOffset;
      neg -= texelOffset;

      float a1= texture2D(texture, pos).a;
      float a2= texture2D(texture, neg).a;

      alpha += (a1 + a2) * kernel[i];
   }

   col= (  col
         + texture2D(texture, uv+texelOffset)
         + texture2D(texture, uv-texelOffset)
        ) * 0.33333;

//   alpha= max(alpha, col.a);

   gl_FragColor= vec4(1.0 - col.a, 0.0, 0.0, alpha);
}
