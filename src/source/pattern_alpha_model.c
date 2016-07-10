#include "../header/pattern_alpha_model.h"

#define ONE_DEG_IN_RAD (2.0f * M_PI) / 360.0f

static inline void genRectRandUV(struct Rectangle *rect) {
  double amountX = (GLfloat)rand() / (GLfloat)RAND_MAX;
  double amountY = (GLfloat)rand() / (GLfloat)RAND_MAX;

  amountX = (amountX * 2.0f) - 1.0f;
  amountY = (amountY * 2.0f) - 1.0f;

  rectangleInitUVScale(rect);
  rectangleMoveUV(rect, amountX, amountY);
}

static inline void genRectRandColor(struct Rectangle *rect) {
  double color = (GLfloat)rand() / (GLfloat)RAND_MAX;
  rectangleSetColorValue(rect, color);
}

static inline void genRectRandRotateUV(struct Rectangle *rect) {
  double randNum = (GLfloat)rand() / (GLfloat)RAND_MAX;
  double rotateFactor = ((randNum * 2) - 1) * 360;

  rectangleRotateUV(rect, rotateFactor);
}

static void patternModelInitUV(struct PatternModel *pattern) {
  for (int i = 0; i < pattern->numUnits; i++) {
    struct Rectangle *rect = pattern->units[i];

    int vertexUVDataCounts = pattern->units[i]->vertexCounts * 2;
    for (int j = 0; j < vertexUVDataCounts; j++) {
      pattern->vertexUV[(i * vertexUVDataCounts) + j] = rect->vertexUV[j];
    }
  }
}

void patternModelRandomizeUV(struct PatternModel *pattern) {
  for (int i = 0; i < pattern->numUnits; i++) {
    genRectRandUV(pattern->units[i]);
  }

  patternModelInitUV(pattern);
}

void patternModelScaleUV(struct PatternModel *pattern, double scaleFactor) {
  for (int i = 0; i < pattern->numUnits; i++) {
    rectangleScaleUV(pattern->units[i], scaleFactor);
  }

  patternModelInitUV(pattern);
}

static void patternModelInitPos(struct PatternModel *pattern) {
  for (int i = 0; i < pattern->numUnits; i++) {
    int vertexVectorDataCounts = pattern->units[i]->vertexCounts * 3;
    struct Rectangle *rect = pattern->units[i];

    for (int j = 0; j < vertexVectorDataCounts; j++) {
      pattern->vertexPosition[(i * vertexVectorDataCounts) + j] =
          rect->vertexPosition[j];
    }

    for (int j = 0; j < 24; j++) {
      pattern->vertexWireframe[(i * 24) + j] = rect->vertexWireframe[j];
    }
  }
}

static void patternModelInitColor(struct PatternModel *pattern) {
  for (int i = 0; i < pattern->numUnits; i++) {
    int vertexVectorDataCounts = pattern->units[i]->vertexCounts * 3;
    struct Rectangle *rect = pattern->units[i];

    for (int j = 0; j < vertexVectorDataCounts; j++) {
      pattern->vertexColor[(i * vertexVectorDataCounts) + j] =
          rect->vertexColor[j];
    }
  }
}

void patternModelFitColor(struct PatternModel *pattern, float min, float max) {
  for (int i = 0; i < pattern->numUnits; i++) {
    int vertexDataCounts = pattern->units[i]->vertexCounts * 3;
    struct Rectangle *rect = pattern->units[i];

    for (int j = 0; j < vertexDataCounts; j++) {
      pattern->vertexColor[(i * vertexDataCounts) + j] =
          fit01(rect->vertexColor[j], min, max);
    }
  }

  // for the sake of function encapsulation and independency, I think that it is
  // better to call the setVBOData explicitly rather than encapsulating the
  // operation here
  //
  // setVBOData(&pattern->colorVBO, pattern->vertexCounts, 3,
  // pattern->vertexColor);
}

void patternModelRandomizeColor(struct PatternModel *pattern) {
  for (int i = 0; i < pattern->numUnits; i++) {
    genRectRandColor(pattern->units[i]);
  }
}

void patternModelRandomizeUVRotate(struct PatternModel *pattern) {
  for (int i = 0; i < pattern->numUnits; i++) {
    genRectRandRotateUV(pattern->units[i]);
  }

  //  float rotateRad = rotateAmount * ONE_DEG_IN_RAD;
  //
  //  for (int i = 0; i < pattern->vertexCounts * 2; i++) {
  //    if (i % 2 == 0) {
  //      float oldX = pattern->vertexUV[i];
  //      float oldY = pattern->vertexUV[i + 1];
  //
  //      // rect->uv[i][0] = (oldX * cos(rotateRad)) - (oldY *
  // sin(rotateRad));
  //      pattern->vertexUV[i] = (oldX * cos(rotateRad)) - (oldY *
  //      sin(rotateRad));
  //
  //      pattern->vertexUV[i + 1] =
  //          (oldX * cos(rotateRad)) - (oldY * sin(rotateRad));
  //    }
  //    // rect->uv[i][1] = (oldY * cos(rotateRad)) + (oldX *
  // sin(rotateRad));
  //  }

  patternModelInitUV(pattern);
}

void patternModelSeamlessModelConstruct(struct PatternModel *pattern,
                                        GtkGLArea *glArea) {
  if (pattern->seamlessModel != NULL) {
    patternModelFree(pattern->seamlessModel);
    pattern->seamlessModel = NULL;
  }

  pattern->numUnits = pattern->numWidth * pattern->numHeight;

  while (1) {
    int reuse = 0;
    for (int i = 0; i < pattern->numUnits; i++) {
      struct Rectangle *rect = pattern->units[i];

      // reuse original unit
      if (rect->position[rect->xMin[0]][0] >= 1.0f) {
        rectangleMove(rect, -__GL_VIEWPORT, 0);
      } else if (rect->position[rect->xMax[0]][0] <= -1.0f) {
        rectangleMove(rect, __GL_VIEWPORT, 0);
      } else if (rect->position[rect->yMin[0]][1] >= 1.0f) {
        rectangleMove(rect, 0, -__GL_VIEWPORT);
      } else if (rect->position[rect->yMax[0]][1] <= -1.0f) {
        rectangleMove(rect, 0, __GL_VIEWPORT);
      } else {
        continue;
      }

      reuse++;
    }

    // g_print("re use : %d\n", reuse);
    if (reuse == 0) {
      break;
    }
  }

  patternModelInitPos(pattern);

  unsigned int numNew = 0;

  for (int i = 0; i < pattern->numUnits; i++) {
    struct Rectangle *rect = pattern->units[i];

    if (rect->position[rect->xMax[0]][0] > 1.0f ||
        rect->position[rect->xMin[0]][0] < -1.0f ||
        rect->position[rect->yMax[0]][1] > 1.0f ||
        rect->position[rect->yMin[0]][1] < -1.0f) {
      numNew++;
    }
  }

  if (numNew != 0) {
    // g_print("num new: %d\n", numNew);
    struct PatternModel *re = defenseMalloc(sizeof(struct PatternModel));

    re->sizeX = pattern->sizeX;
    re->sizeY = pattern->sizeY;

    re->numWidth = pattern->numWidth;
    re->numHeight = pattern->numHeight;

    GLfloat width = (GLfloat)__GL_VIEWPORT / (GLfloat)re->numWidth;
    GLfloat height = (GLfloat)__GL_VIEWPORT / (GLfloat)re->numHeight;

    re->numUnits = numNew;
    re->units = defenseMalloc(numNew * sizeof(struct Rectangle *));

    unsigned int newIndex = 0;
    for (int i = 0; i < pattern->numUnits; i++) {
      struct Rectangle *rect = pattern->units[i];
      struct Rectangle *newRect = NULL;

      if (rect->position[rect->xMax[0]][0] > 1.0f) {
        newRect = rectangleClone(rect);
        rectangleMove(newRect, -__GL_VIEWPORT, 0);
      } else if (rect->position[rect->xMin[0]][0] < -1.0f) {
        newRect = rectangleClone(rect);
        rectangleMove(newRect, __GL_VIEWPORT, 0);
      } else if (rect->position[rect->yMax[0]][1] > 1.0f) {
        newRect = rectangleClone(rect);
        rectangleMove(newRect, 0, -__GL_VIEWPORT);
      } else if (rect->position[rect->yMin[0]][1] < -1.0f) {
        newRect = rectangleClone(rect);
        rectangleMove(newRect, 0, __GL_VIEWPORT);
      }

      if (newRect) {
        re->units[newIndex] = newRect;
        newIndex++;
      }
    }

    re->vertexCounts = re->numUnits * (*re->units)->vertexCounts;
    re->wireframeVertexCounts =
        re->numUnits * (*re->units)->wireframeVertexCounts;

    re->vertexPosition = defenseMalloc(re->vertexCounts * 3 * sizeof(GLfloat));
    re->vertexUV = defenseMalloc(re->vertexCounts * 2 * sizeof(GLfloat));
    re->vertexColor = defenseMalloc(re->vertexCounts * 3 * sizeof(GLfloat));

    re->vertexWireframe =
        defenseMalloc(re->wireframeVertexCounts * 3 * sizeof(GLfloat));

    patternModelInitPos(re);
    patternModelInitColor(re);
    patternModelInitUV(re);

    gtk_gl_area_make_current(glArea);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    re->positionVBO =
        generateVBO(&vao, re->vertexCounts, 3, re->vertexPosition, 0);

    re->colorVBO = generateVBO(&vao, re->vertexCounts, 3, re->vertexColor, 1);

    re->uvVBO = generateVBO(&vao, re->vertexCounts, 2, re->vertexUV, 2);

    re->wireframeVBO =
        generateVBO(&vao, re->wireframeVertexCounts, 3, re->vertexWireframe, 3);

    re->vao = vao;

    re->seamlessModel = NULL;

    pattern->seamlessModel = re;
  }
}

// void patternModelSeamlessConstruct(struct PatternModel *pattern,
//                                   GtkGLArea *glArea) {
//  // pattern->numUnits = pattern->numWidth * pattern->numHeight;
//
//  //  for (int i = 0; i < pattern->numUnits; i++) {
//  //    struct Rectangle *rect = pattern->units[i];
//  //
//  //    // reuse original unit
//  //
//  //    if (rect->position[rect->xMin[0]][0] >= 1.0f) {
//  //      rectangleMove(rect, -__GL_VIEWPORT, 0);
//  //    }
//  //
//  //    if (rect->position[rect->xMax[0]][0] <= -1.0f) {
//  //      rectangleMove(rect, __GL_VIEWPORT, 0);
//  //    }
//  //
//  //    if (rect->position[rect->yMin[0]][1] >= 1.0f) {
//  //      rectangleMove(rect, 0, -__GL_VIEWPORT);
//  //    }
//  //
//  //    if (rect->position[rect->yMax[0]][1] <= -1.0f) {
//  //      rectangleMove(rect, 0, __GL_VIEWPORT);
//  //    }
//  //  }
//
//  // patternModelInitPos(pattern);
//  seamlessModelConstruct(pattern, glArea);
//}

void patternModelInitUnitsPosition(struct PatternModel *pattern) {
  double width = (GLfloat)__GL_VIEWPORT / pattern->numWidth;
  double height = (GLfloat)__GL_VIEWPORT / pattern->numHeight;

  for (int h = 0; h < pattern->numHeight; h++) {
    for (int w = 0; w < pattern->numWidth; w++) {
      struct Rectangle *rect = pattern->units[(h * pattern->numWidth) + w];
      // we already randomize the color within the Rectangle construction
      // no need to do it again here

      // up right corner is the pivot of the Rectangle
      // rectangleMove(rect, (w * -width), (h * -height));
      rectangleMoveTo(rect, (w * -width) + 1, (h * -height) + 1);
    }
  }

  patternModelInitPos(pattern);
}

static void modelGenerate(struct PatternModel *pattern,
                          const unsigned int numWidth,
                          const unsigned int numHeight) {
  pattern->numUnits = numWidth * numHeight;
  pattern->units =
      defenseMalloc(pattern->numUnits * sizeof(struct Rectangle *));

  double width = (GLfloat)__GL_VIEWPORT / (GLfloat)numWidth;
  double height = (GLfloat)__GL_VIEWPORT / (GLfloat)numHeight;

  for (int h = 0; h < numHeight; h++) {
    for (int w = 0; w < numWidth; w++) {
      struct Rectangle *rect = rectangleNew();
      // we already randomize the color within the Rectangle construction
      // no need to do it again here
      // setRectRandColor(rect, 1.0f, 1.0f);

      rectangleSetWidth(rect, width);
      rectangleSetHeight(rect, height);

      // up right corner is the pivot of the Rectangle
      rectangleMove(rect, (w * -width), (h * -height));

      pattern->units[(h * numWidth) + w] = rect;
    }
  }

  pattern->vertexCounts = pattern->numUnits * (*pattern->units)->vertexCounts;
  pattern->wireframeVertexCounts =
      pattern->numUnits * (*pattern->units)->wireframeVertexCounts;

  pattern->vertexPosition =
      defenseMalloc(pattern->vertexCounts * 3 * sizeof(GLfloat));
  pattern->vertexUV =
      defenseMalloc(pattern->vertexCounts * 2 * sizeof(GLfloat));
  pattern->vertexColor =
      defenseMalloc(pattern->vertexCounts * 3 * sizeof(GLfloat));

  pattern->vertexWireframe =
      defenseMalloc(pattern->wireframeVertexCounts * 3 * sizeof(GLfloat));

  patternModelInitPos(pattern);
  patternModelInitColor(pattern);
  patternModelInitUV(pattern);
}

struct PatternModel *patternModelNew(GtkGLArea *glArea,
                                     const unsigned int sizeX,
                                     const unsigned int sizeY,
                                     const unsigned int cpy) {
  struct PatternModel *pattern = defenseMalloc(sizeof(struct PatternModel));

  pattern->sizeX = sizeX;
  pattern->sizeY = sizeY;

  double gcd = euclidGCD(sizeX, sizeY);
  double lcm = gcd * (sizeX / gcd) * (sizeY / gcd);

  double numWidth = (lcm / sizeX) * cpy;
  double numHeight = (lcm / sizeY) * cpy;

  pattern->numWidth = numWidth;
  pattern->numHeight = numHeight;

  modelGenerate(pattern, numWidth, numHeight);

  gtk_gl_area_make_current(glArea);

  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  pattern->positionVBO =
      generateVBO(&vao, pattern->vertexCounts, 3, pattern->vertexPosition, 0);

  glBindVertexArray(vao);
  pattern->colorVBO =
      generateVBO(&vao, pattern->vertexCounts, 3, pattern->vertexColor, 1);

  pattern->uvVBO =
      generateVBO(&vao, pattern->vertexCounts, 2, pattern->vertexUV, 2);

  pattern->wireframeVBO = generateVBO(&vao, pattern->wireframeVertexCounts, 3,
                                      pattern->vertexWireframe, 3);

  pattern->vao = vao;

  pattern->seamlessModel = NULL;

  return pattern;
}

void patternModelFree(struct PatternModel *pattern) {
  for (int i = 0; i < pattern->numUnits; i++) {
    rectangleFree(pattern->units[i]);
  }
  free(pattern->units);

  // free gpu memories or the program will crash
  glDeleteBuffers(1, &pattern->positionVBO);
  glDeleteBuffers(1, &pattern->uvVBO);
  glDeleteBuffers(1, &pattern->colorVBO);
  glDeleteBuffers(1, &pattern->wireframeVBO);

  glDeleteVertexArrays(1, &pattern->vao);

  free(pattern->vertexPosition);
  free(pattern->vertexColor);
  free(pattern->vertexUV);

  free(pattern->vertexWireframe);

  if (pattern->seamlessModel) {
    patternModelFree(pattern->seamlessModel);
  }

  free(pattern);
  pattern = NULL;
}
