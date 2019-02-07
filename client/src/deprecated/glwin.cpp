#include "stdafx.h"
#include "renderdevice.h"
#include <windows.h>
#include <GL/gl.h>

char windowTitle[]="OpenGL Device";

HWND       hWnd;                   // Storage For Window Handle
HGLRC      hRC;                    // Permanent Rendering Context
HDC        hDC;                    // Private GDI Device Context

void* getProcAddress(char* name)
{
   void *ptr= NULL;

   ptr= (void*)wglGetProcAddress(name);

   if (!ptr) printf("%s does not exist!\n", name);

   return ptr;
}

// window callback
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)  
   {
   case WM_DESTROY:
   case WM_CLOSE:
      ExitProcess(0);
      break;
   case WM_KEYDOWN:
      if (wParam==VK_ESCAPE) activeDevice->setAbort();
      break;
   case WM_SIZE:
      activeDevice->resize(LOWORD(lParam),HIWORD(lParam));
      break;
   }

   return DefWindowProc(hWnd, message, wParam, lParam);
}



// create window and init opengl
bool initGL(int x, int y)
{
   GLuint PixelFormat;

   HINSTANCE hInstance = GetModuleHandle(NULL);

   static WNDCLASS wc=
   {
      CS_OWNDC,
         (WNDPROC) WinProc,
         0,0,
         hInstance,
         NULL,
         LoadCursor(NULL, IDC_ARROW),
         NULL,
         NULL,
         windowTitle
   };

   if(!RegisterClass(&wc)) return FALSE;

   hWnd = CreateWindowEx(0, windowTitle, windowTitle, WS_OVERLAPPEDWINDOW, 0,0, x,y, NULL, NULL, hInstance, NULL);
   if (hWnd==NULL) return FALSE;

   // show window
   ShowWindow(hWnd, SW_SHOW);
   UpdateWindow(hWnd);
   SetFocus(hWnd);

   static  PIXELFORMATDESCRIPTOR pfd=
   {
      sizeof(PIXELFORMATDESCRIPTOR),          // Size Of This Pixel Format Descriptor
         1,                                      // Version Number (?)
         PFD_DRAW_TO_WINDOW |                    // Format Must Support Window
         PFD_SUPPORT_OPENGL |                    // Format Must Support OpenGL
         PFD_DOUBLEBUFFER,                       // Must Support Double Buffering
         PFD_TYPE_RGBA,                          // Request An RGBA Format
         32,                                     // Select A 16Bit Color Depth
         0, 0, 0, 0, 0, 0,                       // Color Bits Ignored (?)
         0,                                      // No Alpha Buffer
         0,                                      // Shift Bit Ignored (?)
         0,                                      // No Accumulation Buffer
         0, 0, 0, 0,                             // Accumulation Bits Ignored (?)
         16,                                     // 16Bit Z-Buffer (Depth Buffer)  
         0,                                      // No Stencil Buffer
         0,                                      // No Auxiliary Buffer (?)
         PFD_MAIN_PLANE,                         // Main Drawing Layer
         0,                                      // Reserved (?)
         0, 0, 0                                 // Layer Masks Ignored (?)
   };

   hDC = GetDC(hWnd);                              // Gets A Device Context For The Window
   if (hDC==NULL) return 0;

   PixelFormat = ChoosePixelFormat(hDC, &pfd);     // Finds The Closest Match To The Pixel Format We Set Above
   if (!PixelFormat) return 0;

   if(!SetPixelFormat(hDC,PixelFormat,&pfd)) return 0;

   hRC = wglCreateContext(hDC);
   if (!hRC) return 0;

   if (!wglMakeCurrent(hDC, hRC)) return 0;

   int dx= GetSystemMetrics(SM_CXSIZEFRAME);
   int dy= GetSystemMetrics(SM_CYSIZEFRAME);
   int dm= GetSystemMetrics(SM_CYMENU);
   activeDevice->resize(x-dx*2, y-dm-dy*2-3);

   return TRUE;
}


// close window and kill opengl
void closeGL()
{
   wglMakeCurrent(NULL,NULL);
   wglDeleteContext(hRC);
   ReleaseDC(hWnd,hDC);
   ChangeDisplaySettings(NULL, 0);
   PostQuitMessage(0);
}


void swapBuffers()
{
   SwapBuffers(hDC);
}

void checkMessages()
{
   MSG msg;
   while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
   {
      TranslateMessage( &msg );
      DispatchMessage( &msg );
   }
}
