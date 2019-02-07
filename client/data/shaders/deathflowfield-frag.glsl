uniform sampler2D vertexPosTexture;
uniform sampler2D vertexColTexture;
uniform sampler2D flowfieldTexture;

uniform vec3 center;
uniform float fieldScale;
uniform float timeDelta;

varying vec2 currentPos;

void main()
{
   vec4 pos= texture2D(vertexPosTexture, currentPos);
   vec4 col= texture2D(vertexColTexture, currentPos);

   float fade= col.x;
   vec2 offset= col.yz;

   // position in flowfield relativ to center
   // (with scale & offset to vary the flow effect)
   vec2 uv= (pos.xz - center.xz) * fieldScale + offset;
   vec4 dir= texture2D(flowfieldTexture, uv) - 0.5;

//   dir.x+=0.15;
//   dir.x*=1.5;
   dir.x= dir.x*2.0;
   dir.y= dir.y-0.1;
   dir.z= dir.z*1.0;

   pos.xyz += dir.xyz * timeDelta * 0.05;

   pos.w -= col.x * timeDelta;
   if (pos.w < 0.0) pos.w= 0.0;

   gl_FragColor = pos;
}
