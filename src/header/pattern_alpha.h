#ifndef PATTERN_ALPHA_H
#define PATTERN_ALPHA_H

#include <gtk-3.0/gtk/gtk.h>
#include <GL/glew.h>

#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "gl_helper.h"
#include "constant.h"

#include "pattern_model.h"
#include "../../../general/header/general_helper.h"

void *patternAlphaNew(GtkWindow *mainWindow, GtkContainer *container,
                      GtkGLArea *glArea, GLuint shaderProgram);

void patternAlphaFree(void *patternAlpha);

// void *initPattern(GtkWindow *mainWindow, GtkContainer *container,
//                  GtkGLArea *glArea, GLuint shaderProgram);

#endif
