bin/main : bin/gl_helper bin/pattern_alpha
	gcc `pkg-config --cflags gtk+-3.0 glew glfw3` \
-o bin/main bin/gl_helper bin/pattern_alpha -lstbi -lm src/main.c \
`pkg-config --libs gtk+-3.0 glew glfw3`

bin/gl_helper : 
	gcc `pkg-config --cflags glew glfw3` -c src/source/gl_helper.c \
`pkg-config --libs glew glfw3` -o bin/gl_helper

bin/pattern_alpha:
	gcc `pkg-config --cflags gtk+-3.0 glew glfw3` -c src/source/pattern_alpha.c \
`pkg-config --libs gtk+-3.0 glew glfw3` -o bin/pattern_alpha 

clean:
	rm -r bin/*

build:
	make clean
	make
