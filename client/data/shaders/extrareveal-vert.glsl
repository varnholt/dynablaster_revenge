#version 120

uniform vec3 textureScroll;

varying vec2 uv1;
varying vec2 uv2;

void main()
{
   vec2 uvScale= vec2(2.0, 1.0);
   uv1= gl_MultiTexCoord0.xy * uvScale + vec2(textureScroll.x, 0.0);
   uv2= gl_MultiTexCoord0.xy * uvScale + vec2(textureScroll.y, 0.0);

   gl_Position = ftransform();
}
