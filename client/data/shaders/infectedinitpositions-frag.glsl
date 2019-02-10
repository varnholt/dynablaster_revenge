#version 120

uniform sampler2D depthmap;

uniform mat4 invProj;
uniform vec4 srcRect;   // minx miny maxx maxy

varying vec2 pos;

float frands(vec2 pos, float minimum, float maximum)
{
    float d= dot( pos.xy, vec2(12.9898, 78.233) );
    float n= fract(sin(d) * 43758.5453);
    return minimum + n*(maximum-minimum);
}

void main()
{
    vec2 p2d= vec2(
       frands(pos.xy, srcRect.x, srcRect.z),
       frands(pos.yx, srcRect.y, srcRect.w)
    );

   float z= texture2D( depthmap, (p2d+1.0)*0.5 ).r;

   // unproject (x,y,z) to world space
   vec4 p= invProj * vec4(p2d.x, p2d.y, z*2.0-1.0, 1.0);

   // invert projection
   float t= 1.0 / p.w;
//   float fade= frands(p.yx, 0.0, 1.0);
   gl_FragColor = vec4(p.xyz*t, 1.0);
}
