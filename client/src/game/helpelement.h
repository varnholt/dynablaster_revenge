#ifndef HELPELEMENT_H
#define HELPELEMENT_H


#include <QObject>

#include "constants.h"
#include "framework/frametimer.h"

class HelpElement : public QObject
{

Q_OBJECT

public:

   HelpElement(
      const QString& getPage,
      const QString& message,
      Constants::HelpSeverity severity,
      Constants::HelpLocation location,
      int delay = 0
   );

   void start();
   void startDelayTime();
   bool isDelayElapsed();
   bool isDelayed();
   bool isDelayStarted();

   void setFinished(bool);
   bool isFinished() const;

   void setStarted(bool);
   bool isStarted() const;

   QString getPage() const;
   QString getMessage() const;

   Constants::HelpSeverity getSeverity() const;
   Constants::HelpLocation getLocation() const;

   int getDisplayDuration() const;
   int getPopupDuration() const;
   int getFadeInDuration() const;
   int getFadeOutDuration() const;
   int getIdleDuratiotion() const;


protected slots:


   void timeout();
   void delayTimeout();


protected:

   QString mPage;
   QString mMessage;
   Constants::HelpSeverity mSeverity;
   Constants::HelpLocation mLocation;

   FrameTimer mStartTime;

    /*
      3 phases:
      - animate in
      - delay
      - animate out
    */

    FrameTimer mStartTimeFadeIn;
    FrameTimer mStartTimeDelay;
    FrameTimer mStartTimeFadeOut;

    bool mStarted;
    bool mFinished;

    FrameTimer mTimer;

    static float sDurationFadeIn;
    static float sDurationDelay;
    static float sDurationFadeOut;
    static float sDurationPopup;

    int mDelay;
    bool mDelayElapsed;
    bool mDelayTimerActive;
};

#endif // HELPELEMENT_H

