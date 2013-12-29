DxPortLib is an SDL2 based portability library that is intended to be as
interface compatible as possible with DxLib, a free open source Japanese
library used for game development.

DxLib's home page: http://homepage2.nifty.com/natupaji/DxLib/

As DxPortLib is designed to work with SDL2 and does not use any system
specific functions, it should run on any system that SDL2 runs on.
Therefore you can now build (some) DxLib based games on Linux and OS X,
amongst other platforms.

Libraries required:
SDL2 - http://www.libsdl.org/
SDL2_image - http://www.libsdl.org/projects/SDL_image/
SDL2_ttf - http://www.libsdl.org/projects/SDL_ttf/
Vorbisfile - http://xiph.org/downloads/

---

Please read PORTING.EXAMPLE.txt for an example of how this library is
to be used on a DxLib application.

---

This library is not yet feature complete. Most notably, the rendering
backend uses SDL2_Renderer, which has a great number of limitations.
It is good enough for many simple games, though.

Replacing it with a fully OpenGL backend is planned, but will take some
time to implement.

---

If there are any comments, you may contact the maintainer:

Patrick McCarthy <mauve@sandwich.net>

Twitter: @mauvecow
