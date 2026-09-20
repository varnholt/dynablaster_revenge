#version 300 es
precision mediump float;

// GLES3 port of client/data/effects/spherefragments/shaders/simplelight-frag.glsl
// (texture2D -> texture, varying -> in, gl_FragColor -> an explicit out).

in vec3 normal;
in vec4 color;
in vec2 uv;
in vec3 lightDir;
in vec3 position;

uniform sampler2D texturemap;

out vec4 o_color;

void main()
{
   vec3 col = vec3(0.0, 0.0, 0.0);

   // diffuse intensity
   float intensity = dot(normal, lightDir);
   if (intensity > 0.0)
   {
      vec4 texColor = texture(texturemap, uv);
      col = texColor.xyz * intensity;
   }

   // reflection vector
   vec3 dir = normalize(position);
   vec3 refl = dir - normal * 2.0 * dot(normal, dir);

   // specular
   float specular = dot(refl, lightDir);

   if (specular > 0.0)
   {
      float s = pow(specular, 30.0);
      col += vec3(s, s, s);
   }

   o_color = vec4(col, color.a);
}
