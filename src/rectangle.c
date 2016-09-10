#include "rectangle.h"
#include <cmockery/pbc.h>

#ifdef UNIT_TESTING
#include <cmockery/cmockery_override.h>
#endif

static inline void constructRectangleWireframe(struct Rectangle *rect) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 2; j++) {
      for (int k = 0; k < 3; k++) {
        int positionIndex = (j + i) % 4;
        rect->vertexWireframe[(i * 2 * 3) + (j * 3) + k] =
            rect->position[positionIndex][k];
      }
    }
  }
}

static inline void constructRectangleVertexPos(struct Rectangle *rect) {
  for (int i = 0; i < rect->vertexCounts; i++) {
    for (int j = 0; j < 3; j++) {
      rect->vertexPosition[(i * 3) + j] =
          rect->position[rect->vertexOrder[i]][j];
    }
  }

  constructRectangleWireframe(rect);

  // for (int i = 0; i < 4; i++) {
  //  for (int j = 0; j < 2; j++) {
  //    for (int k = 0; k < 3; k++) {
  //      int positionIndex = (j + i) % 4;
  //      rect->vertexWireframe[(i * 2 * 3) + (j * 3) + k] =
  //          rect->position[positionIndex][k];
  //    }
  //  }
  //}
}

static inline void constructRectangleVertexUV(struct Rectangle *rect) {
  for (int i = 0; i < rect->vertexCounts; i++) {
    for (int j = 0; j < 2; j++) {
      rect->vertexUV[(i * 2) + j] = rect->uv[rect->vertexOrder[i]][j];
    }
  }
}

static inline void constructRectangleVertexColor(struct Rectangle *rect) {
  for (int i = 0; i < 18; i++) {
    rect->vertexColor[i] = rect->color;
  }
}

struct Rectangle *rectangleClone(struct Rectangle *rect) {
  struct Rectangle *re = rectangleNew();

  re->height = rect->height;
  re->width = rect->width;

  re->position[0][0] = rect->position[0][0];
  re->position[0][1] = rect->position[0][1];
  re->position[0][2] = rect->position[0][2];

  re->position[1][0] = rect->position[1][0];
  re->position[1][1] = rect->position[1][1];
  re->position[1][2] = rect->position[1][2];

  re->position[2][0] = rect->position[2][0];
  re->position[2][1] = rect->position[2][1];
  re->position[2][2] = rect->position[2][2];

  re->position[3][0] = rect->position[3][0];
  re->position[3][1] = rect->position[3][1];
  re->position[3][2] = rect->position[3][2];

  re->uv[0][0] = rect->uv[0][0];
  re->uv[0][1] = rect->uv[0][1];

  re->uv[1][0] = rect->uv[1][0];
  re->uv[1][1] = rect->uv[1][1];

  re->uv[2][0] = rect->uv[2][0];
  re->uv[2][1] = rect->uv[2][1];

  re->uv[3][0] = rect->uv[3][0];
  re->uv[3][1] = rect->uv[3][1];

  re->xMax = rect->xMax;
  // re->xMax = rect->xMax;

  re->xMin = rect->xMin;
  // re->xMin = rect->xMin;

  re->yMax = rect->yMax;
  // re->yMax = rect->xMax;

  re->yMin = rect->yMin;
  // re->yMin = rect->yMin;

  re->pivot = rect->pivot;
  //  re->pivot = (GLfloat *)defenseMalloc(3 * sizeof(GLfloat));
  //  re->pivot[0] = 0;
  //  re->pivot[1] = 0;
  //  re->pivot[2] = 0;

  memcpy(re->vertexOrder, rect->vertexOrder, sizeof(unsigned int) * 6);

  //  re->vertexOrder[0] = rect->vertexOrder[0];
  //  re->vertexOrder[1] = rect->vertexOrder[1];
  //  re->vertexOrder[2] = rect->vertexOrder[2];
  //
  //  re->vertexOrder[3] = rect->vertexOrder[3];
  //  re->vertexOrder[4] = rect->vertexOrder[4];
  //  re->vertexOrder[5] = rect->vertexOrder[5];

  re->color = rect->color;

  memcpy(re->vertexPosition, rect->vertexPosition,
         sizeof(GLfloat) * re->vertexCounts * 3);

  memcpy(re->vertexColor, rect->vertexColor,
         sizeof(GLfloat) * re->vertexCounts * 3);

  memcpy(re->vertexUV, rect->vertexUV, sizeof(GLfloat) * re->vertexCounts * 2);

  memcpy(re->vertexWireframe, rect->vertexWireframe,
         sizeof(GLfloat) * re->wireframeVertexCounts * 3);

  //  for (int i = 0; i < re->vertexCounts * 3; i++) {
  //    re->vertexPosition[i] = rect->vertexPosition[i];
  //    re->vertexColor[i] = rect->vertexColor[i];
  //  }

  //  for (int i = 0; i < re->vertexCounts * 2; i++) {
  //    re->vertexUV[i] = rect->vertexUV[i];
  //  }

  // for (int i = 0; i < re->wireframeVertexCounts * 3; i++) {
  //  re->vertexWireframe[i] = rect->vertexWireframe[i];
  //}

  // constructRectangleVertexPos(re);
  // constructRectangleVertexUV(re);
  // constructRectangleVertexColor(re);

  return re;
}

struct Rectangle *rectangleNew() {
  struct Rectangle *re =
      defenseMalloc(sizeof(struct Rectangle), mallocFailAbort, NULL);

  re->vertexCounts = 6;
  re->wireframeVertexCounts = 8;

  //  re->position =
  //      (GLfloat **)defenseMalloc(4 * sizeof(GLfloat *), mallocFailAbort,
  //      NULL);
  //  re->uv =
  //      (GLfloat **)defenseMalloc(4 * sizeof(GLfloat *), mallocFailAbort,
  //      NULL);

  re->position =
      (double **)defenseMalloc(4 * sizeof(double *), mallocFailAbort, NULL);
  re->uv =
      (double **)defenseMalloc(4 * sizeof(double *), mallocFailAbort, NULL);

  re->width = 1.0f;
  re->height = 1.0f;

  // re->rotateDegree = 0.0f;

  // we want to modified the existing position data instead of allocating new
  // memory space
  // re->pivot = (GLfloat **)defenseMalloc(sizeof(GLfloat *));

  //  re->xMax = (unsigned int *)defenseMalloc(2 * sizeof(unsigned int));
  //  re->xMin = (unsigned int *)defenseMalloc(2 * sizeof(unsigned int));
  //  re->yMax = (unsigned int *)defenseMalloc(2 * sizeof(unsigned int));
  //  re->yMin = (unsigned int *)defenseMalloc(2 * sizeof(unsigned int));

  for (int i = 0; i < 4; i++) {
    //  re->position[i] =
    //      (GLfloat *)defenseMalloc(3 * sizeof(GLfloat), mallocFailAbort,
    //      NULL);
    //  re->uv[i] =
    //      (GLfloat *)defenseMalloc(2 * sizeof(GLfloat), mallocFailAbort,
    //      NULL);

    re->position[i] =
        (double *)defenseMalloc(3 * sizeof(double), mallocFailAbort, NULL);
    re->uv[i] =
        (double *)defenseMalloc(2 * sizeof(double), mallocFailAbort, NULL);
  }

  re->position[0][0] = 1.0f;
  re->position[0][1] = 1.0f;
  re->position[0][2] = 0.0f;

  re->position[1][0] = -1.0f;
  re->position[1][1] = 1.0f;
  re->position[1][2] = 0.0f;

  re->position[2][0] = -1.0f;
  re->position[2][1] = -1.0f;
  re->position[2][2] = 0.0f;

  re->position[3][0] = 1.0f;
  re->position[3][1] = -1.0f;
  re->position[3][2] = 0.0f;

  re->uv[0][0] = 1.0f;
  re->uv[0][1] = 1.0f;

  re->uv[1][0] = 0.0f;
  re->uv[1][1] = 1.0f;

  re->uv[2][0] = 0.0f;
  re->uv[2][1] = 0.0f;

  re->uv[3][0] = 1.0f;
  re->uv[3][1] = 0.0f;

  re->xMax = 0;
  // re->xMax = 3;

  re->xMin = 1;
  // re->xMin = 2;

  re->yMax = 0;
  // re->yMax = 1;

  re->yMin = 2;
  // re->yMin = 3;

  re->pivot = 0;
  //  re->pivot = (GLfloat *)defenseMalloc(3 * sizeof(GLfloat));
  //  re->pivot[0] = 0;
  //  re->pivot[1] = 0;
  //  re->pivot[2] = 0;

  re->vertexOrder = (unsigned int *)defenseMalloc(
      re->vertexCounts * sizeof(unsigned int), mallocFailAbort, NULL);

  re->vertexOrder[0] = 0;
  re->vertexOrder[1] = 1;
  re->vertexOrder[2] = 2;

  re->vertexOrder[3] = 2;
  re->vertexOrder[4] = 3;
  re->vertexOrder[5] = 0;

  re->vertexPosition =
      (GLfloat *)defenseMalloc(18 * sizeof(GLfloat), mallocFailAbort, NULL);
  re->vertexUV =
      (GLfloat *)defenseMalloc(12 * sizeof(GLfloat), mallocFailAbort, NULL);
  re->vertexColor =
      (GLfloat *)defenseMalloc(18 * sizeof(GLfloat), mallocFailAbort, NULL);

  re->vertexWireframe =
      (GLfloat *)defenseMalloc(24 * sizeof(GLfloat), mallocFailAbort, NULL);

  constructRectangleVertexPos(re);
  constructRectangleVertexUV(re);

  re->color = 1.0f;
  constructRectangleVertexColor(re);

  return re;
}

void rectangleMove(struct Rectangle *rect, double x, double y) {
  for (int i = 0; i < 4; i++) {
    rect->position[i][0] += x;
    rect->position[i][1] += y;
  }

  constructRectangleVertexPos(rect);
}

void rectangleMoveTo(struct Rectangle *rect, double x, double y) {
  double mX = x - (double)rect->position[rect->pivot][0];
  double mY = y - (double)rect->position[rect->pivot][1];
  rectangleMove(rect, mX, mY);
}

static void checkMaxMinValue(struct Rectangle *rect) {
  rect->xMax = 0;
  rect->xMin = 0;
  rect->yMax = 0;
  rect->yMin = 0;

  for (int i = 1; i < 4; i++) {
    if (rect->position[i][0] > rect->position[rect->xMax][0]) {
      rect->xMax = i;
      goto CHECK_Y;
    }

    if (rect->position[i][0] < rect->position[rect->xMin][0]) {
      rect->xMin = i;
    }

  CHECK_Y:

    if (rect->position[i][1] > rect->position[rect->yMax][1]) {
      rect->yMax = i;
      continue;
    }

    if (rect->position[i][1] < rect->position[rect->yMin][1]) {
      rect->yMin = i;
    }
  }
}

void rectangleMovePoint(struct Rectangle *rect, unsigned int pointNumber,
                        double x, double y) {
  rect->position[pointNumber][0] += x;
  rect->position[pointNumber][1] += y;

  rectangleInitUVProject(rect);

  constructRectangleVertexPos(rect);
  constructRectangleVertexUV(rect);

  checkMaxMinValue(rect);
}

void rectangleMoveEdge(struct Rectangle *rect, unsigned int pointNumber01,
                       unsigned int pointNumber02, double x, double y) {
  rect->position[pointNumber01][0] += x;
  rect->position[pointNumber01][1] += y;

  rect->position[pointNumber02][0] += x;
  rect->position[pointNumber02][1] += y;

  rectangleInitUVProject(rect);

  constructRectangleVertexPos(rect);
  constructRectangleVertexUV(rect);

  checkMaxMinValue(rect);
}

void rectangleInitUVProject(struct Rectangle *rect) {
  double width = (double)rect->position[rect->xMax][0] -
                 (double)rect->position[rect->xMin][0];

  double height = (double)rect->position[rect->yMax][1] -
                  (double)rect->position[rect->yMin][1];

  double scaleFactor = width > height ? width : height;

  REQUIRE(scaleFactor > 0.0f);

  for (int i = 0; i < 4; i++) {
    rect->uv[i][0] =
        (rect->position[i][0] - rect->position[rect->xMin][0]) / scaleFactor;
    rect->uv[i][1] =
        (rect->position[i][1] - rect->position[rect->yMin][1]) / scaleFactor;
  }
}

// void rectangleInitUVScale(struct Rectangle *rect) {
//  rect->uv[0][0] = 1.0f;
//  rect->uv[0][1] = 1.0f;
//
//  rect->uv[1][0] = 0.0f;
//  rect->uv[1][1] = 1.0f;
//
//  rect->uv[2][0] = 0.0f;
//  rect->uv[2][1] = 0.0f;
//
//  rect->uv[3][0] = 1.0f;
//  rect->uv[3][1] = 0.0f;
//
//  if (rect->width > rect->height) {
//    GLfloat scaleFactor = rect->height / rect->width;
//
//    GLfloat width = rect->uv[rect->xMax][0] - rect->uv[rect->xMin][0];
//    scaleFactor = rect->uv[rect->yMin][1] + (width * scaleFactor);
//
//    rect->uv[0][1] = scaleFactor;
//    rect->uv[1][1] = scaleFactor;
//
//  } else if (rect->width < rect->height) {
//    GLfloat scaleFactor = rect->width / rect->height;
//
//    GLfloat height = rect->uv[rect->yMax][1] - rect->uv[rect->yMin][1];
//    scaleFactor = rect->uv[rect->xMin][0] + (height * scaleFactor);
//
//    rect->uv[0][0] = scaleFactor;
//    rect->uv[3][0] = scaleFactor;
//  }
//}

void rectangleSetWidth(struct Rectangle *rect, double width) {
  // explicitily performing the sequential operation rather than construct a
  // loop to prevent the overhead of loop construction in this simple case

  REQUIRE(width > 0.0f);

  rect->position[1][0] = (double)rect->position[0][0] - width;
  rect->position[2][0] = (double)rect->position[3][0] - width;

  rect->width = width;

  rectangleInitUVProject(rect);

  constructRectangleVertexPos(rect);
  constructRectangleVertexUV(rect);
}

void rectangleSetHeight(struct Rectangle *rect, double height) {
  // explicitily performing the sequential operation rather than construct a
  // loop to prevent the overhead of loop construction in this simple case

  REQUIRE(height > 0.0f);

  rect->position[2][1] = (double)rect->position[0][1] - height;
  rect->position[3][1] = (double)rect->position[1][1] - height;

  rect->height = height;

  rectangleInitUVProject(rect);

  constructRectangleVertexPos(rect);
  constructRectangleVertexUV(rect);
}

void rectangleSetColorValue(struct Rectangle *rect, double colorValue) {
  // if (colorValue < 0.0f || colorValue > 1.0f) {
  //  printf("Invalid color value for rectangle: %f\n", colorValue);
  //  return;
  //}

  REQUIRE(colorValue >= 0.0f || colorValue <= 1.0f);
  rect->color = colorValue;

  constructRectangleVertexColor(rect);
}

void rectangleMoveUV(struct Rectangle *rect, double x, double y) {
  for (int i = 0; i < 4; i++) {
    rect->uv[i][0] += x;
    rect->uv[i][1] += y;
  }

  constructRectangleVertexUV(rect);
}

void rectangleScaleUV(struct Rectangle *rect, double scaleFactor) {
  REQUIRE(scaleFactor > 0.0f);

  constructRectangleVertexUV(rect);

  for (int i = 0; i < rect->vertexCounts * 2; i++) {
    rect->vertexUV[i] *= scaleFactor;
  }

  ////////////////////////////////////////////////////////////

  // OpenGL does not work in the way of explicitily wrap the uv position
  // set the third argument of glTexParameteriv function to GL_REPEAT instead

  //  GLfloat width =
  //      rect->uv[rect->xMax[0]][0] > rect->uv[rect->xMin[0]][0]
  //          ? rect->uv[rect->xMax[0]][0] - rect->uv[rect->xMin[0]][0]
  //          : (1.0f - rect->uv[rect->xMin[0]][0]) +
  //          rect->uv[rect->xMax[0]][0];
  //
  //  GLfloat height =
  //      rect->uv[rect->yMax[0]][1] > rect->uv[rect->yMin[0]][1]
  //          ? rect->uv[rect->yMax[0]][1] - rect->uv[rect->yMin[0]][1]
  //          : (1.0f - rect->uv[rect->yMin[0]][1]) +
  //          rect->uv[rect->yMax[0]][1];
  //
  //  GLfloat widthScale = width * scaleFactor;
  //  GLfloat heightScale = height * scaleFactor;
  //
  //  GLfloat xMax = uvWrap(rect->uv[rect->xMin[0]][0] + widthScale);
  //  GLfloat yMax = uvWrap(rect->uv[rect->yMin[0]][1] + heightScale);

  // rect->uv[rect->xMax[0]][0] = xMax;
  // rect->uv[rect->xMax[1]][0] = xMax;

  // rect->uv[rect->yMax[0]][1] = yMax;
  // rect->uv[rect->yMax[1]][1] = yMax;

  // constructRectangleVertexUV(rect);
}

void rectangleRotateUV(struct Rectangle *rect, double degree) {
  // rect->rotateDegree = degree;
  double rotateRad = degree * ONE_DEG_IN_RAD;

  for (int i = 0; i < 4; i++) {
    double oldX = (double)rect->uv[i][0];
    double oldY = (double)rect->uv[i][1];

    rect->uv[i][0] = (oldX * cos(rotateRad)) - (oldY * sin(rotateRad));
    rect->uv[i][1] = (oldY * cos(rotateRad)) + (oldX * sin(rotateRad));
  }

  constructRectangleVertexUV(rect);
}

void rectangleFree(struct Rectangle *re) {
  for (int i = 0; i < 4; i++) {
    free(re->position[i]);
    free(re->uv[i]);
  }

  free(re->position);
  free(re->uv);

  // free(re->xMin);
  // free(re->yMin);
  // free(re->xMax);
  // free(re->yMax);

  free(re->vertexOrder);
  free(re->vertexPosition);
  free(re->vertexUV);
  free(re->vertexColor);

  free(re->vertexWireframe);

  free(re);
}
