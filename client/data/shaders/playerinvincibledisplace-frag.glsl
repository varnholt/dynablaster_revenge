#version 120

// gauss filter with variable radius

uniform sampler2D texture;
uniform sampler2D displace;
uniform vec2 texelOffset;
uniform vec4 displaceOffset;
uniform vec4 sourceRect;
uniform float fade;
uniform vec3 center;

//varying vec2 uv;
varying vec4 pos;

void main()
{
   vec4 result;

   vec2 uv= pos.xy / pos.w;

   // sample displacement map
   vec2 d1= texture2D(displace, (uv-center.xy)*0.70 + texelOffset*displaceOffset.xy).rg - 0.5;
   vec2 d2= texture2D(displace, (uv-center.xy)*0.65 + texelOffset*displaceOffset.yz).rg - 0.5;
   vec2 d3= texture2D(displace, (uv-center.xy)*0.60 + texelOffset*displaceOffset.zw).rg - 0.5;

   d1*=texelOffset * 0.04;
   d2*=texelOffset * 0.04;
   d3*=texelOffset * 0.04;

   // center element
   vec4 c1= texture2D(texture, (uv-sourceRect.xy)*sourceRect.zw + d1);
   vec4 c2= texture2D(texture, (uv-sourceRect.xy)*sourceRect.zw + d2);
   vec4 c3= texture2D(texture, (uv-sourceRect.xy)*sourceRect.zw + d3);

   result= (c1+c2+c3) * 0.4 * fade;

//   result= vec4(d1 * 5.0 + 0.5, 0.0, 1.0);

   gl_FragColor= result;
}
