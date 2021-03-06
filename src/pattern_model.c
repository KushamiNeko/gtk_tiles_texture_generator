#include "pattern_model.h"

//#include <cmockery/pbc.h>
#include "../../general/src/debug_macro.h"

#ifdef UNIT_TESTING
#include <cmockery/cmockery_override.h>
#endif

static inline void genRectRandUV(struct Rectangle *rect) {
  double amountX = (double)rand() / (double)RAND_MAX;
  double amountY = (double)rand() / (double)RAND_MAX;

  amountX = (amountX - 0.5f) * 2.0f;
  amountY = (amountY - 0.5f) * 2.0f;

  double randAmount = ((double)rand() / (double)RAND_MAX) * 10;

  rectangleMoveUV(rect, amountX * randAmount, amountY * randAmount);
}

static inline void genRectRandColor(struct Rectangle *rect) {
  double color = (double)rand() / (double)RAND_MAX;
  rectangleSetColorValue(rect, color);
}

static inline void genRectRandRotateUV(struct Rectangle *rect) {
  double randNum = (double)rand() / (double)RAND_MAX;
  double rotateFactor = ((randNum - 0.5f) * 2.0f) * 720.0f;

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

  REQUIRE(scaleFactor > 0.0f);

  patternModelInitUV(pattern);

  //  for (int i = 0; i < pattern->numUnits; i++) {
  //    int vertexUVDataCounts = pattern->units[i]->vertexCounts * 2;
  //    for (int j = 0; j < vertexUVDataCounts; j++) {
  //      pattern->vertexUV[(i * vertexUVDataCounts) + j] *= scaleFactor;
  //    }
  //  }
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

void patternModelFitColor(struct PatternModel *pattern, double min,
                          double max) {
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

  patternModelInitUV(pattern);
}

// static struct PatternModel *patternModelCloneData(
//    struct PatternModel *pattern) {
//  struct PatternModel *re =
//      DEFENSE_MALLOC(sizeof(struct PatternModel), mallocFailAbort, NULL);
//
//  re->sizeX = pattern->sizeX;
//  re->sizeY = pattern->sizeY;
//
//  // unsigned int sizeX;
//  // unsigned int sizeY;
//
//  re->numWidth = pattern->numWidth;
//  re->numHeight = pattern->numHeight;
//
//  //  unsigned int numWidth;
//  //  unsigned int numHeight;
//
//  // struct Rectangle **units;
//
//  re->numUnits = pattern->numUnits;
//  re->vertexCounts = pattern->vertexCounts;
//  re->wireframeVertexCounts = pattern->wireframeVertexCounts;
//
//  //  unsigned int numUnits;
//  //  unsigned int vertexCounts;
//  //  unsigned int wireframeVertexCounts;
//
//  re->vertexPosition = DEFENSE_MALLOC(re->vertexCounts * 3 * sizeof(GLfloat),
//                                     mallocFailAbort, NULL);
//  re->vertexUV = DEFENSE_MALLOC(re->vertexCounts * 2 * sizeof(GLfloat),
//                               mallocFailAbort, NULL);
//  re->vertexColor = DEFENSE_MALLOC(re->vertexCounts * 3 * sizeof(GLfloat),
//                                  mallocFailAbort, NULL);
//
//  re->vertexWireframe = DEFENSE_MALLOC(
//      re->wireframeVertexCounts * 3 * sizeof(GLfloat), mallocFailAbort, NULL);
//
//  memcpy(re->vertexPosition, pattern->vertexPosition,
//         re->vertexCounts * 3 * sizeof(GLfloat));
//
//  memcpy(re->vertexColor, pattern->vertexColor,
//         re->vertexCounts * 3 * sizeof(GLfloat));
//
//  memcpy(re->vertexUV, pattern->vertexUV,
//         re->vertexCounts * 2 * sizeof(GLfloat));
//
//  memcpy(re->vertexWireframe, pattern->vertexWireframe,
//         re->wireframeVertexCounts * 3 * sizeof(GLfloat));
//
//  // GLfloat *vertexPosition;
//  // GLfloat *vertexUV;
//  // GLfloat *vertexColor;
//
//  // GLfloat *vertexWireframe;
//
//  // GLuint vao;
//  // GLuint positionVBO;
//  // GLuint uvVBO;
//  // GLuint colorVBO;
//
//  // GLuint wireframeVBO;
//
//  re->seamlessModel = NULL;
//
//  return re;
//}

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
      if (rect->position[rect->xMin][0] >= 1.0f) {
        rectangleMove(rect, -GL_HELPER_VIEWPORT_SIZE, 0);
      } else if (rect->position[rect->xMax][0] <= -1.0f) {
        rectangleMove(rect, GL_HELPER_VIEWPORT_SIZE, 0);
      } else if (rect->position[rect->yMin][1] >= 1.0f) {
        rectangleMove(rect, 0, -GL_HELPER_VIEWPORT_SIZE);
      } else if (rect->position[rect->yMax][1] <= -1.0f) {
        rectangleMove(rect, 0, GL_HELPER_VIEWPORT_SIZE);
      } else {
        continue;
      }

      reuse++;
    }

    if (reuse == 0) {
      break;
    }
  }

  patternModelInitPos(pattern);

  //    gtk_gl_area_make_current(glArea);
  //
  //    GLuint vao = 0;
  //    glGenVertexArrays(1, &vao);
  //    glBindVertexArray(vao);
  //
  //    re->positionVBO =
  //        generateVBO(&vao, re->vertexCounts, 3, re->vertexPosition, 0);
  //
  //    re->colorVBO = generateVBO(&vao, re->vertexCounts, 3, re->vertexColor,
  //    1);
  //
  //    re->uvVBO = generateVBO(&vao, re->vertexCounts, 2, re->vertexUV, 2);
  //
  //    re->wireframeVBO =
  //        generateVBO(&vao, re->wireframeVertexCounts, 3, re->vertexWireframe,
  //        3);
  //
  //    re->vao = vao;

  unsigned int numNew = 0;

  for (int i = 0; i < pattern->numUnits; i++) {
    struct Rectangle *rect = pattern->units[i];

    //    if (rect->position[rect->xMax][0] > 1.0f ||
    //  rect->position[rect->xMin][0] < -1.0f ||
    //  rect->position[rect->yMax][1] > 1.0f ||
    //  rect->position[rect->yMin][1] < -1.0f) {
    // numNew++;
    //}

    if (rect->position[rect->xMax][0] > 1.0f) {
      numNew++;
    }

    if (rect->position[rect->xMin][0] < -1.0f) {
      numNew++;
    }

    if (rect->position[rect->yMax][1] > 1.0f) {
      numNew++;
    }

    if (rect->position[rect->yMin][1] < -1.0f) {
      numNew++;
    }
  }

  // g_print("num new: %d\n", numNew);

  if (numNew != 0) {
    struct PatternModel *re =
        DEFENSE_MALLOC(sizeof(struct PatternModel), mallocFailAbort, NULL);

    re->sizeX = pattern->sizeX;
    re->sizeY = pattern->sizeY;

    re->numWidth = pattern->numWidth;
    re->numHeight = pattern->numHeight;

    // double width = (double)GL_HELPER_VIEWPORT_SIZE / (double)re->numWidth;
    // double height = (double)GL_HELPER_VIEWPORT_SIZE / (double)re->numHeight;

    re->numUnits = numNew;
    re->units = DEFENSE_MALLOC(numNew * sizeof(struct Rectangle *),
                               mallocFailAbort, NULL);

    unsigned int newIndex = 0;
    for (int i = 0; i < pattern->numUnits; i++) {
      struct Rectangle *rect = pattern->units[i];
      struct Rectangle *newRect = NULL;

      if (rect->position[rect->xMax][0] > 1.0f) {
        newRect = rectangleClone(rect);
        rectangleMove(newRect, -GL_HELPER_VIEWPORT_SIZE, 0);

        re->units[newIndex] = newRect;
        newIndex++;
      }

      if (rect->position[rect->xMin][0] < -1.0f) {
        newRect = rectangleClone(rect);
        rectangleMove(newRect, GL_HELPER_VIEWPORT_SIZE, 0);

        re->units[newIndex] = newRect;
        newIndex++;
      }

      if (rect->position[rect->yMax][1] > 1.0f) {
        newRect = rectangleClone(rect);
        rectangleMove(newRect, 0, -GL_HELPER_VIEWPORT_SIZE);

        re->units[newIndex] = newRect;
        newIndex++;
      }

      if (rect->position[rect->yMin][1] < -1.0f) {
        newRect = rectangleClone(rect);
        rectangleMove(newRect, 0, GL_HELPER_VIEWPORT_SIZE);

        re->units[newIndex] = newRect;
        newIndex++;
      }

      // if (newRect) {
      //  re->units[newIndex] = newRect;
      //  newIndex++;
      //}
    }

    re->vertexCounts = re->numUnits * (*re->units)->vertexCounts;
    re->wireframeVertexCounts =
        re->numUnits * (*re->units)->wireframeVertexCounts;

    re->vertexPosition = DEFENSE_MALLOC(re->vertexCounts * 3 * sizeof(GLfloat),
                                        mallocFailAbort, NULL);
    re->vertexUV = DEFENSE_MALLOC(re->vertexCounts * 2 * sizeof(GLfloat),
                                  mallocFailAbort, NULL);
    re->vertexColor = DEFENSE_MALLOC(re->vertexCounts * 3 * sizeof(GLfloat),
                                     mallocFailAbort, NULL);

    re->vertexWireframe = DEFENSE_MALLOC(
        re->wireframeVertexCounts * 3 * sizeof(GLfloat), mallocFailAbort, NULL);

    //    memcpy(re->vertexColor, pattern->vertexColor,
    //           re->vertexCounts * 3 * sizeof(GLfloat));

    patternModelInitPos(re);
    patternModelInitUV(re);

    patternModelInitColor(re);

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

    glFlush();
  }
}

void patternModelInitUnitsPosition(struct PatternModel *pattern) {
  double width = (double)GL_HELPER_VIEWPORT_SIZE / (double)pattern->numWidth;
  double height = (double)GL_HELPER_VIEWPORT_SIZE / (double)pattern->numHeight;

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

///////////////////////////////////////////////////////////////////////////////

static void modelGenerate01(struct PatternModel *pattern) {
  pattern->numUnits = pattern->numWidth * pattern->numHeight;
  pattern->units = DEFENSE_MALLOC(
      pattern->numUnits * sizeof(struct Rectangle *), mallocFailAbort, NULL);

  double width = (double)GL_HELPER_VIEWPORT_SIZE / (double)pattern->numWidth;
  double height = (double)GL_HELPER_VIEWPORT_SIZE / (double)pattern->numHeight;

  for (int h = 0; h < pattern->numHeight; h++) {
    for (int w = 0; w < pattern->numWidth; w++) {
      struct Rectangle *rect = rectangleNew();
      // we already randomize the color within the Rectangle construction
      // no need to do it again here
      // setRectRandColor(rect, 1.0f, 1.0f);

      rectangleSetWidth(rect, width);
      rectangleSetHeight(rect, height);

      // up right corner is the pivot of the Rectangle
      rectangleMove(rect, (w * -width), (h * -height));

      pattern->units[(h * pattern->numWidth) + w] = rect;
    }
  }

  pattern->vertexCounts = pattern->numUnits * (*pattern->units)->vertexCounts;
  pattern->wireframeVertexCounts =
      pattern->numUnits * (*pattern->units)->wireframeVertexCounts;

  pattern->vertexPosition = DEFENSE_MALLOC(
      pattern->vertexCounts * 3 * sizeof(GLfloat), mallocFailAbort, NULL);
  pattern->vertexUV = DEFENSE_MALLOC(
      pattern->vertexCounts * 2 * sizeof(GLfloat), mallocFailAbort, NULL);
  pattern->vertexColor = DEFENSE_MALLOC(
      pattern->vertexCounts * 3 * sizeof(GLfloat), mallocFailAbort, NULL);

  pattern->vertexWireframe =
      DEFENSE_MALLOC(pattern->wireframeVertexCounts * 3 * sizeof(GLfloat),
                     mallocFailAbort, NULL);

  // patternModelInitPos(pattern);
  // patternModelInitColor(pattern);
  // patternModelInitUV(pattern);
}

static void modelGenerate02(struct PatternModel *pattern) {
  modelGenerate01(pattern);

  double width = (double)GL_HELPER_VIEWPORT_SIZE / (double)pattern->numWidth;
  double moveAmount = tan(45 * ONE_DEG_IN_RAD) * width;

  for (int h = 0; h < pattern->numHeight; h++) {
    for (int w = 0; w < pattern->numWidth; w++) {
      struct Rectangle *rect = pattern->units[(h * pattern->numWidth) + w];

      if (w % 2 == 0) {
        rectangleMoveEdge(rect, 0, 3, 0.0f, -moveAmount);
        rectangleInitUVProject(rect);
        rectangleRotateUV(rect, 45.0f);
      } else {
        rectangleMoveEdge(rect, 1, 2, 0.0f, -moveAmount);
        rectangleInitUVProject(rect);
        rectangleRotateUV(rect, -45.0f);
      }
    }
  }
}

static void modelGenerate03(struct PatternModel *pattern) {
  //  pattern->numUnits = pattern->numWidth * pattern->numHeight;

  //  double width = (double)GL_HELPER_VIEWPORT_SIZE /
  //  (double)pattern->numWidth;
  //  double height = (double)GL_HELPER_VIEWPORT_SIZE /
  //  (double)pattern->numHeight;
  //
  //  GSList *rectList = NULL;
  //
  //  pattern->units = DEFENSE_MALLOC(pattern->numUnits * sizeof(struct
  //  Rectangle
  //  *),
  //                                 mallocFailAbort, NULL);
  //
  //
  //  for (int h = 0; h < pattern->numHeight; h++) {
  //    for (int w = 0; w < pattern->numWidth; w++) {
  //      struct Rectangle *rect = rectangleNew();
  //      // we already randomize the color within the Rectangle construction
  //      // no need to do it again here
  //      // setRectRandColor(rect, 1.0f, 1.0f);
  //
  //      rectangleSetWidth(rect, width);
  //      rectangleSetHeight(rect, height);
  //
  //      // up right corner is the pivot of the Rectangle
  //      rectangleMove(rect, (w * -width), (h * -height));
  //
  //      pattern->units[(h * pattern->numWidth) + w] = rect;
  //    }
  //  }
  //
  //  pattern->vertexCounts = pattern->numUnits *
  //  (*pattern->units)->vertexCounts;
  //  pattern->wireframeVertexCounts =
  //      pattern->numUnits * (*pattern->units)->wireframeVertexCounts;
  //
  //  pattern->vertexPosition = DEFENSE_MALLOC(
  //      pattern->vertexCounts * 3 * sizeof(GLfloat), mallocFailAbort, NULL);
  //  pattern->vertexUV = DEFENSE_MALLOC(pattern->vertexCounts * 2 *
  //  sizeof(GLfloat),
  //                                    mallocFailAbort, NULL);
  //  pattern->vertexColor = DEFENSE_MALLOC(
  //      pattern->vertexCounts * 3 * sizeof(GLfloat), mallocFailAbort, NULL);
  //
  //  pattern->vertexWireframe =
  //      DEFENSE_MALLOC(pattern->wireframeVertexCounts * 3 * sizeof(GLfloat),
  //                    mallocFailAbort, NULL);
  //  modelGenerate01(pattern);
  //
  //  double width = (GLfloat)GL_HELPER_VIEWPORT_SIZE /
  //  (GLfloat)pattern->numWidth;
  //  double moveAmount = tan(45 * ONE_DEG_IN_RAD) * width;
  //
  //  for (int h = 0; h < pattern->numHeight; h++) {
  //    for (int w = 0; w < pattern->numWidth; w++) {
  //      struct Rectangle *rect = pattern->units[(h * pattern->numWidth) +
  //      w];
  //
  //      if (w % 2 == 0) {
  //        rectangleMoveEdge(rect, 0, 3, 0.0f, -moveAmount);
  //        rectangleRotateUV(rect, 45.0f);
  //      } else {
  //        rectangleMoveEdge(rect, 1, 2, 0.0f, -moveAmount);
  //        rectangleRotateUV(rect, -45.0f);
  //      }
  //    }
  //  }
}

// static void modelUVConstruct01(struct PatternModel *pattern) {
//  // rectangleInitUVProject(rect);
//}
//
// static void modelUVConstruct02(struct PatternModel *pattern) {
//  // rectangleInitUVProject(rect);
//}

///////////////////////////////////////////////////////////////////////////

static void getDimensionNumber01(struct PatternModel *pattern,
                                 const unsigned int cpy) {
  double gcd = euclidGCD(pattern->sizeX, pattern->sizeY);
  double lcm =
      gcd * ((double)pattern->sizeX / gcd) * ((double)pattern->sizeY / gcd);

  double numWidth = (lcm / (double)pattern->sizeX) * cpy;
  double numHeight = (lcm / (double)pattern->sizeY) * cpy;

  pattern->numWidth = (unsigned int)numWidth;
  pattern->numHeight = (unsigned int)numHeight;
}

static void getDimensionNumber02(struct PatternModel *pattern,
                                 const unsigned int cpy) {
  double gcd = euclidGCD(pattern->sizeX * 2, pattern->sizeY);

  double lcm =
      gcd * ((double)pattern->sizeX * 2 / gcd) * ((double)pattern->sizeY / gcd);

  double numWidth = (lcm / (double)pattern->sizeX) * cpy;
  double numHeight = (lcm / (double)pattern->sizeY) * cpy;

  // g_print("numHeight: %f\n", numHeight);

  pattern->numWidth = (unsigned int)numWidth;
  pattern->numHeight = (unsigned int)numHeight;
}

static void getDimensionNumber03(struct PatternModel *pattern,
                                 const unsigned int cpy) {
  double gcd = euclidGCD(pattern->sizeX * 2, pattern->sizeY);

  double lcm =
      gcd * ((double)pattern->sizeX * 2 / gcd) * ((double)pattern->sizeY / gcd);

  double shortEdge =
      pattern->sizeX > pattern->sizeY ? pattern->sizeY : pattern->sizeX;

  double numWidth = (lcm / (double)shortEdge) * cpy;
  double numHeight = (lcm / (double)shortEdge) * cpy;

  pattern->numWidth = (unsigned int)numWidth;
  pattern->numHeight = (unsigned int)numHeight;
}

///////////////////////////////////////////////////////////////////////////

typedef void dimensionFunc(struct PatternModel *pattern,
                           const unsigned int cpy);
typedef void generateFunc(struct PatternModel *pattern);

///////////////////////////////////////////////////////////////////////////

struct PatternModel *patternModelNew(GtkGLArea *glArea,
                                     const unsigned int sizeX,
                                     const unsigned int sizeY,
                                     const unsigned int cpy,
                                     const unsigned int patternIndex) {
  struct PatternModel *pattern =
      DEFENSE_MALLOC(sizeof(struct PatternModel), mallocFailAbort, NULL);

  pattern->sizeX = sizeX;
  pattern->sizeY = sizeY;

  dimensionFunc *dimensionNumberFunc[3] = {
      getDimensionNumber01, getDimensionNumber02, getDimensionNumber03,
  };
  generateFunc *modelGenerateFunc[3] = {
      modelGenerate01, modelGenerate02, modelGenerate03,
  };

  dimensionNumberFunc[patternIndex](pattern, cpy);
  modelGenerateFunc[patternIndex](pattern);

  patternModelInitPos(pattern);
  patternModelInitColor(pattern);
  patternModelInitUV(pattern);

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

  if (patternIndex != 0) {
    patternModelSeamlessModelConstruct(pattern, glArea);
  }

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
