#ifndef PATTERN_MODEL_H
#define PATTERN_MODEL_H

#include <GL/glew.h>
#include <gtk-3.0/gtk/gtk.h>

#include "rectangle.h"
#include "constant.h"
#include "gl_helper.h"
#include "../../../general/header/general_helper.h"

struct PatternModel {
  unsigned int sizeX;
  unsigned int sizeY;

  struct Rectangle **units;
  unsigned int numUnits;
  unsigned int vertexCounts;

  GLfloat *vertexPosition;
  GLfloat *vertexUV;
  GLfloat *vertexColor;

  GLfloat *wireframeColor;

  GLuint vao;
  GLuint positionVBO;
  GLuint uvVBO;
  GLuint colorVBO;
};

// void patternModelInitUV(struct PatternModel *pattern);

void patternModelRandomizeUV(struct PatternModel *pattern);
void patternModelScaleUV(struct PatternModel *pattern, double scaleFactor);

void initPatternGLData(struct PatternModel *pattern);

void patternModelFitColor(struct PatternModel *pattern, float min, float max);
void patternModelRandomizeColor(struct PatternModel *pattern);

struct PatternModel *patternModelNew(GtkGLArea *glArea,
                                     const unsigned int sizeX,
                                     const unsigned int sizeY,
                                     const unsigned int cpy);

void patternModelFree(struct PatternModel *pattern);

#endif
