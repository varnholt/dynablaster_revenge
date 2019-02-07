uniform float intensity;
uniform float time;
varying vec2 uv;
uniform sampler2D texture;

const float scale = 0.02;


vec2 getOffset(float time, vec2 uvTemp)
{
  float a = 1.0 + 0.5 * sin(time + uvTemp.x * 10.0);
  float b = 1.0 + 0.5 * cos(time + uvTemp.y * 10.0);
	
  return scale * vec2(a + sin(b), b + cos(a));
}


void main(void)
{
  vec4 origColor = texture2D(texture, uv);

  float speed = 5.0;

  float prevTime = speed * (time - 1.0);

  // current offset
  vec2 offset= getOffset(time, uv);	
	
  // offset at prev frame
  vec2 prevOffset = getOffset(prevTime, uv);

  // motion vector from previous to current frame
  vec2 delta = offset - prevOffset;

  vec2 uvTemp = uv;
  uvTemp += offset;
	
  vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
	
  // some iterations of unweighted blur
  const int steps = 20;
  float factor = 1.0 / float(steps);
  
  for (int i = 0; i < steps; i++)
  {
     color += texture2D(texture, uvTemp);
     uvTemp += delta * factor;
  }

  gl_FragColor = intensity * (color * factor) + ( (1.0 - intensity) * origColor);

  // gl_FragColor = color * factor;
}

