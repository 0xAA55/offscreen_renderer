CC=gcc
LD=gcc
AR=ar
RANLIB=ranlib
CFLAGS=-O3
CXXFLAGS=-std=c++20 -O3
LDFLAGS=-L.
LDLIBS=-lstdc++

OBJS=
OBJS+=glcore.o
OBJS+=glbuffer.o
OBJS+=glcontext.o
OBJS+=glcore.o
OBJS+=glfwctx.o
OBJS+=inifile.o
OBJS+=rtask.o
OBJS+=strutil.o
OBJS+=taskshader.o
OBJS+=tasksolver.o
OBJS+=tasksstorage.o
OBJS+=tasktex.o
OBJS+=taskthread.o
OBJS+=unibmp.o

all: liboffscr.a

liboffscr.a: $(OBJS)
	$(AR) qc $@ $^
	$(RANLIB) $@

clean:
	rm -f *.a *.o

