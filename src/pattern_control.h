#ifndef PATTERN_CONTROL_H
#define PATTERN_CONTROL_H

#include <GL/glew.h>
#include <gtk-3.0/gtk/gtk.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "constant.h"

#include "../../general/src/general_helper.h"
#include "../../general/src/gl_helper.h"
#include "pattern_model.h"

void *initPatternControl(GtkWindow *mainWindow, GtkContainer *container,
                                GtkGLArea *glArea, GLuint shaderProgram);
#endif