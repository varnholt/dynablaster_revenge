#include "renderdevice.h"
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <stdio.h>

Display    *mDisplay;
Window     mWin;
Atom       mAtom;
GLXContext ctx;

void* getProcAddress(char* name)
{
   void *ptr= NULL;

   ptr= (void*)glXGetProcAddressARB((GLubyte*)name);

   if (!ptr) printf("%s does not exist!\n", name);

   return ptr;
}


bool initGL(int x, int y)
{
   int vi_attr[] =
   {
      GLX_RGBA,
         GLX_DOUBLEBUFFER,
         GLX_RED_SIZE,       4,
         GLX_GREEN_SIZE,     4,
         GLX_BLUE_SIZE,      4,
         GLX_DEPTH_SIZE,    16,
         None
   };

   XVisualInfo *vi;
   Window root_win;
   XWindowAttributes win_attr;
   XSetWindowAttributes set_attr;

   if( ( mDisplay = XOpenDisplay( NULL ) ) == NULL ) return false;

   if( ( vi = glXChooseVisual( mDisplay, DefaultScreen( mDisplay ), vi_attr ) ) == NULL )
   {
      XCloseDisplay( mDisplay );
      return false;
   }

   root_win = RootWindow( mDisplay, vi->screen );

   XGetWindowAttributes( mDisplay, root_win, &win_attr );

//   mWidth= x;
//   mHeight= y;

   set_attr.border_pixel = 0;

   set_attr.colormap =
      XCreateColormap( mDisplay, root_win, vi->visual, AllocNone );

   set_attr.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask |
      ButtonReleaseMask | PointerMotionMask | StructureNotifyMask;

   set_attr.override_redirect = false;

   mWin =
      XCreateWindow(
      mDisplay, root_win, 0, 0, x, y, 0, vi->depth,
      InputOutput, vi->visual, CWBorderPixel | CWColormap |
      CWEventMask | CWOverrideRedirect, &set_attr );

   XStoreName( mDisplay, mWin, "GL View" );
   XMapWindow( mDisplay, mWin );

   mAtom = XInternAtom( mDisplay, "WM_DELETE_WINDOW", True );
   XSetWMProtocols( mDisplay, mWin, &mAtom, 1 );

   if( ( ctx = glXCreateContext( mDisplay, vi, NULL, True ) ) == NULL )
   {
      XDestroyWindow( mDisplay, mWin );
      XCloseDisplay( mDisplay );
      return false;
   }

   if( glXMakeCurrent( mDisplay, mWin, ctx ) == False )
   {
      glXDestroyContext( mDisplay, ctx );
      XDestroyWindow( mDisplay, mWin );
      XCloseDisplay( mDisplay );
      return false;
   }

   return true;
}


void closeGL()
{
   glXMakeCurrent( mDisplay, None, NULL );
   glXDestroyContext( mDisplay, ctx );
   XDestroyWindow( mDisplay, mWin );
   XCloseDisplay( mDisplay );
}


void swapBuffers()
{
   glXSwapBuffers( mDisplay, mWin );
}

void checkMessages()
{
   int key;
   XEvent event;

   //      XPeekEvent( mDisplay, &event );
   while( XPending( mDisplay ) )
   {
      XNextEvent( mDisplay, &event );

      switch( event.type )
      {
      case KeyPress:
         break;

      case KeyRelease:
         key = XLookupKeysym( &event.xkey, 0 );
         switch( key )
         {
         case XK_Tab:
         case XK_Escape:
            activeDevice->setAbort();
            break;
         }
         break;

         //         case MouseMove

      case UnmapNotify:
//         mActive = 0;
         break;

      case   MapNotify:
         //mActive = 1;
         break;

      case ConfigureNotify:
         activeDevice->resize(event.xconfigure.width, event.xconfigure.height);
         break;

      case ClientMessage:
         if( event.xclient.data.l[0] == (int) mAtom ) activeDevice->setAbort();
         //            if( event.xclient.data.l[0] == (int) wmDelete ) mAbort=1;
         break;

      case ReparentNotify:
         break;

      default:
         //            mAbort=1;
         //            printf( "caught unknown event, type %d\n", event.type );
         break;
      }
   }
}

