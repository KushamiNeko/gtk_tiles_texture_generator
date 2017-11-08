ifeq ($(BUILD),debug)
	FLAGS = -O0 -Wall -g -DDEBUG
else
	FLAGS = -O3 -Wall -Werror -s -DNDEBUG
endif

GENERAL_HELPER = ~/programming_projects/c/general/bin

PKG_CONFIG_LIBS = gtk+-3.0 glew glib-2.0

MAIN_PKG_CONFIG_LIBS = $(PKG_CONFIG_LIBS) #glib-2.0

TEST_PKG_CONFIG_LIBS = $(PKG_CONFIG_LIBS) cmockery2

all: bin/rectangle bin/rectangle_test bin/pattern_model \
	bin/pattern_control bin/main_window bin/main

bin/rectangle_test : bin/rectangle
	gcc $(FLAGS) `pkg-config --cflags $(TEST_PKG_CONFIG_LIBS)` \
		-o bin/rectangle_test \
		$(GENERAL_HELPER)/general_helper \
		src/rectangle_test.c -lm \
		`pkg-config --libs $(TEST_PKG_CONFIG_LIBS)`

bin/main : bin/main_window
	gcc $(FLAGS) `pkg-config --cflags $(MAIN_PKG_CONFIG_LIBS)` \
		-o bin/main bin/pattern_control bin/pattern_model bin/rectangle bin/main_window \
		$(GENERAL_HELPER)/general_helper \
		$(GENERAL_HELPER)/gl_helper \
		src/main.c -lm -lstbi `pkg-config --libs $(MAIN_PKG_CONFIG_LIBS)`

bin/main_window: bin/pattern_control
	gcc $(FLAGS) `pkg-config --cflags $(PKG_CONFIG_LIBS)` \
		-c src/main_window.c \
		-o bin/main_window `pkg-config --libs $(PKG_CONFIG_LIBS)`

bin/pattern_control: bin/pattern_model
	gcc $(FLAGS) `pkg-config --cflags $(PKG_CONFIG_LIBS)` \
		-c src/pattern_control.c \
		-o bin/pattern_control `pkg-config --libs $(PKG_CONFIG_LIBS)`


bin/pattern_model: bin/rectangle
	gcc $(FLAGS) `pkg-config --cflags $(PKG_CONFIG_LIBS)` \
		-c src/pattern_model.c \
		-o bin/pattern_model `pkg-config --libs $(PKG_CONFIG_LIBS)`

bin/rectangle:
	gcc $(FLAGS) -c src/rectangle.c -o bin/rectangle

clean:
	rm -f bin/*

release:
	make clean
	make

debug:
	make clean
	make "BUILD=debug"

test:
	make debug
	bin/rectangle_test

