varying vec4 color;
varying vec2 uv;
uniform sampler2D distanceMap;
uniform vec4 outlineColor;

uniform float aaRadius;
uniform float outlineRadius;
uniform float threshold;
uniform float sampleOffset;

float text;
float outline;

void sample(vec2 uv)
{
   // get distance from character
   // <= 0.0 is inside
   // > 0.0 is outside
   float dist= 0.5 - texture2D(distanceMap, uv).a;


   if (dist <= threshold)
   {
      text += 1.0;
      outline += 1.0;
   }
   else 
   if (dist < threshold+aaRadius)
   {
      float alpha= 1.0 - (dist - threshold) / aaRadius;
      text += alpha;
      outline += 1.0;
   }
   else
   if (dist < threshold+outlineRadius)
   {
      float alpha= 1.0 - (dist - threshold) / outlineRadius;
      outline += alpha;
   }
}

void main()
{
   text= 0.0;
   outline= 0.0;

   sample(uv + vec2(-sampleOffset*0.61, -sampleOffset*0.35));
   sample(uv + vec2( sampleOffset*0.35, -sampleOffset*0.61));
   sample(uv + vec2( sampleOffset*0.61,  sampleOffset*0.35));
   sample(uv + vec2(-sampleOffset*0.35,  sampleOffset*0.61));

   text *= 0.25;
   outline *= 0.25;
   
   float alpha= (text + outline * outlineColor.w) * color.w;

   alpha= min(alpha, 1.0);

   gl_FragColor = vec4(
      (outlineColor.xyz * (1.0-text) + color.xyz * text) / alpha,
      alpha
   );
}


