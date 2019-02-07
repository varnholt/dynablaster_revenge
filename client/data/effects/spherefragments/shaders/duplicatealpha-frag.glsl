varying vec2 uv;

uniform vec4 color;
uniform sampler2D texturemap;

void main()
{	
   float alpha = texture2D(texturemap, uv).a;
   gl_FragColor = color * alpha;
}

