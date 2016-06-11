//#include <math.h>
#include "../header/rectangle.h"

#define ONE_DEG_IN_RAD (2.0f * M_PI) / 360.0f

// struct rectangle {
//  GLfloat **position;
//  GLfloat **uv;
//
//  GLfloat width;
//  GLfloat height;
//
//  unsigned int pivot;
//  unsigned int *xMax;
//  unsigned int *xMin;
//  unsigned int *yMax;
//  unsigned int *yMin;
//
//  unsigned int *vertexOrder;
//
//  GLfloat *vertexPosition;
//  GLfloat *vertexUV;
//  GLfloat *vertexColor;
//
//  int vertexCounts;
//
//  GLfloat randColor;
//};

static void constructRectangleVertexPos(struct Rectangle *rect) {
  for (int i = 0; i < rect->vertexCounts; i++) {
    for (int j = 0; j < 3; j++) {
      rect->vertexPosition[(i * 3) + j] =
          rect->position[rect->vertexOrder[i]][j];
    }
  }
}

static void constructRectangleVertexUV(struct Rectangle *rect) {
  for (int i = 0; i < rect->vertexCounts; i++) {
    for (int j = 0; j < 2; j++) {
      rect->vertexUV[(i * 2) + j] = rect->uv[rect->vertexOrder[i]][j];
    }
  }
}

static void constructRectangleVertexColor(struct Rectangle *rect) {
  for (int i = 0; i < 18; i++) {
    rect->vertexColor[i] = rect->color;
  }
}

// static float fit01(float src, double newMin, double newMax) {
//  // if (src > 1.0f) {
//  //  src = 1.0f;
//  //} else if (src < 0.0f) {
//  //  src = 0.0f;
//  //}
//
//  if (src > 1.0f) {
//    src = newMax;
//  } else if (src < 0.0f) {
//    src = newMin;
//  }
//
//  double newRange = newMax - newMin;
//  float re = (src * newRange) + newMin;
//
//  return re;
//}
//

struct Rectangle *rectangleNew() {
  struct Rectangle *re = defenseCalloc(1, sizeof(struct Rectangle));
  re->vertexCounts = 6;

  re->position = (GLfloat **)defenseCalloc(4, sizeof(GLfloat *));
  re->uv = (GLfloat **)defenseCalloc(4, sizeof(GLfloat *));

  re->width = 1.0f;
  re->height = 1.0f;

  // we want to modified the existing position data instead of allocating new
  // memory space
  // re->pivot = (GLfloat **)defenseCalloc(1, sizeof(GLfloat *));

  re->xMax = (unsigned int *)defenseCalloc(2, sizeof(unsigned int));
  re->xMin = (unsigned int *)defenseCalloc(2, sizeof(unsigned int));
  re->yMax = (unsigned int *)defenseCalloc(2, sizeof(unsigned int));
  re->yMin = (unsigned int *)defenseCalloc(2, sizeof(unsigned int));

  for (int i = 0; i < 4; i++) {
    re->position[i] = (GLfloat *)defenseCalloc(3, sizeof(GLfloat));
    re->uv[i] = (GLfloat *)defenseCalloc(2, sizeof(GLfloat));
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

  re->xMax[0] = 0;
  re->xMax[1] = 3;

  re->xMin[0] = 1;
  re->xMin[1] = 2;

  re->yMax[0] = 0;
  re->yMax[1] = 1;

  re->yMin[0] = 2;
  re->yMin[1] = 3;

  re->pivot = 0;

  re->vertexOrder =
      (unsigned int *)defenseCalloc(re->vertexCounts, sizeof(unsigned int));

  re->vertexOrder[0] = 0;
  re->vertexOrder[1] = 1;
  re->vertexOrder[2] = 2;

  re->vertexOrder[3] = 2;
  re->vertexOrder[4] = 3;
  re->vertexOrder[5] = 0;

  re->vertexPosition = (GLfloat *)defenseCalloc(18, sizeof(GLfloat));
  re->vertexUV = (GLfloat *)defenseCalloc(12, sizeof(GLfloat));
  re->vertexColor = (GLfloat *)defenseCalloc(18, sizeof(GLfloat));

  constructRectangleVertexPos(re);
  constructRectangleVertexUV(re);

  re->color = 1.0f;
  constructRectangleVertexColor(re);

  return re;
}

void rectangleMove(struct Rectangle *rect, GLfloat x, GLfloat y) {
  for (int i = 0; i < 4; i++) {
    rect->position[i][0] += x;
    rect->position[i][1] += y;
  }

  constructRectangleVertexPos(rect);
}

void rectangleMoveTo(struct Rectangle *rect, GLfloat x, GLfloat y) {
  GLfloat mX = x - rect->position[rect->pivot][0];
  GLfloat mY = y - rect->position[rect->pivot][1];
  rectangleMove(rect, mX, mY);
}

static void initRectUVScale(struct Rectangle *rect) {
  GLfloat xMax = rect->uv[rect->xMin[0]][0] + 1.0f;
  GLfloat yMax = rect->uv[rect->yMin[0]][1] + 1.0f;
  rect->uv[rect->yMax[0]][1] = yMax;
  rect->uv[rect->yMax[1]][1] = yMax;
  rect->uv[rect->xMax[0]][0] = xMax;
  rect->uv[rect->xMax[1]][0] = xMax;

  if (rect->width > rect->height) {
    GLfloat scaleFactor = rect->height / rect->width;

    GLfloat width = rect->uv[rect->xMax[0]][0] - rect->uv[rect->xMin[0]][0];
    scaleFactor = rect->uv[rect->yMin[0]][1] + (width * scaleFactor);

    rect->uv[rect->yMax[0]][1] = scaleFactor;
    rect->uv[rect->yMax[1]][1] = scaleFactor;
  } else if (rect->width < rect->height) {
    GLfloat scaleFactor = rect->width / rect->height;

    GLfloat height = rect->uv[rect->yMax[0]][1] - rect->uv[rect->yMin[0]][1];
    scaleFactor = rect->uv[rect->xMin[0]][0] + (height * scaleFactor);

    rect->uv[rect->xMax[0]][0] = scaleFactor;
    rect->uv[rect->xMax[1]][0] = scaleFactor;
  }
}

void rectangleSetWidth(struct Rectangle *rect, GLfloat width) {
  // explicitily performing the sequential operation rather than construct a
  // loop to prevent the overhead of loop construction in this simple case
  rect->position[rect->xMin[0]][0] = rect->position[rect->xMax[0]][0] - width;
  rect->position[rect->xMin[1]][0] = rect->position[rect->xMax[0]][0] - width;

  rect->width = width;

  initRectUVScale(rect);

  constructRectangleVertexPos(rect);
  constructRectangleVertexUV(rect);
}

void rectangleSetHeight(struct Rectangle *rect, GLfloat height) {
  // explicitily performing the sequential operation rather than construct a
  // loop to prevent the overhead of loop construction in this simple case

  rect->position[rect->yMin[0]][1] = rect->position[rect->yMax[0]][1] - height;
  rect->position[rect->yMin[1]][1] = rect->position[rect->yMax[0]][1] - height;

  rect->height = height;

  initRectUVScale(rect);

  constructRectangleVertexPos(rect);
  constructRectangleVertexUV(rect);
}

void rectangleSetColorValue(struct Rectangle *rect, GLfloat colorValue) {
  if (colorValue < 0.0f || colorValue > 1.0f) {
    printf("Invalid color value for rectangle: %f\n", colorValue);
    return;
  }
  rect->color = colorValue;

  constructRectangleVertexColor(rect);
}

void rectangleMoveUV(struct Rectangle *rect, GLfloat x, GLfloat y) {
  for (int i = 0; i < 4; i++) {
    rect->uv[i][0] += x;
    rect->uv[i][1] += y;
  }

  constructRectangleVertexUV(rect);
}

void rectangleScaleUV(struct Rectangle *rect, double scaleFactor) {
  initRectUVScale(rect);

  GLfloat width = rect->uv[rect->xMax[0]][0] - rect->uv[rect->xMin[0]][0];
  GLfloat height = rect->uv[rect->yMax[0]][1] - rect->uv[rect->yMin[0]][1];

  GLfloat widthScale = width * scaleFactor;
  GLfloat heightScale = height * scaleFactor;

  GLfloat xMax = rect->uv[rect->xMin[0]][0] + widthScale;
  GLfloat yMax = rect->uv[rect->yMin[0]][1] + heightScale;

  rect->uv[rect->xMax[0]][0] = xMax;
  rect->uv[rect->xMax[1]][0] = xMax;

  rect->uv[rect->yMax[0]][1] = yMax;
  rect->uv[rect->yMax[1]][1] = yMax;

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

  constructRectangleVertexUV(rect);
}

void rectangleRotateUV(struct Rectangle *rect, float degree) {
  float rotateRad = degree * ONE_DEG_IN_RAD;

  for (int i = 0; i < 4; i++) {
    float oldX = rect->uv[i][0];
    float oldY = rect->uv[i][1];

    rect->uv[i][0] = (oldX * cos(rotateRad)) - (oldY * sin(rotateRad));
    rect->uv[i][1] = (oldY * cos(rotateRad)) + (oldX * sin(rotateRad));
  }

  constructRectangleVertexUV(rect);
}

// static void genRectRandUV(struct rectangle *rect) {
//  GLfloat amountX = (GLfloat)rand() / (GLfloat)RAND_MAX;
//  GLfloat amountY = (GLfloat)rand() / (GLfloat)RAND_MAX;
//
//  amountX = (amountX * 2.0f) - 1.0f;
//  amountY = (amountY * 2.0f) - 1.0f;
//
//  moveRectUV(rect, amountX, amountY);
//}

void rectangleFree(struct Rectangle *re) {
  for (int i = 0; i < 4; i++) {
    free(re->position[i]);
    free(re->uv[i]);
  }
  free(re->position);
  free(re->uv);

  free(re->xMin);
  free(re->yMin);
  free(re->xMax);
  free(re->yMax);

  free(re->vertexOrder);
  free(re->vertexPosition);
  free(re->vertexUV);
  free(re->vertexColor);

  free(re);
}