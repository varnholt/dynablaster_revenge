uniform float time;
uniform sampler2D texture;
uniform vec3 color;

varying float alpha;
varying float index;
varying vec2 uv;
// varying float debug;


vec3 getColor()
{
   vec3 mixed;
   vec3 white = vec3(1.0, 1.0, 1.0);

   float val = 0.5 * (1.0 + sin(index));

   mixed = (1.0 - val) * white + val * color;

   return mixed;
}


void main()
{
   vec4 textureColor = texture2D(texture, vec2(uv.x, uv.y));

   float alphaDec = alpha - (time/10.0);

   vec3 mixedColor = getColor();

   gl_FragColor = vec4(
      textureColor.r * mixedColor.r,
      textureColor.g * mixedColor.g,
      textureColor.b * mixedColor.b,
      textureColor.a * alphaDec
   );
}


