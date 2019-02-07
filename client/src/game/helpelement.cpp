#include "helpelement.h"
#include "framework/timerhandler.h"

// static variables
float HelpElement::sDurationFadeIn = 1000;
float HelpElement::sDurationDelay = 6000;
float HelpElement::sDurationFadeOut = 1000;
float HelpElement::sDurationPopup = 2000;


HelpElement::HelpElement(
   const QString &page,
   const QString &message,
   Constants::HelpSeverity severity,
   Constants::HelpLocation location,
   int delay
)
 : QObject(),
   mPage(page),
   mMessage(message),
   mSeverity(severity),
   mLocation(location),
   mStarted(false),
   mFinished(false),
   mDelay(delay),
   mDelayElapsed(false),
   mDelayTimerActive(false)
{
   connect(
      &mTimer,
      SIGNAL(timeout()),
      this,
      SLOT(timeout())
   );
}


void HelpElement::start()
{
   mStartTime.start();
   mTimer.start(getDisplayDuration());
   setStarted(true);
}


void HelpElement::startDelayTime()
{
   if (!mDelayTimerActive)
   {
      TimerHandler::singleShot(
         mDelay,
         this,
         SLOT(delayTimeout())
      );

      mDelayTimerActive = true;
   }
}


void HelpElement::delayTimeout()
{
   mDelayTimerActive = false;
   mDelayElapsed = true;
}


bool HelpElement::isDelayElapsed()
{
   return isDelayed()
      ? mDelayElapsed
      : true;
}


bool HelpElement::isDelayed()
{
   return (mDelay > 0);
}


bool HelpElement::isDelayStarted()
{
   return mDelayTimerActive;
}


void HelpElement::setFinished(bool finished)
{
   mFinished = finished;
}


bool HelpElement::isFinished() const
{
   return mFinished;
}


void HelpElement::setStarted(bool started)
{
   mStarted = started;
}


bool HelpElement::isStarted() const
{
   return mStarted;
}


QString HelpElement::getPage() const
{
   return mPage;
}


QString HelpElement::getMessage() const
{
   return mMessage;
}


Constants::HelpSeverity HelpElement::getSeverity() const
{
   return mSeverity;
}


Constants::HelpLocation HelpElement::getLocation() const
{
   return mLocation;
}


int HelpElement::getDisplayDuration() const
{
   return
      qMax(
           sDurationFadeIn
         + sDurationDelay
         + sDurationFadeOut,
         sDurationPopup
      );
}


int HelpElement::getPopupDuration() const
{
   return sDurationPopup;
}


int HelpElement::getFadeInDuration() const
{
   return sDurationFadeIn;
}


int HelpElement::getFadeOutDuration() const
{
   return sDurationFadeOut;
}


int HelpElement::getIdleDuratiotion() const
{
   return sDurationDelay;
}


void HelpElement::timeout()
{
   setFinished(true);
}





