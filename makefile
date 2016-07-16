bin/main : bin/control 
	gcc `pkg-config --cflags gtk+-3.0 glew` \
-o bin/main bin/control bin/model_base bin/rectangle \
~/programming_projects/c/general/bin/general_helper \
~/programming_projects/c/general/bin/gl_helper \
src/main.c -lstbi -lm `pkg-config --libs gtk+-3.0 glew`


bin/control: bin/model_base 
	gcc `pkg-config --cflags gtk+-3.0 glew` -c src/source/control.c \
`pkg-config --libs gtk+-3.0 glew` -o bin/control 


bin/model_base: bin/rectangle 
	gcc `pkg-config --cflags gtk+-3.0 glew` -c src/source/model_base.c -o bin/model_base \
		`pkg-config --libs gtk+-3.0 glew`

bin/rectangle:
	gcc -c src/source/rectangle.c -o bin/rectangle

clean:
	@if [ $(shell find 'bin' -type d -empty)  ]; then\
		echo 'bin is already clean';\
	else\
		echo 'cleaning bin ...';\
		rm -r bin/*;\
	fi

all:
	make clean
	make
