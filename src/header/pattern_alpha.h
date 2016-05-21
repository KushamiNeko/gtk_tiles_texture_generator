#ifndef PATTERN_ALPHA_H
#define PATTERN_ALPHA_H

#include <gtk-3.0/gtk/gtk.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "gl_helper.h"

#define WINDOW_HEIGHT 900
#define CONTROL_BOX_WIDTH 500
#define BOX_SPACE 5

void initPattern(GtkWindow *mainWindow, GtkContainer *container,
                 GtkGLArea *glArea, GLuint shaderProgram);

#endif
