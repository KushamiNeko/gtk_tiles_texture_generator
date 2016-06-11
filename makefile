bin/main : bin/gl_helper bin/pattern_alpha
	gcc `pkg-config --cflags gtk+-3.0 glew` \
-o bin/main bin/gl_helper bin/pattern_alpha bin/pattern_alpha_model bin/rectangle \
~/programming_projects/c/general/bin/general_helper \
-lstbi -lm src/main.c `pkg-config --libs gtk+-3.0 glew`


bin/pattern_alpha: bin/pattern_alpha_model bin/gl_helper
	gcc `pkg-config --cflags gtk+-3.0 glew` -c src/source/pattern_alpha.c \
`pkg-config --libs gtk+-3.0 glew` -o bin/pattern_alpha 


bin/pattern_alpha_model: bin/rectangle
	gcc `pkg-config --cflags gtk+-3.0 glew` -c src/source/pattern_alpha_model.c -o bin/pattern_alpha_model \
		`pkg-config --libs gtk+-3.0 glew`

bin/gl_helper : 
	gcc `pkg-config --cflags glew` -c src/source/gl_helper.c \
`pkg-config --libs glew` -o bin/gl_helper

bin/rectangle:
	gcc -c src/source/rectangle.c -o bin/rectangle

clean:
	rm -r bin/*

all:
	make clean
	make
