#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
//#include <assert.h>

#include "../../general/src/general_helper.h"
#include "constant.h"

struct Rectangle {
  GLfloat **position;
  GLfloat **uv;

  double width;
  double height;
  double color;

  //double rotateDegree;

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

void rectangleMove(struct Rectangle *rect, double x, double y);

void rectangleMovePoint(struct Rectangle *rect, unsigned int pointNumber,
                        double x, double y);

void rectangleMoveEdge(struct Rectangle *rect,
                       unsigned int pointNumber01,
                       unsigned int pointNumber02, double x, double y);

void rectangleMoveTo(struct Rectangle *rect, double x, double y);

void rectangleInitUVProject(struct Rectangle *rect);

void rectangleSetWidth(struct Rectangle *rect, double width);

void rectangleSetHeight(struct Rectangle *rect, double height);

void rectangleSetColorValue(struct Rectangle *rect, double colorValue);

void rectangleMoveUV(struct Rectangle *rect, double x, double y);

void rectangleScaleUV(struct Rectangle *rect, double scaleFactor);

void rectangleRotateUV(struct Rectangle *rect, float degree);

void rectangleFree(struct Rectangle *rect);

#endif
