all: triangle cube

triangle: examples/triangle.c src/vector.c
	gcc -o triangle examples/triangle.c src/vector.c $(shell sdl2-config --cflags --libs)

cube: examples/cube.c src/vector.c src/matrix.c src/renderer.c src/camera.c
	gcc -o cube examples/cube.c src/vector.c src/matrix.c src/renderer.c src/camera.c $(shell sdl2-config --cflags --libs) -lm

clean:
	rm -f triangle cube
