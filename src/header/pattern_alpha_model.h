#ifndef PATTERN_ALPHA_MODEL_H
#define PATTERN_ALPHA_MODEL_H

#include <GL/glew.h>
#include <gtk-3.0/gtk/gtk.h>

#include "rectangle.h"
#include "constant.h"
//#include "gl_helper.h"
#include "../../../general/src/header/general_helper.h"
//#include "../../../general/src/header/general_list.h"
#include "../../../general/src/header/gl_helper.h"

struct PatternModel {
  unsigned int sizeX;
  unsigned int sizeY;

  unsigned int numWidth;
  unsigned int numHeight;

  struct Rectangle **units;
  unsigned int numUnits;
  unsigned int vertexCounts;

  GLfloat *vertexPosition;
  GLfloat *vertexUV;
  GLfloat *vertexColor;

  GLuint vao;
  GLuint positionVBO;
  GLuint uvVBO;
  GLuint colorVBO;

  struct PatternModel *seamlessModel;
};

extern void patternModelRandomizeUV(struct PatternModel *pattern);

extern void patternModelScaleUV(struct PatternModel *pattern,
                                double scaleFactor);

extern void patternModelSeamlessModelConstruct(struct PatternModel *pattern,
                                               GtkGLArea *glArea);
extern void patternModelInitUnitsPosition(struct PatternModel *pattern);

extern void patternModelFitColor(struct PatternModel *pattern, float min,
                                 float max);
extern void patternModelRandomizeColor(struct PatternModel *pattern);

extern void patternModelRandomizeUVRotate(struct PatternModel *pattern
                                          );

extern struct PatternModel *patternModelNew(GtkGLArea *glArea,
                                            const unsigned int sizeX,
                                            const unsigned int sizeY,
                                            const unsigned int cpy);

extern void patternModelFree(struct PatternModel *pattern);

#endif
