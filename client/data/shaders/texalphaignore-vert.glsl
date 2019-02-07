varying vec2 uv;
varying vec4 vertexColor;
void main(void)
{
   gl_Position = ftransform();
   uv = gl_MultiTexCoord0.xy;
   vertexColor = gl_Color;
}

