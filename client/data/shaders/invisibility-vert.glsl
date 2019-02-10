#version 120

// shader constants
uniform vec3 camera;
uniform mat4 bones[8];

// output
varying vec3 eye;
varying vec2 uv;
varying vec3 nrm;
varying vec4 pos;

void main()
{
   uv = gl_MultiTexCoord0.xy;

   // blend matrix
   mat4 mat;
   mat = bones[0] * gl_MultiTexCoord1.x;
   mat+= bones[1] * gl_MultiTexCoord1.y;
   mat+= bones[2] * gl_MultiTexCoord1.z;
   mat+= bones[3] * gl_MultiTexCoord1.w;
   mat+= bones[4] * gl_MultiTexCoord2.x;
   mat+= bones[5] * gl_MultiTexCoord2.y;
   mat+= bones[6] * gl_MultiTexCoord2.z;
   mat+= bones[7] * gl_MultiTexCoord2.w;

   vec4 v;
   v.x= mat[0].x * gl_Vertex.x + mat[1].x * gl_Vertex.y + mat[2].x * gl_Vertex.z + mat[3].x * gl_Vertex.w;
   v.y= mat[0].y * gl_Vertex.x + mat[1].y * gl_Vertex.y + mat[2].y * gl_Vertex.z + mat[3].y * gl_Vertex.w;
   v.z= mat[0].z * gl_Vertex.x + mat[1].z * gl_Vertex.y + mat[2].z * gl_Vertex.z + mat[3].z * gl_Vertex.w;
   v.w= mat[0].w * gl_Vertex.x + mat[1].w * gl_Vertex.y + mat[2].w * gl_Vertex.z + mat[3].w * gl_Vertex.w;

   // todo: blend normal
   nrm= normalize(gl_NormalMatrix * gl_Normal);

   eye= normalize(gl_NormalMatrix * (v.xyz - camera));
   pos= gl_ModelViewProjectionMatrix * v;

   gl_Position = pos;
}
