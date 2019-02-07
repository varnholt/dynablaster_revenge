// move particle positions along flow field and create
// new particle at random position when a particle faded out

uniform sampler2D vertexPosTexture;
uniform sampler2D vertexParamTexture;
uniform sampler2D flowfieldTexture;
uniform sampler2D depthTexture;

uniform vec3  center;
uniform float fieldScale;
uniform float timeDelta;
uniform vec4  srcRect;   // minx miny maxx maxy
uniform mat4  invProj;    // inverse projection
uniform float stop;

varying vec2 currentPos;

float frands(vec2 pos, float minimum, float maximum)
{
    float d= dot( pos.xy, vec2(12.9898, 78.233) );
    float n= fract(sin(d) * 43758.5453);
    return minimum + n*(maximum-minimum);
}

void main()
{
   vec4 pos= texture2D(vertexPosTexture, currentPos);
   vec4 param= texture2D(vertexParamTexture, currentPos);

   // decay
   pos.w -= param.x * timeDelta;

   if (pos.w > 0.0)
   {
       // move particle along flowfield

       // position in flowfield relativ to center
       // (with scale & offset to vary the flow effect)
       vec2 offset= param.yz;
       vec2 uv= (pos.xz - center.xz) * fieldScale + offset;
       vec4 dir= texture2D(flowfieldTexture, uv) - 0.5;

       dir.x= dir.x*1.8;
       dir.y= dir.y-0.0;
       dir.z= dir.z*1.8;

       pos.xyz += dir.xyz * timeDelta * 0.05;
   }
   else
   {
       if (stop < 0.5)
       {
           // random position in 2d source rect
           vec2 p2d= vec2(
              frands(pos.xy, srcRect.x, srcRect.z),
              frands(pos.yz, srcRect.y, srcRect.w)
           );

           // inverse project 2d -> 3d
           float z= texture2D( depthTexture, (p2d+1.0)*0.5 ).r;

           // unproject (x,y,z) to world space
           vec4 p3d= invProj * vec4(p2d.x, p2d.y, z*2.0-1.0, 1.0);

           // invert projection
           float t= 1.0 / p3d.w;

           pos= vec4(p3d.xyz*t, 1.0); // reset decay to 1.0
    //       pos= vec4(0.0, 0.0, 0.0, 1.0);
       }
       else
       {
           pos.w= 0.0;
       }
   }

   gl_FragColor = pos;
}
