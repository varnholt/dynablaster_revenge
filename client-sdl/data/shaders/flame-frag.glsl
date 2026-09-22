#version 300 es
precision mediump float;

// GLES3 port of client/data/shaders/flame-frag.glsl. sampler1D/texture1D -> sampler2D/texture
// (the gradient map is uploaded as a 2D texture with height 1 - GLES3 has no 1D textures, see
// DetonationManager::init()). sampler3D/texture3D -> texture (GLES3 has native 3D texture
// support, no substitution needed there). The noise volume is uploaded as single-channel R8 (see
// DetonationManager::init()) instead of the legacy ALPHA format (not part of GLES3's texImage3D
// format table) - texture(...).a becomes texture(...).r. gl_ProjectionMatrix (fixed-function,
// used here for a manual depth write, not just the standard vertex transform) becomes an explicit
// u_projection uniform - same value as the vertex stage's, GLSL uniforms are shared across a
// linked program. gl_FragDepth/gl_DepthRange are real GLSL ES 3.00 built-ins, ported as-is.

in vec3 rayPosWorld;
in vec3 raypos;
in vec3 raydir;

uniform float time;
uniform float top;
uniform float bottom;
uniform float left;
uniform float right;
uniform vec3  boundmin;
uniform vec3  boundmax;
// explicit highp - must match the vertex stage's implicit default precision for a shared uniform
// name, or linking fails ("precision mismatch between shaders for uniform").
uniform highp mat4 u_projection;
uniform sampler3D noisemap;
uniform sampler2D gradientmap;

out vec4 o_color;

mat3 m = mat3( 0.00,  0.80,  0.60,
              -0.80,  0.36, -0.48,
              -0.60, -0.48,  0.64 );

float noise( in vec3 x )
{
   const float scale = 1.0;
   const vec3 texSizeInv = vec3( scale / 32.0, scale / 32.0, scale / 32.0 );
   float res = texture(noisemap, x * texSizeInv).r * 2.0 - 1.0;

   return res;
}

float fbm( vec3 p )
{
   float f;
                 f  = 0.5000 * noise( p );
   p = m*p*2.02; f += 0.2500 * noise( p );
   p = m*p*2.03; f += 0.1250 * noise( p );
   p = m*p*2.01; f += 0.0625 * noise( p );
   return f;
}

vec4 gradient(float x)
{
   vec4 c;
   c = texture(gradientmap, vec2(x, 0.5));
   return c;
}

vec4 map( in vec3 p )
{
   float d1 = 1.0 - length( p.xz ) * 5.0;
   float d2 = 1.0 - length( p.yz ) * 5.0;

   if (p.y < -bottom)
   {
      d1 -= (-p.y-bottom)*4.0;
   }
   else if (p.y > top)
   {
      d1 -= (p.y-top)*4.0;
   }

   if (p.x > right)
   {
      d2 -= (p.x-right)*4.0;
   }
   else if (p.x < -left)
   {
      d2 -= (-p.x-left)*4.0;
   }

   float d = max(d1,d2);

   float fadepos = dot(p,p);
   float dist = max( 5.0 - fadepos*1.5 - time*time*1.0, 0.0 );
   d -= (0.1+time*1.5 + dist)*time*time;
   d += (3.0 + time*time) * fbm( p*3.5 + vec3(-1.2,-0.9, 1.0)*time*2.0 ) - 0.2;

   vec4 res = gradient(d);

   return res;
}

vec4 trace( in vec3 pos, in vec3 dir )
{
   vec4 sum = vec4(0.0, 0.0, 0.0, 0.0);

   float range = 1.0;

   float t = 0.0;
   while (t<range)
   {
      vec3 curpos = pos + dir*t;

      vec4 col = map( curpos );

      sum += col*(1.0 - sum.a);

      if (sum.a > 0.95)
      {
         break;
      }

      t += 0.1/5.0;
   }

   vec3 worldPos = rayPosWorld + dir*t;
   float clipz = u_projection[0].z * worldPos.x
               + u_projection[1].z * worldPos.y
               + u_projection[2].z * worldPos.z
               + u_projection[3].z;

   float clipw = u_projection[0].w * worldPos.x
               + u_projection[1].w * worldPos.y
               + u_projection[2].w * worldPos.z
               + u_projection[3].w;

   float depth = clipz / clipw;
   gl_FragDepth = ((gl_DepthRange.diff * depth) + gl_DepthRange.near + gl_DepthRange.far) / 2.0;

   return sum;
}

void main(void)
{
   o_color = trace( raypos, normalize( raydir ) );
}
