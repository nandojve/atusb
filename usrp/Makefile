CFLAGS=-Wall -O9
LDLIBS=-lm
LDLIBS_fft=-lfftw3 -lm

all:	peak avg fft

fft:	fft.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS_fft)
