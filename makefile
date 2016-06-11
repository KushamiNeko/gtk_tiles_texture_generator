#bin/main : bin/gl_helper bin/pattern_alpha
#	gcc `pkg-config --cflags gtk+-3.0 glew glfw3` \
#-o bin/main bin/gl_helper bin/pattern_alpha -lstbi -lm src/main.c \
#`pkg-config --libs gtk+-3.0 glew glfw3`
#
#bin/gl_helper : 
#	gcc `pkg-config --cflags glew glfw3` -c src/source/gl_helper.c \
#`pkg-config --libs glew glfw3` -o bin/gl_helper

bin/pattern_alpha: bin/pattern_model
	gcc `pkg-config --cflags gtk+-3.0 glew` -c src/source/pattern_alpha.c \
`pkg-config --libs gtk+-3.0 glew` -o bin/pattern_alpha 


bin/pattern_model: bin/rectangle
	gcc `pkg-config --cflags gtk+-3.0 glew` -c src/source/pattern_model.c -o bin/pattern_model \
		`pkg-config --libs gtk+-3.0 glew`

bin/rectangle:
	gcc -c src/source/rectangle.c -o bin/rectangle

clean:
	rm -r bin/*

build:
	make clean
	make
