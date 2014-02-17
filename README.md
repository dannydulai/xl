xl: The simple X11 lock
==

xscreensaver, xlock, xlockmore, all of them, completley useless. They have overflows, they have buggy drawning routines, they are very prone to crashing. The more complex they get, the more useless they are. If a screen locking program decides to crash on me, its not doing a very good job of keeping my terminal locked. I've had many people just go to my screen and hit tons of keys just to make it crash, or cycling the screen saver to get to a certain one known to crash. Absolutly ridiculous!

This is my solution to the problem. Very simple, very small, very few features. Doesn't even blank your screen, all this program does is lock your keyboard and mouse, and only unlocks it if you type your password and hit return. The password is the first argument of the command line or, if XLPASSWD environment variable is set, the contents of XLPASSWD, otherwise your UNIX password.
