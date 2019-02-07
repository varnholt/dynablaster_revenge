#ifndef GAMEJOYSTICKMAPPING_H
#define GAMEJOYSTICKMAPPING_H

// Qt
#include <QObject>

// joystick
#include "../joystick/joystickinfo.h"

class GameJoystickMapping : public QObject
{
    Q_OBJECT

   public:

      //! constructor
      GameJoystickMapping(QObject *parent = 0);

      void updateKeysPressed();

      //! setter for hat value
      void setHatValue(JoystickConstants::Hat hat);

      //! getter for hat value
      JoystickConstants::Hat getHatValue() const;

      //! setter for axis values
      void setAxisValues(int x, int y);

      //! getter for axis value
      void getAxisValues(int& x, int&y);

      //! setter for keys pressed
      void setKeysPressed(int keys);

      //! getter for keys pressed
      int getKeysPressed() const;

      //! setter for keys pressed previous
      void setKeysPressedPrevious(int keys);

      //! getter for keys pressed previous
      int getKeysPressedPrevious() const;


      void setAxisKeys(int keys);
      int getAxisKeys() const;

      void setHatKeys(int keys);
      int getHatKeys() const;

      void setAxisThreshold(int threshold);
      int getAxisThreshold();


   signals:

      //! keypress "event" going to client
      void keyPressed(int);

      //! keyrelease "event" going to client
      void keyReleased(int);

      //! pass joystick info to others
      void dataReceived(const JoystickInfo&);


      // these are mostly obsolete now

      //! hat was pressed
      void hat(JoystickConstants::Hat hatValue);

      //! button was pressed
      void button(int buttons);

      //! axis was triggered
      void axis(int x, int y);


   public slots:

      //! incoming data
      void data(const JoystickInfo&);


   protected:

      //! process axes
      bool processAxes();

      //! process buttons
      bool processButtons();

      //! process hats
      bool processHats();


      //! joystick info coming from the joystick handler
      JoystickInfo mJoystickInfo;

      //! keys pressed in this call
      int mKeysPressed;

      //! keys pressed in prevous call
      int mKeysPressedPrevious;

      // button ids

      //! key mapping for bomb
      int mButtonBombId;

      //! key mapping for kick
      int mButtonKickId;


      // thresholds

      //! axis threshold
      int mAxisThreshold;


      JoystickConstants::Hat mHatValue;

      int mAxisXValue;
      int mAxisYValue;

      int mAxisKeys;
      int mHatKeys;

};

#endif // GAMEJOYSTICKMAPPING_H

