uniform vec3 field;
uniform vec3 color;
uniform float heightmap[49];
uniform float time;
uniform vec3 camera;

varying float alpha;
varying float index;
varying vec2 uv;


float getHeight(float fx, float fy)
{
   float height = 0.0;

   int absoluteX = int(floor(fx));
   int absoluteY = int(floor(fy));

   int centerX = int(floor(field.x));
   int centerY = int(floor(field.y));

   int x = absoluteX - centerX;
   int y = absoluteY - centerY;

   x += 3;
   y += 3;

   if (
         x > -1 && x < 7
      && y > -1 && y < 7
   )
   {
      height = heightmap[(y * 7) + x];
   }

   return height;
}


mat3 rotateX(const float f)
{
   float c = cos(f);
   float s = sin(f);

   mat3 m = mat3(
      1.0, 0.0, 0.0,
      0.0,   c,   s,
      0.0,  -s,   c
   );

   return m;
}


mat3 rotateY(const float f)
{
   float c = cos(f);
   float s = sin(f);

   mat3 m = mat3(
        c, 0.0,  -s,
      0.0, 1.0, 0.0,
        s, 0.0,   c
   );

   return m;
}


mat3 rotateZ(const float f)
{
   float c = cos(f);
   float s = sin(f);

   mat3 m = mat3(
        c,   s, 0.0,
       -s,   c, 0.0,
      0.0, 0.0, 1.0
   );

   return m;
}


void main(void)
{
   uv = gl_MultiTexCoord0.xy;

   vec3 ray = gl_MultiTexCoord1.xyz;
   float speed = gl_MultiTexCoord2.x;
   index = gl_MultiTexCoord2.y;

   mat3 rotX = rotateX(time + index);
   mat3 rotY = rotateY(time + index);
   mat3 rotZ = rotateZ(time + index);
   mat3 rot = rotX * rotY * rotZ;

   vec3 center;
   vec3 pos = rot * gl_Vertex.xyz;
   vec3 down = vec3(0.0, 0.0, -0.5 * time * time);

   // compute alpha (normal against camera direction; in world space)
   vec3 n1 = rot * gl_Normal;
   alpha = dot(n1, camera);
   alpha = abs(alpha);

   /*
   vec3 oldCenter;
   vec3 oldPos = pos;
   vec3 oldDown = vec3(0.0, 0.0, -0.5 * (time-0.01) * (time-0.01));
   oldCenter = ( (time-0.01) * speed) * ray.xyz;
   oldCenter += field;
   oldPos += oldCenter;
   */

   // move particle
   center = (time * speed) * ray.xyz;
   center += field;
   pos += center;

   float dz = down.z;

   /*
   // check box collision when particle is going down
   if ( (pos.z + down.z) < (oldPos.z + oldDown.z) )
   {
      float height = getHeight(center.x, center.y);

      if (pos.z + down.z < height)
      {
         dz = height - pos.z;
         // debug = height;
      }
   }
   */

   pos.z += dz;

   gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 1.0);
}

