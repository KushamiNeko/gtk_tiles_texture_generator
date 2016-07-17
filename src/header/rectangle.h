#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <GL/gl.h>
#include <math.h>
#include <stdio.h>

#include "../../../general/src/header/general_helper.h"
#include "constant.h"

struct Rectangle {
  GLfloat **position;
  GLfloat **uv;

  GLfloat width;
  GLfloat height;
  GLfloat color;

  GLfloat rotateDegree;

  unsigned int pivot;
  // unsigned int *xMax;
  // unsigned int *xMin;
  // unsigned int *yMax;
  // unsigned int *yMin;

  unsigned int xMax;
  unsigned int xMin;
  unsigned int yMax;
  unsigned int yMin;

  unsigned int *vertexOrder;

  // GLfloat *pivot;

  GLfloat *vertexPosition;
  GLfloat *vertexUV;
  GLfloat *vertexColor;

  GLfloat *vertexWireframe;

  unsigned int vertexCounts;
  unsigned int wireframeVertexCounts;
};

struct Rectangle *rectangleNew();

struct Rectangle *rectangleClone(struct Rectangle *rect);

extern void rectangleMove(struct Rectangle *rect, GLfloat x, GLfloat y);

extern void rectangleMovePoint(struct Rectangle *rect, unsigned int pointNumber,
                               GLfloat x, GLfloat y);

extern void rectangleMoveEdge(struct Rectangle *rect,
                              unsigned int pointNumber01,
                              unsigned int pointNumber02, GLfloat x, GLfloat y);

extern void rectangleMoveTo(struct Rectangle *rect, GLfloat x, GLfloat y);

extern void rectangleInitUVScale(struct Rectangle *rect);

extern void rectangleSetWidth(struct Rectangle *rect, GLfloat width);

extern void rectangleSetHeight(struct Rectangle *rect, GLfloat height);

extern void rectangleSetColorValue(struct Rectangle *rect, GLfloat colorValue);

extern void rectangleMoveUV(struct Rectangle *rect, GLfloat x, GLfloat y);

extern void rectangleScaleUV(struct Rectangle *rect, double scaleFactor);

extern void rectangleRotateUV(struct Rectangle *rect, float degree);

extern void rectangleFree(struct Rectangle *rect);

#endif
