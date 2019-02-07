// varying vec3 normal;
varying vec2 uv;
varying vec3 lightDir;
varying vec3 viewDir;

uniform vec3   osLightPos;
uniform vec3   osCameraPos;


void main()
{
   // pass uv to fragment shader
   uv= gl_MultiTexCoord0.xy;

   // build tangent space matrix
   vec3 z= normalize( gl_Vertex.xyz );
   vec3 x= gl_Normal.xyz;
   vec3 y= cross(x, z);

   mat3 tangentSpaceMatrix = mat3(x, y, z);

   // direction vector from light to vertex
   lightDir= normalize( gl_Vertex.xyz - osLightPos );
   lightDir= lightDir * tangentSpaceMatrix;

   // direction vector from camera to vertex
   viewDir= normalize( gl_Vertex.xyz - osCameraPos );
   viewDir= viewDir * tangentSpaceMatrix;

   // transform vertex to camera space
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
