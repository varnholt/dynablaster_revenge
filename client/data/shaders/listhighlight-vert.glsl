varying vec4 color;
varying vec2 uvClamp;
varying vec2 uvHighlight;

void main()
{
   color = gl_Color;

   uvClamp = gl_MultiTexCoord0.xy;
   uvHighlight = gl_MultiTexCoord1.xy;

   gl_Position = ftransform();
}


