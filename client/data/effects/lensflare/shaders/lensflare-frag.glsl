#version 120

uniform sampler2D texture;

varying float alpha;
varying vec2 uv;
varying vec3 color;

void main()
{
   vec4 textureColor = texture2D(texture, vec2(uv.x, uv.y));

   gl_FragColor = vec4(
      textureColor.r * color.r * alpha,
      textureColor.g * color.g * alpha,
      textureColor.b * color.b * alpha,
      textureColor.a
   );
}


