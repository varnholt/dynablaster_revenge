//
// C++ Interface: constants
//
// Description: 
//
//
// Author: Matthias Varnholt <matto@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CONSTANTS_H
#define CONSTANTS_H

// server
#define SERVER_CONFIG_FILE_SERVER    "data/server.ini"
#define SERVER_SPEED                 0.05f
#define SERVER_HEARTBEAT_IN_HZ       50
#define SERVER_PORT                  6300
#define SERVER_MOVE_EPSILON          0.3f
#define SERVER_PREPARATION_SYNC_TIME 2
#define SERVER_PREPARATION_TIME      4 // 3, 2, 1, go
#define SERVER_FINISHING_TIME        1500
#define SERVER_BOMB_TICKTIME_DEFAULT 3000
#define SERVER_BOMB_NEIGHBOUR_DELAY  100
#define SERVER_IMMUNE_FIELD_DELAY    1000
#define SERVER_PLAYER_SYNC_MAX_TIME  20000
#define SERVER_SHAKE_CHECK_INTERVAL  250
#define SERVER_DEFAULT_BOMBCOUNT     1
#define SERVER_DEFAULT_FLAMECOUNT    2
#define SERVER_DEFAULT_SPEEDUPS      0
#define SERVER_DEFAULT_SPEED         1.0f
#define SERVER_SPEEDUP_INCREMENT     0.15f
#define SERVER_MAX_SPEEDUPS          5
#define SERVER_SKULL_DURATION        20000
#define SERVER_SKULL_BOMBCOUNT_MIN   1
#define SERVER_SKULL_BOMBCOUNT_MAX   10
#define SERVER_SKULL_FLAMECOUNT_MIN  1
#define SERVER_SKULL_FLAMECOUNT_MAX  10
#define SERVER_SKULL_SPEED_MIN       SERVER_DEFAULT_SPEED * 0.5f
#define SERVER_SKULL_SPEED_MAX       SERVER_DEFAULT_SPEED + (SERVER_SPEEDUP_INCREMENT * 10.0f)
#define SERVER_WEIGHT_BOMBS          30.0f
#define SERVER_WEIGHT_FLAMES         30.0f
#define SERVER_WEIGHT_KICKS          10.0f
#define SERVER_WEIGHT_SKULLS         15.0f
#define SERVER_WEIGHT_SPEEDUPS       15.0f
#define SERVER_MAX_REMAINING_TIME    10
#define SERVER_SPECTATOR_DELAY       4000
#define SERVER_KICK_PLAYER_DISTANCE  0.85f
#define SERVER_SPAWN_INTERVAL        15000

// client
#define CLIENT_MESSAGE_TEXT_MAXIMUM 80
#define SHOW_WINNER_FADE_IN_TIME    500
#define SHOW_WINNER_DISPLAY_TIME    4000
#define SHOW_WINNER_FADE_OUT_TIME   500
#define SHOW_WINNER_ADDITIONAL_TIME 2000
#define SHOW_WINNER_SHOW_MENU_TIME  1000
#define SHOW_WINNER_TIME_SUM          SHOW_WINNER_FADE_IN_TIME    \
                                    + SHOW_WINNER_DISPLAY_TIME    \
                                    + SHOW_WINNER_FADE_OUT_TIME   \
                                    + SHOW_WINNER_ADDITIONAL_TIME \
                                    + SHOW_WINNER_SHOW_MENU_TIME
#define ITEM_INTERPOLATION_EPS      0.3f


/**
	@author Matthias Varnholt <matto@gmx.de>
*/
class Constants
{
   public:

      //! key enum
      enum Key
      {
         KeyUp    = 0x01,
         KeyDown  = 0x02,
         KeyLeft  = 0x04,
         KeyRight = 0x08,
         KeyBomb  = 0x10
      };

      //! directions
      enum Direction
      {
         DirectionUnknown = -1,
         DirectionUp,
         DirectionDown,
         DirectionLeft,
         DirectionRight
      };

      //! player color
      enum Color
      {
         ColorWhite  = 1,
         ColorBlack  = 2,
         ColorRed    = 3,
         ColorGreen  = 4,
         ColorBlue   = 5,
         ColorGrey   = 6,
         ColorYellow = 7,
         ColorPurple = 8,
         ColorCyan   = 9,
         ColorOrange = 10
      };

      //! game state
      enum GameState
      {
         GameStopped   = 1,
         GamePreparing = 2,
         GameActive    = 3,
         GameFinishing = 4
      };

      //! the extra's type
      enum ExtraType
      {
         ExtraBomb    =  0x1,
         ExtraFlame   =  0x2,
         ExtraSpeedup =  0x4,
         ExtraKick    =  0x8,
         ExtraSkull   =  0x10
      };

      //! supported skull types
      enum SkullType
      {
         SkullAutofire       = 0,
         SkullMinimumBomb    = 1,
         SkullKeyboardInvert = 2,
         SkullMushroom       = 3,
         SkullInvisible      = 4,
         SkullInvincible     = 5,
         SkullMaximumBomb    = 6,
         SkullSlow           = 7,
         SkullFast           = 8,
         SkullNoBomb         = 9,
         SkullReset          = 10
      };

      //! playfield dimensions
      enum Dimension
      {
         DimensionInvalid = 0,
         Dimension13x11   = 143,
         Dimension19x17   = 323,
         Dimension25x21   = 525
      };

      //! help severity
      enum HelpSeverity
      {
         HelpSeverityNotification,
         HelpSeverityError
      };

      //! help location
      enum HelpLocation
      {
         HelpLocationTopLeft,
         HelpLocationTopRight,
         HelpLocationBottomLeft,
         HelpLocationButtomRight
      };

      //! error type
      enum ErrorType
      {
         ErrorDefault,
         ErrorSyncTimeout
      };

      //! game mode
      enum GameMode
      {
         GameModeSinglePlayer,
         GameModeMultiPlayer
      };
};

#endif
