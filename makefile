bin/main : bin/pattern_control 
	gcc `pkg-config --cflags gtk+-3.0 glew` \
-o bin/main bin/pattern_control bin/pattern_model bin/rectangle \
~/programming_projects/c/general/bin/general_helper \
~/programming_projects/c/general/bin/gl_helper \
src/main.c -lm `pkg-config --libs gtk+-3.0 glew`


bin/pattern_control: bin/pattern_model 
	gcc `pkg-config --cflags gtk+-3.0 glew` -c src/source/pattern_control.c \
`pkg-config --libs gtk+-3.0 glew` -o bin/pattern_control 


bin/pattern_model: bin/rectangle 
	gcc `pkg-config --cflags gtk+-3.0 glew` -c src/source/pattern_model.c -o bin/pattern_model \
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
