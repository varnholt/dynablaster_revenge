#version 120

uniform vec2 sun;
uniform float time;
uniform float normalizedlength;

varying vec2 uv;
varying vec3 color;
varying float alpha;


vec2 rotate(vec2 p, float speed)
{
   float r = speed * time;

   mat2 rotation = mat2(
       vec2( cos(r), sin(r)),
       vec2(-sin(r), cos(r))
   );

   return rotation * p;
}


void main(void)
{
   // init
   color = gl_Normal;
   vec2 offset = gl_Color.rg;
   uv = gl_Color.ba;

   vec2 pos = gl_Vertex.xy;
   float scalar = gl_Vertex.z;
   float rotationSpeed = gl_Vertex.w;

   pos = rotate(pos, rotationSpeed);

   alpha = normalizedlength;

   // shift along ray
   vec2 shifted = sun * (1.0 - scalar);
   pos += shifted;

   // vertical shift
   vec2 vertical;
   vertical.x = sun.y;
   vertical.y = -sun.x;
   vertical *= offset.y;
   pos += vertical;

   // final pos
   gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 0.0, 1.0);
}

