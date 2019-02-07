// shader constants
uniform vec3 camera;

// output
varying vec3 eye;
varying vec2 uv;
varying vec3 nrm;

void main()
{	
   nrm= normalize(gl_NormalMatrix * gl_Normal);
   eye= normalize(gl_NormalMatrix * (gl_Vertex.xyz - camera));
   uv = gl_MultiTexCoord0.xy;
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
