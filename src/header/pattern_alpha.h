#ifndef PATTERN_ALPHA_H
#define PATTERN_ALPHA_H

#include <gtk-3.0/gtk/gtk.h>
#include <GL/glew.h>

#include <stdlib.h>
#include <time.h>
#include <string.h>

//#include "gl_helper.h"
#include "constant.h"

#include "pattern_alpha_model.h"
#include "../../../general/src/header/general_helper.h"
#include "../../../general/src/header/gl_helper.h"

extern void *patternAlphaNew(const char *patternName);

#endif
