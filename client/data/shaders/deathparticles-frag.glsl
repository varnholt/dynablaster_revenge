#version 120

uniform sampler2D texturemap;
uniform sampler2D colormap;

varying vec2 uv;

void main()
{
   vec4 maskColor= texture2D(texturemap, gl_PointCoord.xy);
   vec4 color= texture2D(colormap, uv);

   gl_FragColor = maskColor * color;
}
