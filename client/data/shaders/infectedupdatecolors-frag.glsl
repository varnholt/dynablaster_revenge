uniform sampler2D positionmap;
uniform sampler2D colormap;

uniform mat4 proj;
uniform float stop;

varying vec2 pos2d;

void main()
{
   vec4 result= vec4(0.0, 0.0, 0.0, 0.0);

   vec4 pos= texture2D( positionmap, pos2d );
   if (pos.w >= 1.0 && stop < 0.5)
   {
       // this particle has just been reset:
       // pick a new color from the color map

       // project world space (x,y,z) to screen space
       vec4 p= proj * vec4(pos.xyz, 1.0);
       float t= 1.0 / p.w;
       vec2 uv= (p.xy*t + 1.0)*0.5;  // 0..1

       result= texture2D( colormap, uv );
   }

   gl_FragColor = result;
}
