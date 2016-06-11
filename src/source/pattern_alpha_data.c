#include "rectangle_unit.c"

struct patternModel {
  unsigned int sizeX;
  unsigned int sizeY;

  struct rectangle **units;
  unsigned int numUnits;
  int vertexCounts;

  GLfloat *vertexPosition;
  GLfloat *vertexUV;
  GLfloat *vertexColor;

  GLfloat *wireframeColor;

  // gchar *texturePath;

  GLuint vao;
  GLuint positionVBO;
  GLuint uvVBO;
  GLuint colorVBO;

  GLuint wireframeColorVBO;
};

static void initPatternUV(struct patternModel *pattern) {
  for (int i = 0; i < pattern->numUnits; i++) {
    struct rectangle *rect = pattern->units[i];

    int vertexUVDataCounts = pattern->units[i]->vertexCounts * 2;
    for (int j = 0; j < vertexUVDataCounts; j++) {
      pattern->vertexUV[(i * vertexUVDataCounts) + j] = rect->vertexUV[j];
    }
  }
}

static void initPatternRandUV(struct patternModel *pattern) {
  for (int i = 0; i < pattern->numUnits; i++) {
    genRectRandUV(pattern->units[i]);
  }

  initPatternUV(pattern);
}

static void initPatternUVScale(struct patternModel *pattern,
                               double scaleFactor) {
  for (int i = 0; i < pattern->numUnits; i++) {
    scaleRectUV(pattern->units[i], scaleFactor);
  }

  initPatternUV(pattern);
}

static void initPatternGLData(struct patternModel *pattern) {
  for (int i = 0; i < pattern->numUnits; i++) {
    int vertexVectorDataCounts = pattern->units[i]->vertexCounts * 3;
    struct rectangle *rect = pattern->units[i];

    for (int j = 0; j < vertexVectorDataCounts; j++) {
      pattern->vertexPosition[(i * vertexVectorDataCounts) + j] =
          rect->vertexPosition[j];
      pattern->vertexColor[(i * vertexVectorDataCounts) + j] = 1.0f;
    }

    // int vertexUVDataCounts = pattern->units[i]->vertexCounts * 2;
    // for (int j = 0; j < vertexUVDataCounts; j++) {
    //  pattern->vertexUV[(i * vertexUVDataCounts) + j] = rect->vertexUV[j];
    //}
  }

  initPatternUV(pattern);
}

static void fitPatternRandColor(struct patternModel *pattern, float min,
                                float max) {
  for (int i = 0; i < pattern->numUnits; i++) {
    int vertexDataCounts = pattern->units[i]->vertexCounts * 3;
    struct rectangle *rect = pattern->units[i];

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

static void initPatternRandColor(struct patternModel *pattern, float min,
                                 float max) {
  for (int i = 0; i < pattern->numUnits; i++) {
    genRectRandColor(pattern->units[i]);
  }

  fitPatternRandColor(pattern, min, max);
}

static void modelGenerate(struct patternModel *pattern,
                          const unsigned int numWidth,
                          const unsigned int numHeight) {
  pattern->numUnits = numWidth * numHeight;
  pattern->units = calloc(pattern->numUnits, sizeof(struct rectangle *));

  GLfloat width = (GLfloat)__GL_VIEWPORT / (GLfloat)numWidth;
  GLfloat height = (GLfloat)__GL_VIEWPORT / (GLfloat)numHeight;

  for (int h = 0; h < numHeight; h++) {
    for (int w = 0; w < numWidth; w++) {
      struct rectangle *rect = newRectangle();
      // we already randomize the color within the rectangle construction
      // no need to do it again here
      // setRectRandColor(rect, 1.0f, 1.0f);

      setRectWidth(rect, width);
      setRectHeight(rect, height);

      // up right corner is the pivot of the rectangle
      moveRectangle(rect, (w * -width), (h * -height));

      pattern->units[(h * numWidth) + w] = rect;
    }
  }

  pattern->vertexCounts = pattern->numUnits * (*pattern->units)->vertexCounts;

  pattern->vertexPosition = calloc(pattern->vertexCounts * 3, sizeof(GLfloat));
  pattern->vertexUV = calloc(pattern->vertexCounts * 2, sizeof(GLfloat));
  pattern->vertexColor = calloc(pattern->vertexCounts * 3, sizeof(GLfloat));

  pattern->wireframeColor = calloc(pattern->vertexCounts * 3, sizeof(GLfloat));

  initPatternGLData(pattern);
}

static unsigned int euclidGCD(const unsigned int x, const unsigned int y) {
  unsigned int a, b, r;

  a = x;
  b = y;
  r = a % b;

  while (r != 0) {
    a = b;
    b = r;
    r = a % b;
  }

  return b;
}

static struct patternModel *patternConstruct(GtkGLArea *glArea,
                                             const unsigned int sizeX,
                                             const unsigned int sizeY,
                                             const unsigned int cpy) {
  struct patternModel *pattern = calloc(1, sizeof(struct patternModel));
  // pattern->texturePath = NULL;

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

static void freePatternModel(struct patternModel *pattern) {
  for (int i = 0; i < pattern->numUnits; i++) {
    freeRectangle(pattern->units[i]);
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
