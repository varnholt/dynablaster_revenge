uniform float particleSize;

varying vec4 color;

const vec4 particleColor= vec4(0.0, 1.0, 0.3, 0.0);

void main()
{
   // transfrom position
   vec4 p= gl_ProjectionMatrix * vec4( gl_Vertex.xyz, 1.0 );

   // alpha fades out over the life of the particle 1.0 -> 0.0
   float alpha= gl_Vertex.w;// * gl_Color.w;

   // particle size after 2d projection
   gl_PointSize = particleSize * sqrt(alpha) / p.w;

   color= gl_Color;

   // blend color to nice green
//   color= gl_Color * alpha + particleColor * (1.0-alpha);

   gl_Position = p;
}
