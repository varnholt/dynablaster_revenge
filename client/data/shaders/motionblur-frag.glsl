#version 120

uniform float intensity;
uniform vec3 motionDir;
varying vec2 uv;
uniform sampler2D texture;


void main(void)
{
   vec4 origColor = texture2D(texture, uv);

   vec4 color = vec4(0.0, 0.0, 0.0, 0.0);

   const int steps = 20;
   float factor = 1.0 / float(steps);

   vec2 offset = vec2(0.0, 0.0);
   vec2 delta = motionDir.xy * factor;

   for (int i = 0; i < steps; i++)
   {
      color += texture2D(texture, uv + offset );
      offset += delta;
   }

   gl_FragColor = intensity * factor * color + ( (1.0 - intensity) * origColor);

   // gl_FragColor = color;
   // gl_FragColor = vec4(origColor.rg, 1.0f, origColor.a);
}

