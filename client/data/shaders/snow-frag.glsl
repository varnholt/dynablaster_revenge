#version 120

uniform sampler2D texturemap;
uniform vec4 color;

const float alpha = 0.35;

void main()
{
   vec4 tex= texture2D(texturemap, gl_PointCoord);

   gl_FragColor = tex * alpha;
}
