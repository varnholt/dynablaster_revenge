#include "stdafx.h"
#include "profiling.h"

double getCpuTick()
{
   double t=0;

#ifdef _MSC_VER
//   #ifdef WIN32
//      _asm {
//         lea edi,t
//         rdtsc
//         mov [edi],eax
//         mov [edi+4],edx
//         fild qword ptr [edi]
//         fstp qword ptr [edi]
//      };
//   #endif
#endif

#ifdef _LINUX_
   unsigned int a, d;
   asm volatile
   (
       "rdtsc"        // read timestamp counter (64bit) into edx:eax (2x 32bit registers)
       : "=a" (a)     // store eax to "a"
       , "=d" (d)     // store edx to "d"
   );
   // combine two 32bit integers into 64bit long
   long long count= ((long long)a) | (((long long)d) << 32);
   t= count;
#endif

   return t;
}

