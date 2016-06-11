#ifndef RECTANGLE_UNIT_H
#define RECTANGLE_UNIT_H

#include <math.h>
#include <stdio.h>
#include <GL/gl.h>

#include "../../../general/header/general_helper.h"

struct Rectangle {
  GLfloat **position;
  GLfloat **uv;

  GLfloat width;
  GLfloat height;
  GLfloat color;

  unsigned int pivot;
  unsigned int *xMax;
  unsigned int *xMin;
  unsigned int *yMax;
  unsigned int *yMin;

  unsigned int *vertexOrder;

  GLfloat *vertexPosition;
  GLfloat *vertexUV;
  GLfloat *vertexColor;

  unsigned int vertexCounts;
};

struct Rectangle *rectangleNew();
void rectangleMove(struct Rectangle *rect, GLfloat x, GLfloat y);
void rectangleMoveTo(struct Rectangle *rect, GLfloat x, GLfloat y);
void rectangleSetWidth(struct Rectangle *rect, GLfloat width);
void rectangleSetHeight(struct Rectangle *rect, GLfloat height);
void rectangleSetColorValue(struct Rectangle *rect, GLfloat colorValue);
void rectangleMoveUV(struct Rectangle *rect, GLfloat x, GLfloat y);
void rectangleScaleUV(struct Rectangle *rect, double scaleFactor);
void rectangleRotateUV(struct Rectangle *rect, float degree);
void rectangleFree(struct Rectangle *rect);

#endif
