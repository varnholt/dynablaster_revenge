#version 120

uniform sampler2D texturemap;
uniform vec2 fresnelFactor;

varying vec3 normal;
varying vec4 color;
varying vec2 uv;
varying vec3 lightDir;
varying vec3 position;

void main()
{
   vec3 col = vec3(0.0, 0.0, 0.0);

   // diffuse intensity
   float intensity = dot(normal, lightDir);
   if (intensity > 0.0)
   {
      vec4 texColor= texture2D(texturemap, uv);
      col = /*color.xyz * */ texColor.xyz * intensity;
   }

   // reflection vector
   vec3 dir = normalize(position);
   vec3 refl = dir - normal * 2.0 * dot(normal, dir);

   // specular
   float specular = dot(refl, lightDir);

   if (specular > 0.0)
   {
      float s = pow(specular, 30.0);
      col += vec3(s,s,s);
   }

   float fresnel= pow( fresnelFactor.x - abs(normal.z), fresnelFactor.y );
   gl_FragColor = vec4(col, color.a) + vec4(0.7, 0.9, 1.5, 1.0) * fresnel;
}
