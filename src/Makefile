CC = gcc
CFLAGS = -ggdb -fopenmp

voronoi.ppm: voronoi
	./voronoi

voronoi: voronoi.c ppm/ppm.o

ppm/ppm.o: ppm/ppm.c
	$(MAKE) -C ppm

clean:
	-rm	voronoi voronoi.ppm
	$(MAKE) -C ppm clean
