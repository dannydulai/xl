/*
 *  Danny Dulai
 *  
 *  Simple, transparent screen locker in X11
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
    Window root;
    int len = 0, i;
    XEvent ev;
    char keybuffer[1024], *passwd, *pasbuf = NULL;
   
    if (argc > 1) {
	setenv("XLPASSWD", argv[1], 1);
	execl("/proc/self/exe", argv[0], NULL);
	fprintf(stderr, "failed to execute self, use XLPASSWD environment\n");
	exit(1);
    }
    else if ((passwd = getenv("XLPASSWD")))
	passwd = pasbuf = strdup(crypt(passwd, "ax"));
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
		if (pasbuf) free(pasbuf);
		exit(1);
	    }
	}
#endif
	if (!passwd) {
	    fprintf(stderr, "could not get passwd\n");
	    if (pasbuf) free(pasbuf);
	    exit(1);
	}
    }

    if (strlen(passwd) < 2) {
	fprintf(stderr, "passwd too short\n");
	if (pasbuf) free(pasbuf);
	exit(1);
    }

    if ((display = XOpenDisplay(NULL)) == NULL) {
	fprintf(stderr, "could not connect to $DISPLAY\n");
	if (pasbuf) free(pasbuf);
	exit(1);
    }

    root = DefaultRootWindow(display);
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
		    if (pasbuf) free(pasbuf);
		    exit(0);
		} else 
		    len = 0;
	    } else 
		len += i;
	}
    }
}
