CC = gcc
CFLAGS = -ggdb -fopenmp

vonoroi.ppm: vonoroi
	./vonoroi

vonoroi: vonoroi.c ppm/ppm.o

ppm/ppm.o: ppm/ppm.c
	$(MAKE) -C ppm

clean:
	-rm	vonoroi vonoroi.ppm
	$(MAKE) -C ppm clean
