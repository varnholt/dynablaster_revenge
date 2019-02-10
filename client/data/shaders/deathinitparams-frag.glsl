#version 120

uniform sampler2D depthmap;
uniform vec2 center;
uniform mat4 invProj;

varying vec2 pos;

float frands(vec2 pos, float minimum, float maximum)
{
    float d= dot( pos.xy, vec2(12.9898, 78.233) );
    float n= fract(sin(d) * 43758.5453);
    return minimum + n*(maximum-minimum);
}

void main()
{
   float z= texture2D( depthmap, (pos+1.0)*0.5 ).r;

   // unproject (x,y,z) to world space
   vec4 v= invProj * vec4(pos.x, pos.y, z*2.0-1.0, 1.0);

   // invert projection
   float t= 1.0 / v.w;

   vec3 p= v.xyz*t;

   vec4 a= vec4(
        frands(p.xy, 0.003, 0.005),
        0.5 + (p.y - center.y)*0.5 + frands(p.yz, -0.05, 0.05),
        0.1 + (p.x - center.x)*0.5 + frands(p.zx, -0.05, 0.05),
        0.0
   );

   gl_FragColor= a;
}
