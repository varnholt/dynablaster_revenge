#version 120

// varying vec3 normal;
varying vec4 color;
varying vec2 uv;
varying vec3 lightDir;
varying vec3 viewDir;
varying float fresnelAmount;

uniform vec3   lightPosition;
uniform vec3   cameraPosition;
uniform mat4   transformations[16];
uniform float  fresnelFactors[16];
uniform mat4   projection;

mat4 inverse(mat4 m)
{
   mat4 n;

   float det= m[0].x*(m[1].y*m[2].z - m[1].z*m[2].y)
            + m[0].y*(m[1].z*m[2].x - m[1].x*m[2].z)
            + m[0].z*(m[1].y*m[2].y - m[1].y*m[2].x);

   float inv= 1.0 / det;

   // generate adjoint matrix
   n[0].x= (m[1].y*m[2].z - m[1].z*m[2].y)*inv;
   n[1].x= (m[1].z*m[2].x - m[1].x*m[2].z)*inv;
   n[2].x= (m[1].x*m[2].y - m[1].y*m[2].x)*inv;
   n[3].x= 0.0;

   n[0].y= (m[0].z*m[2].y - m[0].y*m[2].z)*inv;
   n[1].y= (m[0].x*m[2].z - m[0].z*m[2].x)*inv;
   n[2].y= (m[0].y*m[2].x - m[0].x*m[2].y)*inv;
   n[3].y= 0.0;

   n[0].z= (m[0].y*m[1].z - m[0].z*m[1].y)*inv;
   n[1].z= (m[0].z*m[1].x - m[0].x*m[1].z)*inv;
   n[2].z= (m[0].x*m[1].y - m[0].y*m[1].x)*inv;
   n[3].z= 0.0;

   // new translation vector = negative transposed rot/scale mat *  old translation vector
   n[0].w= -(n[0].x*m[0].w) - (n[0].y*m[1].w) - (n[0].z*m[2].w);
   n[1].w= -(n[1].x*m[0].w) - (n[1].y*m[1].w) - (n[1].z*m[2].w);
   n[2].w= -(n[2].x*m[0].w) - (n[2].y*m[1].w) - (n[2].z*m[2].w);
   n[3].w= 1.0;

   return n;
}

void main()
{
   // texcoord.z contains the integer index of the transformation matrix
   int transIndex= int(gl_MultiTexCoord0.z);
   mat4 modelView= transformations[transIndex];

   // transform light- and camera-position from world- to object-space
   mat4 invModelView= inverse( modelView );
//   mat4 invModelView= invTransformations[transIndex];
   vec3 osLightPos= vec3(invModelView * vec4(lightPosition, 1.0));
   vec3 osCameraPos= vec3(invModelView * vec4(cameraPosition, 1.0));

   // pass uv to fragment shader
   uv = gl_MultiTexCoord0.xy;

   // build tangent space matrix
   vec3 x,y,z;
   z= gl_Normal;
   x= gl_MultiTexCoord1.xyz;
   y = cross(x, z);
   mat3 tangentSpaceMatrix = mat3(x, y, z);

   // direction vector from light to vertex
   lightDir= normalize( gl_Vertex.xyz - osLightPos );
   lightDir= lightDir * tangentSpaceMatrix;

   // direction vector from camera to vertex
   viewDir= normalize( gl_Vertex.xyz - osCameraPos );
   viewDir= viewDir * tangentSpaceMatrix;

   fresnelAmount= fresnelFactors[transIndex];
   color = vec4(gl_MultiTexCoord0.www, gl_Color.w);

   // transform vertex to camera space
   gl_Position = projection * modelView * gl_Vertex;
}
