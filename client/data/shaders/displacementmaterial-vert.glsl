#version 120

uniform float time;
varying vec2 uv;
varying vec3 nrm;

float wave(float x, float y, float t)
{
   t= -fract(t/640.0)*5.0*800.0;
   x*=600.0;
   y*=1000.0;

   return 0.5 + 0.49 * cos(t+x / (128.0 + 32.0 * cos(y / 64.0+t))) * cos(y / (320.0 + 16.0 * sin(x / 64.0 )));
}

void main()
{
   const float eps= 0.02;
   float t= time + gl_ModelViewMatrix[3].x + gl_ModelViewMatrix[3].y + gl_ModelViewMatrix[3].z;

   // untransformed flag mesh is [0..1] in xz
   // front side is y>0, back side is y<0 (circa 0.05)
   vec3 pos= gl_Vertex.xyz;

   vec2 tex= gl_MultiTexCoord0.xy;

   // flagge ist am mast (u=0) fest -> amplitude=0
   float amp= sqrt(tex.x*9.0) / 3.0;
   if (amp > 1.0) amp = 1.0;

   pos.x += wave(tex.x, tex.y, t+0.0) * 0.2 * amp;
   pos.y += wave(tex.x, tex.y, t+0.6) * 0.3 * amp;
   pos.z += wave(tex.x, tex.y, t-0.4) * 0.2 * amp;

   float nx= wave(tex.x+eps, tex.y, t) - wave(tex.x-eps, tex.y, t);
   float nz= wave(tex.x, tex.y+eps, t) - wave(tex.x, tex.y-eps, t);
   nrm= normalize( gl_NormalMatrix * vec3(nx*amp, nz*amp, 0.02) );

   // transform to world position
   gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 1.0);
   uv = tex;
}
