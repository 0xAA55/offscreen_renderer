CC=gcc
LD=gcc
CFLAGS=-O3 -fopenmp -I. -Iinclude -fmax-errors=5
CXXFLAGS=$(CFLAGS) -std=c++20
LDFLAGS=-L. -Lliboffscr -Lunibmp
LDLIBS=-lm -lstdc++ -loffscr -lunibmp -lgomp -lGL -lEGL

all: gpurenderer

gpurenderer: offscr/main.o liboffscr/liboffscr.a unibmp/libunibmp.a
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

liboffscr/liboffscr.a:
	$(MAKE) -C liboffscr

unibmp/libunibmp.a:
	$(MAKE) -C unibmp

clean:
	rm -f gpurenderer *.o *.a
	$(MAKE) -C liboffscr clean
	$(MAKE) -C unibmp clean
