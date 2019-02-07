uniform float sizeFactor;

varying vec4 color;

void main()
{
   // copy color
   color= gl_Color;

   // calculate vertex position from base, camera vectors and size
   vec3 pos= gl_Vertex.xyz;// + (uv.x * cameraLeft + uv.y * cameraUp) * temp.w;

   // random motion
   pos.x += sin(gl_Vertex.z*26.0 - gl_Vertex.y*11.0) * 0.004;
   pos.y += cos(gl_Vertex.x*22.0 - gl_Vertex.z*14.0) * 0.004;

   pos.x += sin(gl_Vertex.x*6.0 - gl_Vertex.z*2.5) * 0.01;
   pos.y += cos(gl_Vertex.y*3.5 - gl_Vertex.z*5.0) * 0.01;

   // variiert fallgeschwindigkeit
   pos.z += sin(gl_Vertex.y*4.0 - gl_Vertex.x*3.0) * 0.05;

   // transfrom position
   vec4 p= gl_ModelViewProjectionMatrix * vec4(pos, 1.0);
   gl_Position = p;
   gl_PointSize = gl_Vertex.w * sizeFactor / p.w;
}
