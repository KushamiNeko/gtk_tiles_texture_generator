#include "../header/pattern_alpha_model.h"

// struct PatternModel {
//  unsigned int sizeX;
//  unsigned int sizeY;
//
//  struct Rectangle **units;
//  unsigned int numUnits;
//  unsigned int vertexCounts;
//
//  GLfloat *vertexPosition;
//  GLfloat *vertexUV;
//  GLfloat *vertexColor;
//
//  GLfloat *wireframeColor;
//
//  // gchar *texturePath;
//
//  GLuint vao;
//  GLuint positionVBO;
//  GLuint uvVBO;
//  GLuint colorVBO;
//
//  // GLuint wireframeColorVBO;
//};

static inline void genRectRandUV(struct Rectangle *rect) {
  GLfloat amountX = (GLfloat)rand() / (GLfloat)RAND_MAX;
  GLfloat amountY = (GLfloat)rand() / (GLfloat)RAND_MAX;

  amountX = (amountX * 2.0f) - 1.0f;
  amountY = (amountY * 2.0f) - 1.0f;

  rectangleMoveUV(rect, amountX, amountY);
}

static inline void genRectRandColor(struct Rectangle *rect) {
  GLfloat color = (GLfloat)rand() / (GLfloat)RAND_MAX;
  rectangleSetColorValue(rect, color);
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

// static void initPatternGLData(struct PatternModel *pattern) {
//  for (int i = 0; i < pattern->numUnits; i++) {
//    int vertexVectorDataCounts = pattern->units[i]->vertexCounts * 3;
//    struct Rectangle *rect = pattern->units[i];
//
//    for (int j = 0; j < vertexVectorDataCounts; j++) {
//      pattern->vertexPosition[(i * vertexVectorDataCounts) + j] =
//          rect->vertexPosition[j];
//      pattern->vertexColor[(i * vertexVectorDataCounts) + j] = 1.0f;
//    }
//
//    // int vertexUVDataCounts = pattern->units[i]->vertexCounts * 2;
//    // for (int j = 0; j < vertexUVDataCounts; j++) {
//    //  pattern->vertexUV[(i * vertexUVDataCounts) + j] = rect->vertexUV[j];
//    //}
//  }
//
//  patternModelInitUV(pattern);
//}

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

static void modelGenerate(struct PatternModel *pattern,
                          const unsigned int numWidth,
                          const unsigned int numHeight) {
  pattern->numUnits = numWidth * numHeight;
  pattern->units = defenseCalloc(pattern->numUnits, sizeof(struct Rectangle *));

  GLfloat width = (GLfloat)__GL_VIEWPORT / (GLfloat)numWidth;
  GLfloat height = (GLfloat)__GL_VIEWPORT / (GLfloat)numHeight;

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

  pattern->vertexPosition = calloc(pattern->vertexCounts * 3, sizeof(GLfloat));
  pattern->vertexUV = calloc(pattern->vertexCounts * 2, sizeof(GLfloat));
  pattern->vertexColor = calloc(pattern->vertexCounts * 3, sizeof(GLfloat));

  pattern->wireframeColor = calloc(pattern->vertexCounts * 3, sizeof(GLfloat));

  patternModelInitPos(pattern);
  patternModelInitColor(pattern);
  patternModelInitUV(pattern);
}

struct PatternModel *patternModelNew(GtkGLArea *glArea,
                                     const unsigned int sizeX,
                                     const unsigned int sizeY,
                                     const unsigned int cpy) {
  struct PatternModel *pattern = defenseCalloc(1, sizeof(struct PatternModel));

  pattern->sizeX = sizeX;
  pattern->sizeY = sizeY;

  GLfloat gcd = euclidGCD(sizeX, sizeY);
  GLfloat lcm = gcd * (sizeX / gcd) * (sizeY / gcd);

  GLfloat numWidth = (lcm / sizeX) * cpy;
  GLfloat numHeight = (lcm / sizeY) * cpy;

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

  pattern->vao = vao;

  return pattern;
}

void patternModelFree(struct PatternModel *pattern) {
  for (int i = 0; i < pattern->numUnits; i++) {
    rectangleFree(pattern->units[i]);
  }
  free(pattern->units);

  // free gpu memories or the program will crash
  glDeleteBuffers(1, &pattern->positionVBO);
  glDeleteBuffers(1, &pattern->colorVBO);

  glDeleteVertexArrays(1, &pattern->vao);

  free(pattern->vertexPosition);
  free(pattern->vertexColor);
  free(pattern->vertexUV);
  free(pattern);
}
