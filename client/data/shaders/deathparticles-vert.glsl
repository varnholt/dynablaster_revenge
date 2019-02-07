uniform float particleSize;

varying vec2 uv;

void main()
{
   // transfrom position
   vec4 p= gl_ProjectionMatrix * vec4( gl_Vertex.xyz, 1.0 );

   // copy texture coordinate
   uv= gl_MultiTexCoord0.xy;

   // alpha fades out over the life of the particle 1.0 -> 0.0
   float alpha= gl_Vertex.w;

   // particle size after 2d projection
   gl_PointSize = particleSize * sqrt(alpha) / p.w;

   gl_Position = p;
}
