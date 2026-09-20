#version 300 es

// GLES3 port of client/data/effects/spherefragments/shaders/spherefragments-vert.glsl.
// gl_Vertex/gl_Normal/gl_MultiTexCoord0/gl_MultiTexCoord1 become explicit attributes; gl_Color
// becomes a hardcoded 1.0 alpha - traced the original call site (SphereFragmentContainer::
// drawFragments(), always called right after mSocket->draw()'s cleanupGlParameter() leaves
// glColor4f(1,1,1,1) current, with no per-vertex color array bound) and confirmed gl_Color.w is
// always 1.0 at this draw. The hand-rolled inverse(mat4) is kept (renamed fragmentInverse() to
// avoid a name collision - this driver's GLSL ES 3.00 compiler already reserves `inverse` as a
// built-in despite the spec not requiring a mat4 inverse() until ES 3.10, and redeclaring it is a
// compile error: "built-in redefinition is not allowed"), not just legacy boilerplate.

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec4 a_texcoord0;
layout(location = 3) in vec3 a_tangent;

out vec4 color;
out vec2 uv;
out vec3 lightDir;
out vec3 viewDir;
out float fresnelAmount;

uniform vec3   lightPosition;
uniform vec3   cameraPosition;
uniform mat4   transformations[16];
uniform float  fresnelFactors[16];
uniform mat4   projection;

mat4 fragmentInverse(mat4 m)
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
   int transIndex= int(a_texcoord0.z);
   mat4 modelView= transformations[transIndex];

   // transform light- and camera-position from world- to object-space
   mat4 invModelView= fragmentInverse( modelView );
   vec3 osLightPos= vec3(invModelView * vec4(lightPosition, 1.0));
   vec3 osCameraPos= vec3(invModelView * vec4(cameraPosition, 1.0));

   // pass uv to fragment shader
   uv = a_texcoord0.xy;

   // build tangent space matrix
   vec3 x,y,z;
   z= a_normal;
   x= a_tangent;
   y = cross(x, z);
   mat3 tangentSpaceMatrix = mat3(x, y, z);

   // direction vector from light to vertex
   lightDir= normalize( a_position - osLightPos );
   lightDir= lightDir * tangentSpaceMatrix;

   // direction vector from camera to vertex
   viewDir= normalize( a_position - osCameraPos );
   viewDir= viewDir * tangentSpaceMatrix;

   fresnelAmount= fresnelFactors[transIndex];
   color = vec4(vec3(a_texcoord0.w), 1.0);

   // transform vertex to camera space
   gl_Position = projection * modelView * vec4(a_position, 1.0);
}
