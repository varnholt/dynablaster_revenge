#version 120

uniform sampler2D texturemap;
uniform sampler2D gradientmap;

uniform float fadeThreshold;

const float fadeRange= 0.25;

// input
varying vec3 eye;
varying vec2 uv;
varying vec3 nrm;
varying vec4 pos;

void main()
{
   float fade= texture2D(gradientmap, uv).r;

   float alpha= 0.0;
   if (fade < fadeThreshold)
      alpha= 0.0;
   else if (fade < fadeThreshold+fadeRange)
      alpha= (fade - fadeThreshold) / fadeRange;
   else
      alpha= 1.0 + (fade - fadeThreshold - fadeRange) / fadeRange;

   float factor= 0.0;
   if (alpha > 1.0 && alpha < 2.0)
   {
      if (alpha < 1.5)
         factor= (alpha-1.0) * 2.0;
      else
         factor= 1.0 - (alpha-1.5) * 2.0;
      alpha= 1.0;
   }

   vec2 glassUv= pos.xy / pos.w * 0.5 + 0.5 + nrm.xy*0.02;
   vec4 tex= texture2D(texturemap, glassUv) * (1.0 + factor * 4.0);

   gl_FragColor = vec4(tex.xyz, alpha);
}
