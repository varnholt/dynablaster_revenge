varying vec3 rayPosWorld;
varying vec3 raypos;
varying vec3 raydir;
uniform float time;
uniform float top;
uniform float bottom;
uniform float left;
uniform float right;
uniform vec3  boundmin;
uniform vec3  boundmax;
uniform sampler3D noisemap;
uniform sampler1D gradientmap;

mat3 m = mat3( 0.00,  0.80,  0.60,
              -0.80,  0.36, -0.48,
              -0.60, -0.48,  0.64 );

float noise( in vec3 x )
{
   const float scale= 1.0;
   const vec3 texSizeInv= vec3( scale / 32.0, scale / 32.0, scale / 32.0 );
   float res= texture3D(noisemap, x * texSizeInv).a * 2.0 - 1.0;

    return res;
}

float fbm( vec3 p )
{
    float f;
                  f  = 0.5000*noise( p ); 
    p = m*p*2.02; f += 0.2500*noise( p ); 
    p = m*p*2.03; f += 0.1250*noise( p ); 
    p = m*p*2.01; f += 0.0625*noise( p );
    return f;
}

vec4 gradient(float x)
{
   vec4 c;
   c= texture1D(gradientmap, x);
   return c;
}

vec4 map( in vec3 p )
{
	float d1= 1.0 - length( p.xz ) * 5.0;
//   if (p.z > 2.0) d -= p.z-2.0;// * 0.5;
   float d2= 1.0 - length( p.yz ) * 5.0;

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

   float d= max(d1,d2);

   float fadepos= dot(p,p);//max(abs(p.x), abs(p.y));
   float dist= max( 5.0 - fadepos*1.5 - time*time*1.0, 0.0 );
   d -= (0.1+time*1.5 + dist)*time*time;// - (0.0 - pow(dot(p,p), time+0.1));
	d += (3.0 + time*time) * fbm( p*3.5 + vec3(-1.2,-0.9, 1.0)*time*2.0 ) - 0.2;

/*
   float fadepos= max(abs(p.x), abs(p.y));
   float dist= abs( 5.0 - time*1.0 );
   d -= (0.1+time*1.5 + dist*2.0)*time*time*time;// - (0.0 - pow(dot(p,p), time+0.1));
	d += 3.0 * fbm( p*3.5 + vec3(-1.2,-0.9, 1.0)*time*2.0 ) - 0.2;
*/

//   vec4 res = vec4( d );
//   res.xyz = mix( 1.15*vec3(1.0,0.95,0.8), vec3(0.7,0.7,0.7), res.x );

	vec4 res = gradient(d);

	return res;
}

vec4 trace( in vec3 pos, in vec3 dir )
{
	vec4 sum = vec4(0.0, 0.0, 0.0, 0.0);

   // clip ray to bounding box
/*
   float tx,ty,tz;
   if (rd.x < 0.0)
      tx= (boundmin.x - pos.x) / rd.x;
   else
      tx= (boundmax.x - pos.x) / rd.x;

   if (rd.y < 0.0)
      ty= (boundmin.y - pos.y) / rd.y;
   else
      ty= (boundmax.y - pos.y) / rd.y;

   // camera always looks from above, so z ends always at -0.5 in bounding box space
   tz= (-0.5-ro.z) / rd.z;

   if (ty < tx) tx= ty;
   if (tz < tx) tx= tz;
//   if (tx > 1.0) tx= 1.0;
// ray passes box from [0..tx]
*/

   float range= 1.0;

   float t= 0.0;
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

   vec3 worldPos= rayPosWorld + dir*t;
   float clipz= gl_ProjectionMatrix[0].z * worldPos.x
              + gl_ProjectionMatrix[1].z * worldPos.y
              + gl_ProjectionMatrix[2].z * worldPos.z
              + gl_ProjectionMatrix[3].z;

   float clipw= gl_ProjectionMatrix[0].w * worldPos.x
              + gl_ProjectionMatrix[1].w * worldPos.y
              + gl_ProjectionMatrix[2].w * worldPos.z
              + gl_ProjectionMatrix[3].w;

//   float depth= (((far-near) * (clipPos.z / clipPos.w)) + near + far) / 2.0;
   float depth= clipz / clipw;
//   gl_FragDepth= depth;
   gl_FragDepth = ((gl_DepthRange.diff * depth) + gl_DepthRange.near + gl_DepthRange.far) / 2.0;
//   sum.xyz /= (0.01+sum.w*1.0);

	return sum;//clamp( sum, 0.0, 1.0 );
}

void main(void)
{
   gl_FragColor = trace( raypos, normalize( raydir ) );
}
