#version 120

uniform sampler2D texturemap;

varying vec4 color;

void main()
{
   vec4 maskColor= texture2D(texturemap, gl_PointCoord.xy);

   gl_FragColor = maskColor * color;
}
