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
GLfixed interface. This is, of course, no dig at the fine folks at Mesa (we have
a lot of respect to them and some of us collaborate on the project as well).

Finally, note that speed is listed as the last point and while it's not the most
important aspect, it will not neglected and whenever possible, the most optimal
algorithms will be used. With careful tunning, applications can achieve good
speeds using µGLy.

More details on the rendering and state machine management TBD.
