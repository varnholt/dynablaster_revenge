uniform sampler2D depthmap;
uniform mat4 invProj;
uniform vec4 boundingRect; // minx,miny, maxx, maxy

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

   gl_FragColor = vec4(v.xyz*t, 1.0);
}
