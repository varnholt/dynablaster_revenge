#pragma once

#include <QObject>

class FrameTimer : public QObject
{
   Q_OBJECT

public:
   FrameTimer();
   FrameTimer(QObject* parent);
   FrameTimer(const FrameTimer& other);
   ~FrameTimer();

   FrameTimer& operator = (const FrameTimer& other);

   static FrameTimer currentTime();

   bool isValid() const;

   FrameTimer addMSecs(float ms) const;
   float msecsTo(const FrameTimer& other) const;

   void start();
   void start(float interval);
   void restart();
   void stop();

   float elapsed() const;

   bool update();

   float interval() const;
   void setInterval(float ms);

   void setSingleShot(bool singleShot);

signals:
   void timeout();

public:
   bool  mStarted;
   bool  mSingleShot;
   float mStartTime;
   float mInterval;
   bool  mDelete;
};
