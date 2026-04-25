µGLy
====

The µGLy project aims to fully implement the OpenGL ES 1.0 Common Lite
specification, by following some basic constrains:

- Correctness above all
- Memory frugality
- Flexibility
- Speed

The aspect of flexibility is where this implementation offers something
interesting for embedded developers: the option to turn off certain features (at
the cost of no longer adhering to the standards).

In part, the memory frugality also plays a key here: the goal is to use arenas
whenever possible, so the user has to provide its own memory pool for the
renderer. Formulas for calculating the necessary memory will be provided for
each configuration.

The correctness is another important aspect to note - given that GL ES 1.0 CL is
exclusively a fixed-point math standard, there will be degree of error when
compared to, say, Mesa's own GL ES 1.0 implementation; but that deviation is due
to the fact that Mesa uses floating points internally, despite exposing a
`GLfixed` interface. This is, of course, no dig at the fine folks at Mesa (we have
a lot of respect to them and some of us collaborate on the project as well).

Finally, note that speed is listed as the last point and while it's not the most
important aspect, it will not neglect and whenever possible, the most optimal
algorithms will be used. With careful tuning, applications can achieve good
speeds using µGLy.

# Getting started

µGLy, as of now, requires CMake and a valid ISO C90 compiler.
The test application expects to **be run from the root of the project**.

The development builds of µGLy uses SDL2 and SDL2_image for loading png files
(alternatively, you can include the file `samples/tex64x64xTGBA32.h`).

If you want to use µGLy on other platforms, you will need to provide
the following functions:

- `void initWindow( KeyCallback callback)`
- `void graphicsShutdown(void)`
- `void swapBuffers(void)`

Optionally, you can provide this, if you don't use `samples/tex64x64xTGBA32.h`.

- `struct Bitmap* loadBitmap(const char *filename)`

## Supported systems

µGLy has been tested in the following scenarios:

- MacOS 14 (aarch64)
- MacOS 12 (x86_64)
- MacOS 10.7 (x86_64)
- Ubuntu Linux (x86_64)
- Debian (aarch64, including inside Android, using UserLAnd and XServerSDL)
- Apache NuttX (both using ASCII output and ST7735S-based displays), running on boards such as the NUCLEO F767ZI and
  RaspberryPi Pico
- Bare metal on the Raspberry Pi Pico W (using a ST7789VE-based display, as well as a ST7735S one)

This is hardly surprising, given that the rasterizer used as a basis for µGLy once ran on platforms as limited as the
Nintendo Game Boy Advance and the Commodore Amiga CD32, as well as PowerPC platforms and 486/DOS.
