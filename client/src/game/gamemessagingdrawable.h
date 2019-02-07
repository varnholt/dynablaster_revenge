#ifndef GAMEMESSAGINGDRAWABLE_H
#define GAMEMESSAGINGDRAWABLE_H

// Qt
#include <QColor>
#include <QImage>
#include <QObject>

// base
#include "drawable.h"

// game
#include "animatedgamemessage.h"

// engine
#include "image/psd.h"
#include "framework/frametimer.h"

// forward declarations
class BitmapFont;
class PSDLayer;

class GameMessagingDrawable : public QObject, public Drawable
{
   Q_OBJECT

   public:

      //! constructor
      GameMessagingDrawable(RenderDevice*);

      //! destructor
      virtual ~GameMessagingDrawable();

      //! setter for the message currently written by the user
      void updateMessageVertices();

      //! overwrite intialize gl
      void initializeGL();

      //! overwrite paint gl
      void paintGL();

      //! a key was pressed
      virtual void keyPressEvent(QKeyEvent* event);

      //! overwrite setvisible
      virtual void setVisible(bool visible);

      //! getter for message
      const QString& getMessage() const;

      //! setter for message
      void setMessage(const QString &value);


   public slots:

      //! add a message to the message stack
      void messageReceived(
         int senderId,
         const QString& message,
         bool typingFinished
      );


   protected slots:

      //! remove a message from stack
      void popMessage();

      //! disable ingame-messaging
      void disableIngameMessaging();

      //! game state was changed
      void gameStateChanged();


   protected:

      //! initialize gl parameters
      void initGlParameters();

      //! cleanup gl parameters
      void cleanupGlParameters();

      //! setter for writing active flag
      void setActive(bool active);

      //! getter for writing active flag
      bool isActive() const;

      //! toggle active
      void toggleActive();

      //! build nick vertices
      void buildNickVertices();

      //! draw text
      void drawText(bool drawUserInput);

      //! clear typed in stuff
      void clearMessage();


      // cursor

      //! getter for cursor at end state
      bool isCursorAtEnd() const;

      //! setter for cursor position
      void setCursorPosition(int);

      //! getter for cursor position
      int getCursorPosition() const;

      //! cursor right
      void moveCursorRight();

      //! cursor left
      void moveCursorLeft();

      //! cursor to start
      void moveCursorToStart();

      //! cursor to end
      void moveCursorToEnd();

      //! draw the cursor
      void drawCursor();

      //! restart cursor time
      void restartCursorTime();


      // overlay drawing

      //! initialize layers
      void initializeLayers();

      //! draw message overlay
      bool drawMessageOverlay();

      //! restart activation time
      void restartActivationTime();

      //! getter for activation time
      const FrameTimer& getActivationTime() const;


      // overlay members

      //! psd instance
      PSD mPsd;

      //! filename to load from
      QString mFilename;

      //! list of psd layers
      QList<PSDLayer*> mPsdLayers;

      //! layer for lineedit say
      PSDLayer* mLineEditSayLayer;

      //! player name layer
      PSDLayer* mPlayerNameLayer;


      // font

      //! list of messages displayed
      QList<AnimatedGameMessage*> mMessages;

      //! bitmap font to use
      BitmapFont* mFont;


      // user input

      //! text that is currently entered by the user
      QString mMessage;

      //! current message's vertices
      Array<Vertex> mMessageVertices;

      //! nick vertices
      Array<Vertex> mNickVertices;

      //! writing activated
      bool mActive;

      //! activation time
      FrameTimer mActivationTime;


      // cursor

      //! cursor position
      int mCursorPosition;

      //! cursor color
      QColor mColor;

};

#endif // GAMEMESSAGINGDRAWABLE_H
