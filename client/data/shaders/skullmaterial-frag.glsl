#version 120

uniform sampler2D specularmap;
uniform sampler2D diffusemap;
uniform sampler2D shadowmap;
uniform sampler2D texturemap;

// input
varying vec3 eye;
varying vec3 nrm;
varying vec2 uv1;
varying vec2 uv2;
varying float faceShadow;

vec4 textureSphere(sampler2D map, vec3 dir)
{
   vec3 t= dir + vec3(0.0, 0.0, 1.0);
   float m= 0.5 / sqrt(dot(t,t));
   vec2 uv= vec2(0.5 + t.x * m, 0.5 - t.y * m);
   return texture2D(map, uv);
}

void main()
{
   // reflection vector
   vec3 n= normalize( nrm );
   vec3 e= normalize( eye );
   vec3 refl= e - n * 2.0 * dot(n, e);

   vec4 diff= textureSphere(diffusemap, n);
   vec4 spec= textureSphere(specularmap, refl);
   vec4 tex= texture2D(texturemap, uv1);

   vec4 shad= texture2D(shadowmap, uv2);

   shad= shad*faceShadow + vec4(1.0,1.0,1.0,1.0) * (1.0 - faceShadow);

   vec4 final= (diff * tex * shad) + (spec * tex.a);

   gl_FragColor = vec4(final.rgb, 1.0);
}
