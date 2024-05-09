CC=gcc
LD=gcc
AR=ar
RANLIB=ranlib
CFLAGS=-O3 -fopenmp -I. -I.. -I../include
CXXFLAGS=$(CFLAGS) -std=c++20
LDFLAGS=-L.
LDLIBS=-lstdc++ -lgomp -lGL -lEGL -lglfw

OBJS=
OBJS+=glcore.o
OBJS+=glbuffer.o
OBJS+=glcontext.o
OBJS+=glcore.o
OBJS+=eglctx.o
OBJS+=glfwctx.o
OBJS+=inifile.o
OBJS+=rtask.o
OBJS+=strutil.o
OBJS+=taskshader.o
OBJS+=tasksolver.o
OBJS+=tasksstorage.o
OBJS+=tasktex.o
OBJS+=taskthread.o

all: liboffscr.a

liboffscr.a: $(OBJS)
	$(AR) qc $@ $(OBJS) 
	$(RANLIB) $@

clean:
	rm -f *.a *.o

