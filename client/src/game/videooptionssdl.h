#ifndef VIDEOOPTIONSSDL_H
#define VIDEOOPTIONSSDL_H

#include "videooptions.h"

class VideoOptionsSDL : public VideoOptions
{
   public:

      VideoOptionsSDL();

      virtual void initialize();

   protected:

      virtual void initializeModes();
};

#endif // VIDEOOPTIONSSDL_H
