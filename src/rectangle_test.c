#include "rectangle.h"
#include <assert.h>
#include "rectangle.c"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <cmockery/cmockery.h>

// struct Rectangle *rectangleNew();
// void rectangleFree(struct Rectangle *rect);

static void test_memoryLeakAndInit(void **state) {
  struct Rectangle *rect = rectangleNew();
  assert(rect->height == 1.0f);
  assert(rect->width == 1.0f);
  //assert(rect->rotateDegree == 0.0f);

  assert_int_equal(rect->vertexCounts, 6);
  assert_int_equal(rect->pivot, 0);
  assert_int_equal(rect->xMax, 0);
  assert_int_equal(rect->yMax, 0);
  assert_int_equal(rect->xMin, 1);
  assert_int_equal(rect->yMin, 2);

  assert(rect->position[0][0] == 1.0f);
  assert(rect->position[0][1] == 1.0f);
  assert(rect->position[0][2] == 0.0f);

  assert(rect->position[1][0] == -1.0f);
  assert(rect->position[1][1] == 1.0f);
  assert(rect->position[1][2] == 0.0f);

  assert(rect->position[2][0] == -1.0f);
  assert(rect->position[2][1] == -1.0f);
  assert(rect->position[2][2] == 0.0f);

  assert(rect->position[3][0] == 1.0f);
  assert(rect->position[3][1] == -1.0f);
  assert(rect->position[3][2] == 0.0f);

  assert(rect->vertexPosition[0] == 1.0f);
  assert(rect->vertexPosition[1] == 1.0f);
  assert(rect->vertexPosition[2] == 0.0f);

  assert(rect->vertexPosition[3] == -1.0f);
  assert(rect->vertexPosition[4] == 1.0f);
  assert(rect->vertexPosition[5] == 0.0f);

  assert(rect->vertexPosition[6] == -1.0f);
  assert(rect->vertexPosition[7] == -1.0f);
  assert(rect->vertexPosition[8] == 0.0f);

  assert(rect->vertexPosition[9] == -1.0f);
  assert(rect->vertexPosition[10] == -1.0f);
  assert(rect->vertexPosition[11] == 0.0f);

  assert(rect->vertexPosition[12] == 1.0f);
  assert(rect->vertexPosition[13] == -1.0f);
  assert(rect->vertexPosition[14] == 0.0f);

  assert(rect->vertexPosition[15] == 1.0f);
  assert(rect->vertexPosition[16] == 1.0f);
  assert(rect->vertexPosition[17] == 0.0f);

  assert(rect->uv[0][0] == 1.0f);
  assert(rect->uv[0][1] == 1.0f);

  assert(rect->uv[1][0] == 0.0f);
  assert(rect->uv[1][1] == 1.0f);

  assert(rect->uv[2][0] == 0.0f);
  assert(rect->uv[2][1] == 0.0f);

  assert(rect->uv[3][0] == 1.0f);
  assert(rect->uv[3][1] == 0.0f);

  assert_int_equal(rect->vertexOrder[0], 0);
  assert_int_equal(rect->vertexOrder[1], 1);
  assert_int_equal(rect->vertexOrder[2], 2);

  assert_int_equal(rect->vertexOrder[3], 2);
  assert_int_equal(rect->vertexOrder[4], 3);
  assert_int_equal(rect->vertexOrder[5], 0);

  assert_int_equal(rect->wireframeVertexCounts, 8);
  rectangleFree(rect);
}

// void rectangleSetColorValue(struct Rectangle *rect, GLfloat colorValue);

static void test_setColorValue(void **state) {
  struct Rectangle *rect = rectangleNew();
  assert(rect->color == 1.0f);
  double newColor = 0.5f;
  rectangleSetColorValue(rect, newColor);
  assert(rect->color == newColor);
  rectangleFree(rect);
}

// void rectangleMove(struct Rectangle *rect, GLfloat x, GLfloat y);

static void test_move(void **state) {
  struct Rectangle *rect = rectangleNew();
  double moveX = 0.7f;
  double moveY = 0.3f;
  rectangleMove(rect, moveX, moveY);

  assert(rect->position[0][0] == 1.7f);
  assert(rect->position[0][1] == 1.3f);
  assert(rect->position[0][2] == 0.0f);

  assert(rect->position[1][0] == -0.3f);
  assert(rect->position[1][1] == 1.3f);
  assert(rect->position[1][2] == 0.0f);

  assert(rect->position[2][0] == -0.3f);
  assert(rect->position[2][1] == -0.7f);
  assert(rect->position[2][2] == 0.0f);

  assert(rect->position[3][0] == 1.7f);
  assert(rect->position[3][1] == -0.7f);
  assert(rect->position[3][2] == 0.0f);

  assert(rect->vertexPosition[0] == 1.7f);
  assert(rect->vertexPosition[1] == 1.3f);
  assert(rect->vertexPosition[2] == 0.0f);

  assert(rect->vertexPosition[3] == -0.3f);
  assert(rect->vertexPosition[4] == 1.3f);
  assert(rect->vertexPosition[5] == 0.0f);

  assert(rect->vertexPosition[6] == -0.3f);
  assert(rect->vertexPosition[7] == -0.7f);
  assert(rect->vertexPosition[8] == 0.0f);

  assert(rect->vertexPosition[9] == -0.3f);
  assert(rect->vertexPosition[10] == -0.7f);
  assert(rect->vertexPosition[11] == 0.0f);

  assert(rect->vertexPosition[12] == 1.7f);
  assert(rect->vertexPosition[13] == -0.7f);
  assert(rect->vertexPosition[14] == 0.0f);

  assert(rect->vertexPosition[15] == 1.7f);
  assert(rect->vertexPosition[16] == 1.3f);
  assert(rect->vertexPosition[17] == 0.0f);

  rectangleFree(rect);

  rect = rectangleNew();
  moveX = -0.7f;
  moveY = -0.3f;
  rectangleMove(rect, moveX, moveY);

  assert(rect->position[0][0] == 0.3f);
  assert(rect->position[0][1] == 0.7f);
  assert(rect->position[0][2] == 0.0f);

  assert(rect->position[1][0] == -1.7f);
  assert(rect->position[1][1] == 0.7f);
  assert(rect->position[1][2] == 0.0f);

  assert(rect->position[2][0] == -1.7f);
  assert(rect->position[2][1] == -1.3f);
  assert(rect->position[2][2] == 0.0f);

  assert(rect->position[3][0] == 0.3f);
  assert(rect->position[3][1] == -1.3f);
  assert(rect->position[3][2] == 0.0f);

  assert(rect->vertexPosition[0] == 0.3f);
  assert(rect->vertexPosition[1] == 0.7f);
  assert(rect->vertexPosition[2] == 0.0f);

  assert(rect->vertexPosition[3] == -1.7f);
  assert(rect->vertexPosition[4] == 0.7f);
  assert(rect->vertexPosition[5] == 0.0f);

  assert(rect->vertexPosition[6] == -1.7f);
  assert(rect->vertexPosition[7] == -1.3f);
  assert(rect->vertexPosition[8] == 0.0f);

  assert(rect->vertexPosition[9] == -1.7f);
  assert(rect->vertexPosition[10] == -1.3f);
  assert(rect->vertexPosition[11] == 0.0f);

  assert(rect->vertexPosition[12] == 0.3f);
  assert(rect->vertexPosition[13] == -1.3f);
  assert(rect->vertexPosition[14] == 0.0f);

  assert(rect->vertexPosition[15] == 0.3f);
  assert(rect->vertexPosition[16] == 0.7f);
  assert(rect->vertexPosition[17] == 0.0f);

  rectangleFree(rect);

  rect = rectangleNew();
  moveX = 0.0f;
  moveY = 0.0f;
  rectangleMove(rect, moveX, moveY);

  assert(rect->position[0][0] == 1.0f);
  assert(rect->position[0][1] == 1.0f);
  assert(rect->position[0][2] == 0.0f);

  assert(rect->position[1][0] == -1.0f);
  assert(rect->position[1][1] == 1.0f);
  assert(rect->position[1][2] == 0.0f);

  assert(rect->position[2][0] == -1.0f);
  assert(rect->position[2][1] == -1.0f);
  assert(rect->position[2][2] == 0.0f);

  assert(rect->position[3][0] == 1.0f);
  assert(rect->position[3][1] == -1.0f);
  assert(rect->position[3][2] == 0.0f);

  assert(rect->vertexPosition[0] == 1.0f);
  assert(rect->vertexPosition[1] == 1.0f);
  assert(rect->vertexPosition[2] == 0.0f);

  assert(rect->vertexPosition[3] == -1.0f);
  assert(rect->vertexPosition[4] == 1.0f);
  assert(rect->vertexPosition[5] == 0.0f);

  assert(rect->vertexPosition[6] == -1.0f);
  assert(rect->vertexPosition[7] == -1.0f);
  assert(rect->vertexPosition[8] == 0.0f);

  assert(rect->vertexPosition[9] == -1.0f);
  assert(rect->vertexPosition[10] == -1.0f);
  assert(rect->vertexPosition[11] == 0.0f);

  assert(rect->vertexPosition[12] == 1.0f);
  assert(rect->vertexPosition[13] == -1.0f);
  assert(rect->vertexPosition[14] == 0.0f);

  assert(rect->vertexPosition[15] == 1.0f);
  assert(rect->vertexPosition[16] == 1.0f);
  assert(rect->vertexPosition[17] == 0.0f);

  rectangleFree(rect);
}

// void rectangleMoveTo(struct Rectangle *rect, GLfloat x, GLfloat y);

static void test_moveTo(void **state) {
  struct Rectangle *rect = rectangleNew();
  double moveX = 0.7f;
  double moveY = 0.3f;
  rectangleMoveTo(rect, moveX, moveY);

  assert(rect->position[rect->pivot][0] == 0.7f);
  assert(rect->position[rect->pivot][1] == 0.3f);
  assert(rect->position[rect->pivot][2] == 0.0f);

  assert(rect->position[0][0] == 0.7f);
  assert(rect->position[0][1] == 0.3f);
  assert(rect->position[0][2] == 0.0f);

  assert(rect->position[1][0] == -1.3f);
  assert(rect->position[1][1] == 0.3f);
  assert(rect->position[1][2] == 0.0f);

  assert(rect->position[2][0] == -1.3f);
  assert(rect->position[2][1] == -1.7f);
  assert(rect->position[2][2] == 0.0f);

  assert(rect->position[3][0] == 0.7f);
  assert(rect->position[3][1] == -1.7f);
  assert(rect->position[3][2] == 0.0f);

  assert(rect->vertexPosition[0] == 0.7f);
  assert(rect->vertexPosition[1] == 0.3f);
  assert(rect->vertexPosition[2] == 0.0f);

  assert(rect->vertexPosition[3] == -1.3f);
  assert(rect->vertexPosition[4] == 0.3f);
  assert(rect->vertexPosition[5] == 0.0f);

  assert(rect->vertexPosition[6] == -1.3f);
  assert(rect->vertexPosition[7] == -1.7f);
  assert(rect->vertexPosition[8] == 0.0f);

  assert(rect->vertexPosition[9] == -1.3f);
  assert(rect->vertexPosition[10] == -1.7f);
  assert(rect->vertexPosition[11] == 0.0f);

  assert(rect->vertexPosition[12] == 0.7f);
  assert(rect->vertexPosition[13] == -1.7f);
  assert(rect->vertexPosition[14] == 0.0f);

  assert(rect->vertexPosition[15] == 0.7f);
  assert(rect->vertexPosition[16] == 0.3f);
  assert(rect->vertexPosition[17] == 0.0f);

  rectangleFree(rect);

  rect = rectangleNew();
  moveX = -0.7f;
  moveY = -0.3f;
  rectangleMoveTo(rect, moveX, moveY);

  assert(rect->position[rect->pivot][0] == moveX);
  assert(rect->position[rect->pivot][1] == moveY);
  assert(rect->position[rect->pivot][2] == 0.0f);

  assert(moveX == rect->position[0][0]);
  assert(moveY == rect->position[0][1]);
  assert(rect->position[0][2] == 0.0f);

  assert(-2.7f == rect->position[1][0]);
  assert(-0.3f == rect->position[1][1]);
  assert(rect->position[1][2] == 0.0f);

  assert(-2.7f == rect->position[2][0]);
  assert(-2.3f == rect->position[2][1]);
  assert(rect->position[2][2] == 0.0f);

  assert(-0.7f == rect->position[3][0]);
  assert(-2.3f == rect->position[3][1]);
  assert(rect->position[3][2] == 0.0f);

  assert(-0.7f == rect->vertexPosition[0]);
  assert(-0.3f == rect->vertexPosition[1]);
  assert(rect->vertexPosition[2] == 0.0f);

  assert(-2.7f == rect->vertexPosition[3]);
  assert(-0.3f == rect->vertexPosition[4]);
  assert(rect->vertexPosition[5] == 0.0f);

  assert(-2.7f == rect->vertexPosition[6]);
  assert(-2.3f == rect->vertexPosition[7]);
  assert(rect->vertexPosition[8] == 0.0f);

  assert(-2.7f == rect->vertexPosition[9]);
  assert(-2.3f == rect->vertexPosition[10]);
  assert(rect->vertexPosition[11] == 0.0f);

  assert(-0.7f == rect->vertexPosition[12]);
  assert(-2.3f == rect->vertexPosition[13]);
  assert(rect->vertexPosition[14] == 0.0f);

  assert(-0.7f == rect->vertexPosition[15]);
  assert(-0.3f == rect->vertexPosition[16]);
  assert(rect->vertexPosition[17] == 0.0f);

  rectangleFree(rect);
}

// void rectangleSetWidth(struct Rectangle *rect, double width);
// void rectangleInitUVProject(struct Rectangle *rect);

static void test_setWidth(void **state) {
  struct Rectangle *rect = rectangleNew();
  double width = 0.5f;
  rectangleSetWidth(rect, width);

  assert(rect->position[0][0] == 1.0f);
  assert(rect->position[0][1] == 1.0f);
  assert(rect->position[0][2] == 0.0f);

  assert(rect->position[1][0] == 0.5f);
  assert(rect->position[1][1] == 1.0f);
  assert(rect->position[1][2] == 0.0f);

  assert(rect->position[2][0] == 0.5f);
  assert(rect->position[2][1] == -1.0f);
  assert(rect->position[2][2] == 0.0f);

  assert(rect->position[3][0] == 1.0f);
  assert(rect->position[3][1] == -1.0f);
  assert(rect->position[3][2] == 0.0f);

  assert(rect->uv[0][0] == 0.25f);
  assert(rect->uv[0][1] == 1.0f);

  assert(rect->uv[1][0] == 0.0f);
  assert(rect->uv[1][1] == 1.0f);

  assert(rect->uv[2][0] == 0.0f);
  assert(rect->uv[2][1] == 0.0f);

  assert(rect->uv[3][0] == 0.25f);
  assert(rect->uv[3][1] == 0.0f);

  assert(rect->vertexUV[0] == 0.25f);
  assert(rect->vertexUV[1] == 1.0f);

  assert(rect->vertexUV[2] == 0.0f);
  assert(rect->vertexUV[3] == 1.0f);

  assert(rect->vertexUV[4] == 0.0f);
  assert(rect->vertexUV[5] == 0.0f);

  assert(rect->vertexUV[6] == 0.0f);
  assert(rect->vertexUV[7] == 0.0f);

  assert(rect->vertexUV[8] == 0.25);
  assert(rect->vertexUV[9] == 0.0f);

  assert(rect->vertexUV[10] == 0.25f);
  assert(rect->vertexUV[11] == 1.0f);

  assert(rect->width == width);

  rectangleFree(rect);

  rect = rectangleNew();
  width = 0.0f;
  expect_assert_failure(rectangleSetWidth(rect, width));
  rectangleFree(rect);
}

// void rectangleSetHeight(struct Rectangle *rect, double height);

static void test_setHeight(void **state) {
  struct Rectangle *rect = rectangleNew();
  double height = 0.5f;
  rectangleSetHeight(rect, height);

  assert(rect->position[0][0] == 1.0f);
  assert(rect->position[0][1] == 1.0f);
  assert(rect->position[0][2] == 0.0f);

  assert(rect->position[1][0] == -1.0f);
  assert(rect->position[1][1] == 1.0f);
  assert(rect->position[1][2] == 0.0f);

  assert(rect->position[2][0] == -1.0f);
  assert(rect->position[2][1] == 0.5f);
  assert(rect->position[2][2] == 0.0f);

  assert(rect->position[3][0] == 1.0f);
  assert(rect->position[3][1] == 0.5f);
  assert(rect->position[3][2] == 0.0f);

  assert(rect->uv[0][0] == 1.0f);
  assert(rect->uv[0][1] == 0.25f);

  assert(rect->uv[1][0] == 0.0f);
  assert(rect->uv[1][1] == 0.25f);

  assert(rect->uv[2][0] == 0.0f);
  assert(rect->uv[2][1] == 0.0f);

  assert(rect->uv[3][0] == 1.0f);
  assert(rect->uv[3][1] == 0.0f);

  assert(rect->vertexUV[0] == 1.0f);
  assert(rect->vertexUV[1] == 0.25f);

  assert(rect->vertexUV[2] == 0.0f);
  assert(rect->vertexUV[3] == 0.25f);

  assert(rect->vertexUV[4] == 0.0f);
  assert(rect->vertexUV[5] == 0.0f);

  assert(rect->vertexUV[6] == 0.0f);
  assert(rect->vertexUV[7] == 0.0f);

  assert(rect->vertexUV[8] == 1.0f);
  assert(rect->vertexUV[9] == 0.0f);

  assert(rect->vertexUV[10] == 1.0f);
  assert(rect->vertexUV[11] == 0.25f);

  assert(rect->height == height);

  rectangleFree(rect);

  rect = rectangleNew();
  height = 0.0f;
  expect_assert_failure(rectangleSetHeight(rect, height));
  rectangleFree(rect);
}

// void rectangleMoveUV(struct Rectangle *rect, GLfloat x, GLfloat y);

static void test_moveUV(void **state) {
  struct Rectangle *rect = rectangleNew();
  double moveX = 0.7f;
  double moveY = 0.3f;

  rectangleMoveUV(rect, moveX, moveY);

  assert(rect->uv[0][0] == 1.7f);
  assert(rect->uv[0][1] == 1.3f);

  assert(rect->uv[1][0] == 0.7f);
  assert(rect->uv[1][1] == 1.3f);

  assert(rect->uv[2][0] == 0.7f);
  assert(rect->uv[2][1] == 0.3f);

  assert(rect->uv[3][0] == 1.7f);
  assert(rect->uv[3][1] == 0.3f);

  assert(rect->vertexUV[0] == 1.7f);
  assert(rect->vertexUV[1] == 1.3f);

  assert(rect->vertexUV[2] == 0.7f);
  assert(rect->vertexUV[3] == 1.3f);

  assert(rect->vertexUV[4] == 0.7f);
  assert(rect->vertexUV[5] == 0.3f);

  assert(rect->vertexUV[6] == 0.7f);
  assert(rect->vertexUV[7] == 0.3f);

  assert(rect->vertexUV[8] == 1.7f);
  assert(rect->vertexUV[9] == 0.3f);

  assert(rect->vertexUV[10] == 1.7f);
  assert(rect->vertexUV[11] == 1.3f);

  rectangleFree(rect);

  rect = rectangleNew();
  moveX = -0.7f;
  moveY = -0.3f;
  rectangleMoveUV(rect, moveX, moveY);

  assert(rect->uv[0][0] == 0.3f);
  assert(rect->uv[0][1] == 0.7f);

  assert(rect->uv[1][0] == -0.7f);
  assert(rect->uv[1][1] == 0.7f);

  assert(rect->uv[2][0] == -0.7f);
  assert(rect->uv[2][1] == -0.3f);

  assert(rect->uv[3][0] == 0.3f);
  assert(rect->uv[3][1] == -0.3f);

  rectangleFree(rect);

  rect = rectangleNew();
  moveX = 0.0f;
  moveY = 0.0f;
  rectangleMoveUV(rect, moveX, moveY);

  assert(rect->uv[0][0] == 1.0f);
  assert(rect->uv[0][1] == 1.0f);

  assert(rect->uv[1][0] == 0.0f);
  assert(rect->uv[1][1] == 1.0f);

  assert(rect->uv[2][0] == 0.0f);
  assert(rect->uv[2][1] == 0.0f);

  assert(rect->uv[3][0] == 1.0f);
  assert(rect->uv[3][1] == 0.0f);

  rectangleFree(rect);
}

// void rectangleScaleUV(struct Rectangle *rect, double scaleFactor);

//static void test_scaleUV(void **state) {
//  struct Rectangle *rect = rectangleNew();
//  double scaleFactor = 2.0f;
//
//  rectangleScaleUV(rect, scaleFactor);
//
//  assert(rect->uv[0][0] == 1.0f);
//  assert(rect->uv[0][1] == 1.0f);
//
//  assert(rect->uv[1][0] == 0.0f);
//  assert(rect->uv[1][1] == 1.0f);
//
//  assert(rect->uv[2][0] == 0.0f);
//  assert(rect->uv[2][1] == 0.0f);
//
//  assert(rect->uv[3][0] == 1.0f);
//  assert(rect->uv[3][1] == 0.0f);
//
//  assert(rect->vertexUV[0] == 2.0f);
//  assert(rect->vertexUV[1] == 2.0f);
//
//  assert(rect->vertexUV[2] == 0.0f);
//  assert(rect->vertexUV[3] == 2.0f);
//
//  assert(rect->vertexUV[4] == 0.0f);
//  assert(rect->vertexUV[5] == 0.0f);
//
//  assert(rect->vertexUV[6] == 0.0f);
//  assert(rect->vertexUV[7] == 0.0f);
//
//  assert(rect->vertexUV[8] == 2.0f);
//  assert(rect->vertexUV[9] == 0.0f);
//
//  assert(rect->vertexUV[10] == 2.0f);
//  assert(rect->vertexUV[11] == 2.0f);
//
//  rectangleFree(rect);
//
//  rect = rectangleNew();
//  scaleFactor = 0.5f;
//  rectangleScaleUV(rect, scaleFactor);
//
//  assert(rect->uv[0][0] == 1.0f);
//  assert(rect->uv[0][1] == 1.0f);
//
//  assert(rect->uv[1][0] == 0.0f);
//  assert(rect->uv[1][1] == 1.0f);
//
//  assert(rect->uv[2][0] == 0.0f);
//  assert(rect->uv[2][1] == 0.0f);
//
//  assert(rect->uv[3][0] == 1.0f);
//  assert(rect->uv[3][1] == 0.0f);
//
//  assert(rect->vertexUV[0] == 0.5f);
//  assert(rect->vertexUV[1] == 0.5f);
//
//  assert(rect->vertexUV[2] == 0.0f);
//  assert(rect->vertexUV[3] == 0.5f);
//
//  assert(rect->vertexUV[4] == 0.0f);
//  assert(rect->vertexUV[5] == 0.0f);
//
//  assert(rect->vertexUV[6] == 0.0f);
//  assert(rect->vertexUV[7] == 0.0f);
//
//  assert(rect->vertexUV[8] == 0.5f);
//  assert(rect->vertexUV[9] == 0.0f);
//
//  assert(rect->vertexUV[10] == 0.5f);
//  assert(rect->vertexUV[11] == 0.5f);
//
//  rectangleFree(rect);
//
//  rect = rectangleNew();
//  scaleFactor = 0.0f;
//  expect_assert_failure(rectangleScaleUV(rect, scaleFactor));
//
//  rectangleFree(rect);
//}

// void rectangleMovePoint(struct Rectangle *rect, unsigned int pointNumber,
//                        GLfloat x, GLfloat y);

static void test_movePoint(void **state) {
  struct Rectangle *rect = rectangleNew();
  double moveX = 0.5f;
  double moveY = 0.5f;

  rectangleMovePoint(rect, 0, moveX, moveY);

  //  printf("\n\n\n");
  //  printf("xmax: %d\n", rect->xMax);
  //  printf("ymax: %d\n", rect->yMax);
  //  printf("xmin: %d\n", rect->xMin);
  //  printf("ymin: %d\n", rect->yMin);
  //  printf("\n\n\n");

  assert(rect->position[0][0] == 1.5f);
  assert(rect->position[0][1] == 1.5f);
  assert(rect->position[0][2] == 0.0f);

  assert(rect->xMax == 0);
  assert(rect->yMax == 0);
  assert(rect->xMin == 1 || rect->xMin == 2);
  assert(rect->yMin == 2 || rect->yMin == 3);

  rectangleMovePoint(rect, 2, moveX, moveY);

  //  printf("\n\n\n");
  //  printf("xmax: %d\n", rect->xMax);
  //  printf("ymax: %d\n", rect->yMax);
  //  printf("xmin: %d\n", rect->xMin);
  //  printf("ymin: %d\n", rect->yMin);
  //  printf("\n\n\n");

  assert(rect->position[2][0] == -0.5f);
  assert(rect->position[2][1] == -0.5f);
  assert(rect->position[2][2] == 0.0f);

  assert(rect->xMax == 0);
  assert(rect->yMax == 0);
  assert(rect->xMin == 1);
  assert(rect->yMin == 3);

  rectangleMovePoint(rect, 3, moveX, 0.6);

  //  printf("\n\n\n");
  //  printf("xmax: %d\n", rect->xMax);
  //  printf("ymax: %d\n", rect->yMax);
  //  printf("xmin: %d\n", rect->xMin);
  //  printf("ymin: %d\n", rect->yMin);
  //  printf("\n\n\n");

  assert(rect->position[3][0] == 1.5f);
  assert(rect->position[3][1] == -0.4f);
  assert(rect->position[3][2] == 0.0f);

  assert(rect->xMax == 0 || rect->xMax == 3);
  assert(rect->yMax == 0);
  assert(rect->xMin == 1);
  assert(rect->yMin == 2);

  rectangleFree(rect);
}

// void rectangleMoveEdge(struct Rectangle *rect,
//                       unsigned int pointNumber01,
//                       unsigned int pointNumber02, GLfloat x, GLfloat y);

static void test_moveEdge(void **state) {
  struct Rectangle *rect = rectangleNew();
  double moveX = 0.5f;
  double moveY = 0.5f;

  rectangleMoveEdge(rect, 0, 3, moveX, moveY);

  //  printf("\n\n\n");
  //  printf("xmax: %d\n", rect->xMax);
  //  printf("ymax: %d\n", rect->yMax);
  //  printf("xmin: %d\n", rect->xMin);
  //  printf("ymin: %d\n", rect->yMin);
  //  printf("\n\n\n");

  assert(rect->position[0][0] == 1.5f);
  assert(rect->position[0][1] == 1.5f);
  assert(rect->position[0][2] == 0.0f);

  assert(rect->position[3][0] == 1.5f);
  assert(rect->position[3][1] == -0.5f);
  assert(rect->position[3][2] == 0.0f);

  assert(rect->xMax == 0 || rect->xMax == 3);
  assert(rect->yMax == 0);
  assert(rect->xMin == 1 || rect->xMin == 2);
  assert(rect->yMin == 2);

  rectangleMoveEdge(rect, 1, 2, 0.0f, -moveY);

  //  //  printf("\n\n\n");
  //  //  printf("xmax: %d\n", rect->xMax);
  //  //  printf("ymax: %d\n", rect->yMax);
  //  //  printf("xmin: %d\n", rect->xMin);
  //  //  printf("ymin: %d\n", rect->yMin);
  //  //  printf("\n\n\n");

  assert(rect->position[1][0] == -1.0f);
  assert(rect->position[1][1] == 0.5f);
  assert(rect->position[1][2] == 0.0f);

  assert(rect->position[2][0] == -1.0f);
  assert(rect->position[2][1] == -1.5f);
  assert(rect->position[2][2] == 0.0f);

  assert(rect->xMax == 0 || rect->xMax == 3);
  assert(rect->yMax == 0);
  assert(rect->xMin == 1 || rect->xMin == 2);
  assert(rect->yMin == 2);

  rectangleMoveEdge(rect, 0, 1, moveX, 0.0f);

  assert(rect->position[0][0] == 2.0f);
  assert(rect->position[0][1] == 1.5f);
  assert(rect->position[0][2] == 0.0f);

  assert(rect->position[1][0] == -0.5f);
  assert(rect->position[1][1] == 0.5f);
  assert(rect->position[1][2] == 0.0f);

  assert(rect->xMax == 0);
  assert(rect->yMax == 0);
  assert(rect->xMin == 2);
  assert(rect->yMin == 2);

  //  //  printf("\n\n\n");
  //  //  printf("xmax: %d\n", rect->xMax);
  //  //  printf("ymax: %d\n", rect->yMax);
  //  //  printf("xmin: %d\n", rect->xMin);
  //  //  printf("ymin: %d\n", rect->yMin);
  //  //  printf("\n\n\n");

  rectangleFree(rect);
}

int main(int argc, char **argv) {
  const UnitTest tests[] = {
      unit_test(test_memoryLeakAndInit),
      unit_test(test_setColorValue),
      unit_test(test_move),
      unit_test(test_moveUV),
      unit_test(test_moveTo),
      unit_test(test_setWidth),
      unit_test(test_setHeight),
      //unit_test(test_scaleUV),
      unit_test(test_movePoint),
      unit_test(test_moveEdge),

  };

  return run_tests(tests, "rectangle");
}

// struct Rectangle *rectangleClone(struct Rectangle *rect);
// void rectangleRotateUV(struct Rectangle *rect, float degree);
