uniform float alpha;
varying vec4 vertexColor;
varying vec2 uv;
uniform sampler2D tex;
void main()
{
   vec4 color = texture2D(tex, uv);
   gl_FragColor = color * vertexColor * vec4(1.0, 1.0, 1.0, alpha);
}

