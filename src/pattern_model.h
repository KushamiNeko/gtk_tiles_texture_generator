#ifndef PATTERN_MODEL_H
#define PATTERN_MODEL_H

#include <GL/glew.h>
#include <gtk-3.0/gtk/gtk.h>
#include <glib-2.0/glib.h>

#include "constant.h"
#include "rectangle.h"

#include "../../general/src/general_helper.h"

#include "../../general/src/gl_helper.h"

struct PatternModel {
  unsigned int sizeX;
  unsigned int sizeY;

  unsigned int numWidth;
  unsigned int numHeight;

  struct Rectangle **units;

  unsigned int numUnits;
  unsigned int vertexCounts;
  unsigned int wireframeVertexCounts;

  GLfloat *vertexPosition;
  GLfloat *vertexUV;
  GLfloat *vertexColor;

  GLfloat *vertexWireframe;

  GLuint vao;
  GLuint positionVBO;
  GLuint uvVBO;
  GLuint colorVBO;

  GLuint wireframeVBO;

  struct PatternModel *seamlessModel;
};

void patternModelRandomizeUV(struct PatternModel *pattern);

void patternModelScaleUV(struct PatternModel *pattern,
                         double scaleFactor);

void patternModelSeamlessModelConstruct(struct PatternModel *pattern,
                                        GtkGLArea *glArea);
void patternModelInitUnitsPosition(struct PatternModel *pattern);

void patternModelFitColor(struct PatternModel *pattern, float min,
                          float max);
void patternModelRandomizeColor(struct PatternModel *pattern);

void patternModelRandomizeUVRotate(struct PatternModel *pattern);

struct PatternModel *patternModelNew(GtkGLArea *glArea,
                                     const unsigned int sizeX,
                                     const unsigned int sizeY,
                                     const unsigned int cpy,
                                     const unsigned int patternIndex);

void patternModelFree(struct PatternModel *pattern);

#endif
