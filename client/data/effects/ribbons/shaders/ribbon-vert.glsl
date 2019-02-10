#version 120

uniform float time;
uniform vec4 fieldPosition;
uniform float circleOffset;

varying vec2 uv;


float getRadius(float u)
{
   float radius = 0.0;
   float normalizedTime = time * 0.5;

   // cut off front
   if (u < normalizedTime)
      radius = 3.0;

   // cut off back
   if (u < normalizedTime * 0.5)
      radius = 0.0;

   return radius;
}


float wave(float x, float y, float t)
{
   return 0.5 * (sin(t * 0.05 * (cos(x * 5.0 + 5.0 * t))));
}


vec3 f(float x)
{
   float dir = mod(floor(circleOffset), 2.0) == 0.0 ? 1.0 : -1.0;

   x += 0.2 * circleOffset;

   uv = gl_MultiTexCoord0.xy;

   float u = uv.x;
   float v = uv.y;

   float radius = u * getRadius(u);

   float px = dir * radius * (1.0+cos(dir * x + circleOffset)) * 50.0 * wave(u,u,u);
   float py = dir * radius * (1.0+sin(dir * x + circleOffset)) * 50.0 * wave(u,u,u);
   float pz = 6.0 * u;

   float nx = wave(px, py, time);
   float ny = wave(py, px, time);
   float nz = 0.5 * nx + 0.5 * ny;
   px += nx;
   py += ny;
   pz += nz;

   return vec3(px, py, pz);
}


void main(void)
{
    uv = gl_MultiTexCoord0.xy;

   float u = uv.x;
   float v = uv.y;
   float thickness = 0.5;

   // 100 vertices on a length of 6 * M_PI
   // => the next vertex is 0.18849555921538759430775860299677 away.
   //    approximately.
   float x = gl_Vertex.x;
   vec3 v1 = f(x);
   vec3 v2 = f(x + 0.18);

   // generate coordinate system at position v1
   vec3 up = vec3(0.0, 0.0, 1.0);
   vec3 vx = normalize(vec3(v2 - v1)); // ribbon direction
   vec3 vy = cross(vx, up);            // 90° to the side
   vec3 vz = cross(vy, vx);            // on top of the other two

   mat3 mat = mat3(vx, vy, vz);

   // add rotation circle around v1
   vec3 rot = vec3(
      0.0,
      cos(6.0 * x + time) * v * thickness,
      sin(4.0 * x + time) * v * thickness
   );

   rot = mat * rot;
   v1 = rot + v1;

   // shift pos to field in game
   v1.xy += fieldPosition.xy;

   gl_Position = gl_ModelViewProjectionMatrix * vec4(v1, 1.0);
}

