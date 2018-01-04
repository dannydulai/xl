/*
 *  Danny Dulai
 *
 *  Simple, transparent screen locker in X11
 *  Screen blanking added by Alex Yuriev <github.com/alexyuriev>
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <crypt.h>
#include <string.h>
#include <pwd.h>

#ifdef NEED_SHADOW
#include <shadow.h>
#endif

int
main(int argc, char *argv[])
{
    Display *display;
    XEvent ev;

    int len = 0, i;
    char keybuffer[1024], *passwd;

    if (argc > 1) {
	setenv("XLPASSWD", argv[1], 1);
	execl("/proc/self/exe", argv[0], NULL);
	fprintf(stderr, "failed to execute self, use XLPASSWD environment\n");
	exit(1);
    }
    else if ((passwd = getenv("XLPASSWD")))
	passwd = strdup(crypt(passwd, "ax"));
    else {
#ifndef NEED_SHADOW
	passwd = getpwuid(getuid())->pw_passwd;
#else
	{
		struct spwd *sp = getspnam(getpwuid(getuid())->pw_name);
		if (sp)
			passwd = sp->sp_pwdp;
		else {
			fprintf(stderr, "could not get shadow entry\n");
			exit(1);
		}
    }
#endif
    if (!passwd) {
	fprintf(stderr, "could not get passwd\n");
    exit(1);
    }
    }

    if (strlen(passwd) < 2) {
    fprintf(stderr, "passwd too short\n");
    exit(1);
    }

    if ((display = XOpenDisplay(NULL)) == NULL) {
    fprintf(stderr, "could not connect to $DISPLAY\n");
    exit(1);
    }

    Window root = DefaultRootWindow(display);
    if ( root < 0 ) {
        fprintf(stderr, "Failed to get a root window ID of a default screen\n");
        exit(1);
    }

    int screen = DefaultScreen( display );


    XWindowAttributes xwRootWinAttr;
    Status statusRet = XGetWindowAttributes( display, root, &xwRootWinAttr );

    Window w = XCreateSimpleWindow( display, root, 0, 0, xwRootWinAttr.width, xwRootWinAttr.height, 0, 0, BlackPixel(display, screen) );

    XColor black;
    black.red   = 0;
    black.green = 0;
    black.blue  = 0;

    static char noData[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    Pixmap emptyBitMap = XCreateBitmapFromData( display, w, noData, 8, 8);

    Cursor invisibleCursor = XCreatePixmapCursor( display, emptyBitMap, emptyBitMap, &black, &black, 0, 0);
    XDefineCursor( display, w, invisibleCursor );
    XFreeCursor( display, invisibleCursor );
    XFreePixmap( display, emptyBitMap );

    /* Attributes for the locked window */

    XSetWindowAttributes xwWindowSetAttr;
    xwWindowSetAttr.save_under        = True;
    xwWindowSetAttr.override_redirect = True;

    XChangeWindowAttributes( display, w, CWOverrideRedirect | CWSaveUnder, &xwWindowSetAttr );
    XMapWindow( display, w);

    XGrabPointer(display, root, 1, ButtonPress, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
    XGrabKeyboard(display, root, 0, GrabModeAsync, GrabModeAsync, CurrentTime);
    XSelectInput(display, root, KeyPressMask);

    while (XNextEvent(display, &ev), 1) {
    if (ev.type == KeyPress) {
        KeySym keysym;
        XComposeStatus compose;
        if (len > sizeof(keybuffer)-10)
        len = 0;
        i = XLookupString(&ev.xkey, keybuffer+len, 10, &keysym, &compose);

        if (keysym == XK_Return) {
        keybuffer[len] = 0;
        if (len && !strcmp(crypt(keybuffer, passwd), passwd)) {
            XUngrabKeyboard(display, CurrentTime);
            XUngrabPointer(display, CurrentTime);
            exit(0);
        } else
            len = 0;
        } else
        len += i;
    }
    }
}
