# About

Dynablaster Revenge is a remake of the game Dynablaster, released by Hudson Soft in 1991. The goal of this remake is to keep the original game-play as untouched as possible while adding some new features such as networked multiplayer and real-time 3D rendering. In case you're not yet familiar with the original game goal, it's quite simple: Bomb all other players from the screen. Either by collecting flame extras to increase the bomb radius or by picking up bomb extras in order to have more bombs to drop you're able to surround your enemies with your bombs, or blow them away with clever bomb chain reactions.

# Credits

## Core team
- mueslee (Matthias Varnholt), code & team lead
- hellfire (Christoph Grote), code
- deathstar, artwork

## Level design
- deathstar, Mansion concept & design
- Christopher Aldridge, space concept & design
- Sebastian Meckelmann, castle concept & design

## 3D
- xabotage (Daniel Phelps), character animation
- hellfire, main caracter design, UVs, game integration
- mueslee, space level lowpoly edit, UVs, game integration

## Music
- daxx, music & SFX
- svenzzon, music & SFX
- cold storage, music
- jco, music
- keito, music
- neoj1n (c.c.catch), music
- netpoet, music
- romeo knight, music
- skaven, music
- sunspire, music

## We thank      
- alk, testing & feedback
- jan, testing & feedback
- fuxx, development infrastructure
- neoman Development infrastructure

# How to build

## Dependencies
- Qt5, [License](https://doc.qt.io/qt-5/opensourcelicense.html)
- SDL2, [License](https://www.libsdl.org/license.php)

## Supported compilers
- MSVC 2010
- MSVC 2013
- MSVC 2015
- MSVC 2017
- gcc 5
- gcc 6
- gcc 7
- gcc 8
- clang 7

## Compiling
Dynablaster Revenge uses Qt's qmake build system.
Therefore compiling the game should be as easy as this:
```
qmake
jom
```

# Requirements

To run the game you need a video-card with OpenGL 3.0 support and at least
512MB of video memory.

It works properly with the following cards:
- Nvidia Geforce GTX 560 Ti
- Nvidia Geforce 8800 GTS
- Nvidia Geforce 9600 GT
- AMD Radeon HD 6540
- AMD Radeon HD 6850
- AMD Radeon HD 7750
- Intel HD Graphics 4000

If the required OpenGL features are not available, the game will not start 
and you'll just see a message box. In this case it can help to update the 
video driver. Also make sure you're using a driver from the video card 
vendor and not, for example, Microsoft's default driver.

# Help

## Extras

### Bomb
Pick up the bomb extra and you'll have one more bomb to place. This is one of the most simple extras; with it and some practice you may learn tactics to drive your opponents into a corner.

### Flame
Collect the flame extra to create larger detonations. With each additional flame you'll be able to reach one more space with your bombs. Always keep track on the number of bombs your opponents have collected – it could save your life.

### Speedup 
As the name suggests, your player's speed will increase with each speedup collected. Speedups will make your player much more agile and let you reach extras before your opponents do.

### Kick
You’ll either love or hate this one – kick extras enable you to kick bombs away until they reach any obstacle like players, walls or other bombs. This extra will definitely mess up all your opponent's tactics.

### Skulls
This extra actually consists of six separate ones. Once the skull extra is revealed you’ll see a rotating cube; each side of the cube represents a different effect when picked up. There’s one that will make your player drop bombs all the time, another one that will flip your controls, and a third that will restrict your bombs to a single flame. The mushroom skull will make you quite dizzy, but there’s also cool stuff: invisibility and invincibility – yes, invincibility. By the way, the skull extra is predictable – you’ll soon find out how it works.

## Hotkeys

- `[F1]`, Open Hotkey overview
- `[F2]`, Mute music
- `[F3]`, Mute sound effects
- `[F4]`, Display your local IPs
- `[F10]`, End current game (game owner only)
- `[F11]`, Save game playback to disk (currently developer only)
- `[PageUp]`, Next track
- `[PageDown]`, Previews track
- `[Alt+Return]`, Toggle full screen


## Ingame hotkeys

- `[Return]`, Chat
- `[Escape]`, Rage quit (immediately leave the game)
- `[Tab]`, Show player names


##  Multiplayer
   
Quick Guide to Local Multiplayer Setup:

1) Connect a load of USB controllers to your PC. A joypad is required  for each additional player - wired Xbox 360 pads work perfectly.
2) Load the game, set the IP address as 127.0.0.1 and select 'Multi'. This will open a server on your PC. Create a new game and when in the 'Lounge', add your friends to the game by pressing the 'add local player' button. Alternatively you may use the hotkeys `[shift]` and `[+]` or `[ctrl]` and `[=]` depending on what's more convenient with your keyboard layout. 
            
Please note a 'complete' implementation is scheduled for version 1.2; i.e. a new menu page will be introduced for this mode.

## Quick Guide to LAN Multiplayer Setup:

1) Set the IP address to 127.0.0.1, and select 'Multi'. This will open a server on your PC.
2) The other players then just have to connect to your IP address by entering your IP into the hostname field. If you don't know your IP just press F4 in the menus.

## Quick Guide to Online Multiplayer Setup:

The best option here is to have a dedicated server (a 'Linux shell' or a Windows machine connected to a fast internet connection) you can run the server on. If this is not an option, you can use Titan's own server by typing 'dynablaster.titan-sucks.net' in the IP field on the title screen.

If you just want to use your own connection in order to play over the internet, you can still set up a port forwarding from your router (if you have one) to your PC (i.e. make your router forward all traffic on port 6300 to your PCs IP). Host your server on 127.0.0.1, as you would for a LAN game, then tell your friends your internet IP address (not the LAN one of course) so they can connect.

# FAQ

- Q: Why am I having trouble placing bombs and moving the player at
the same time?  
A: This can be related to keyboard ghosting. Have a look at https://web.archive.org/web/20140608171426/http://www.microsoft.com/appliedsciences/antighostingexplained.mspx
in order to see which keys work fine for you.

- Q: Your bots could be improved in so many ways. Why don't you make
them better?  
A: They're not perfect by intention. It makes them more amiable :)

- Q: Why isn't there a Mac port?  
A: We don't have a Mac.


# Contact

All kind of bugreports, ideas and suggestions are greatly appreciated.      
In order to contact us, please send a mail to:

- matthias`[dot]`varnholt`[at]`gmail`[dot]`com or
- hellfire`[at]`untergrund`[dot]`net or
- deathstar`[dot]`gfx`[at]`gmail`[dot]`com

