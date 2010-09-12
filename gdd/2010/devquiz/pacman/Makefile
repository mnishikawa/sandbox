# GDD2010 devquiz pacman makefile

CC	= gcc
CFLAGS	= -O
LDFLAGS	=
INCLUDES 	= -I/usr/local/include -I.
LIBS	= 
TARGET	= pacman
OBJS	= pacman.o pacman-sim.o


$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	-rm -f $(TARGET) $(OBJS) *~ \#* core

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

