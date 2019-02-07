varying vec3 normal;
varying vec4 color;
varying vec2 uv;
varying vec3 lightDir;
varying vec3 position;

void main()
{	
   // vec3 lightPosition = vec3(-10.0, 10.0, -100.0);
   vec3 lightPosition = vec3(30.0, 15.0, -100.0);

   position= vec3(gl_ModelViewMatrix * gl_Vertex);
   uv = gl_MultiTexCoord0.xy;

   gl_Position = gl_ProjectionMatrix * vec4(position, 1.0);
   lightDir = normalize(position - lightPosition);

   normal = normalize(gl_NormalMatrix * gl_Normal);

   color = gl_Color;
}
