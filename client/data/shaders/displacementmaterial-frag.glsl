#version 120

uniform sampler2D texturemap;
uniform sampler2D diffusemap;

// input
varying vec2 uv;
varying vec3 nrm;

vec4 textureSphere(sampler2D map, vec3 dir)
{
   vec3 t= dir + vec3(0.0, 0.0, 1.0);
   float m= 0.5 / sqrt(dot(t,t));
//   float m= sqrt( 1.0 + t.z + 2.0*t.z*t.z );
   t= vec3(0.5, 0.5, 0.5) + vec3(t.x, -t.y, t.z)*m;
   return texture2D(map, t.xy);
}

void main()
{
   vec4 col= texture2D(texturemap, uv);
   vec4 diff= textureSphere(diffusemap, nrm);

   gl_FragColor = col * diff;
}
