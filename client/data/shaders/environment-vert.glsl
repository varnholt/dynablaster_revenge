// shader constants
uniform vec3 camera;

// output
varying vec3 eye;
varying vec3 nrm;

void main()
{	
   nrm= normalize(gl_NormalMatrix * gl_Normal);
   eye= normalize(gl_NormalMatrix * (gl_Vertex.xyz - camera));
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
