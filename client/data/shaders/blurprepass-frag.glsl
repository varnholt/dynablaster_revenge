#version 120

// simple downsampling filter as pre-pass for gauss filter
uniform float texelOffsetX;
uniform float texelOffsetY;
varying vec2 uv;
uniform sampler2D texture;
uniform float clampU;
uniform float clampV;

// normalized filter coefficients
const int filterSize= 4;
const float coeff[filterSize*2+1]= {
   0.005172755104,
   0.02976715006,
   0.1038975641,
   0.2199511528,
   0.2824228704,
   0.2199511528,
   0.1038975641,
   0.02976715006,
   0.005172755104
};

vec4 getSample(vec2 uv)
{
   uv.x= min(uv.x, clampU);
   uv.y= min(uv.y, clampV);

   return texture2D(texture, uv);
}

void main()
{
   // uv-step to next pixel in source image
   vec2 texelOffset= vec2( texelOffsetX, texelOffsetY );

   vec2 pos= uv - texelOffset * filterSize;

   vec4 col= vec4(0.0, 0.0, 0.0, 0.0);
   for (int i=0; i<=filterSize*2; i++)
   {
      col += getSample(pos) * coeff[i];
      pos += texelOffset;
   }

   gl_FragColor= col;
}
