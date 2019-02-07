uniform float time;
uniform float circleOffset;
uniform sampler2D texture;

varying vec2 uv;

float duration = 1.5;
float fadeOutTime = 1.5;
float cutOffset = 0.05;


float getRadius(float u)
{
   // overall duration (duration + fade out time) exceeds 2
   float normalizedTime = time / duration;
   if (normalizedTime > 1.0)
      normalizedTime = 1.0;

   float radius = 0.0;

   // alpha unchanged        | not drawn
   // |###################///|--------------------|---->
   // ^                   ^                       ^
   // 0                   alpha changed           1    u
   //                                                  t

   if (u + cutOffset < normalizedTime)
      radius = 1.0;
   else if (u < normalizedTime)
      radius = (normalizedTime - u) / cutOffset;

   // start cutting off buttom when the whole thing is displayed to the half
   // animation time in vertex shader is 2, use vertex shader normalized time
   // here
   /*
   float nTimeVert = time * 0.5;
   if (nTimeVert > 0.5 && time < 2.0)
   {
      if (u < (nTimeVert * 0.5) + cutOffset)
         radius = ((nTimeVert * 0.5) - u) / cutOffset;
   }
   */

   return radius;
}


void main()
{
   vec4 col = vec4(uv.x, uv.x, 1.0, uv.x * 0.5);
   vec4 textureColor = texture2D(texture, vec2(uv.x - time*0.03, 0.1*uv.y));
   col *= textureColor;

   // fade out
   float alpha = 1.0;
   if (time > duration)
   {
      float fadeOutFactor = 1.0 - (time - duration)/fadeOutTime;
      alpha = uv.x * 0.5 * fadeOutFactor;
      col.a = alpha;
   }

   col.a *= getRadius(uv.x);

   float verticalAlpha = 0.0;
   if (uv.y >= 0.5)
       verticalAlpha = (1.0-uv.y);
   else
       verticalAlpha = (uv.y);
   col.a *= 2.0*verticalAlpha;

   col.r = circleOffset * 0.03;

   gl_FragColor = vec4(col);
}


