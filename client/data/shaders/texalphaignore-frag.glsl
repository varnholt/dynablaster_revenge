#version 120

uniform float alpha;
varying vec4 vertexColor;
varying vec2 uv;
uniform sampler2D tex;
void main()
{
   vec4 color = texture2D(tex, uv);
   float a= alpha * vertexColor.a;
   gl_FragColor =  vec4(color.rgb, a);
}

