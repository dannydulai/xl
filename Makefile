# uncomment next line if you need shadow passwd support
#NEEDSHADOW= -DNEED_SHADOW

X11HOME = /usr/X11R6

CC	= gcc
CFLAGS	= -O2 -Wall -I${X11HOME}/include ${NEEDSHADOW}
LIBS	= -L${X11HOME}/lib -lX11

# Uncomment for systems with libcrypt
LIBS   += -lcrypt

# uncomment if you are running under solaris
#LIBS   += -lnsl -lsocket

OBJS = main.o

all:	xl

xl: $(OBJS)
	$(CC) $(CFLAGS) -o xl $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS)

realclean: clean
	rm -f xl
