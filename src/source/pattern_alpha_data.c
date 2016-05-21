struct rectangle {
  GLfloat **position;
  GLfloat **uv;

  GLfloat width;
  GLfloat height;

  // GLfloat *pivot;
  // GLfloat **xMax;
  // GLfloat **xMin;
  // GLfloat **yMax;
  // GLfloat **yMin;

  unsigned int pivot;
  unsigned int *xMax;
  unsigned int *xMin;
  unsigned int *yMax;
  unsigned int *yMin;

  unsigned int *vertexOrder;

  GLfloat *vertexPosition;
  GLfloat *vertexUV;
  GLfloat *vertexColor;

  int vertexCounts;

  GLfloat randColor;
};

static void constructRectangleVertex(struct rectangle *rect) {
  for (int i = 0; i < rect->vertexCounts; i++) {
    for (int j = 0; j < 3; j++) {
      rect->vertexPosition[(i * 3) + j] =
          rect->position[rect->vertexOrder[i]][j];
    }

    for (int j = 0; j < 2; j++) {
      rect->vertexUV[(i * 2) + j] = rect->uv[rect->vertexOrder[i]][j];
    }
  }
}

static float fit01(float src, double newMin, double newMax) {
  if (src > 1.0f) {
    src = 1.0f;
  } else if (src < 0.0f) {
    src = 0.0f;
  }

  double newRange = newMax - newMin;
  // float m = 1.0f / newRange;
  // float m = newRange;
  // float re = (src / m) + newMin;
  float re = (src * newRange) + newMin;

  return re;
}

static void genRectRandColor(struct rectangle *rect) {
  // float randColor = (GLfloat)rand() / (GLfloat)RAND_MAX;
  // rect->randColor = fit01(randColor, min, max);
  rect->randColor = (GLfloat)rand() / (GLfloat)RAND_MAX;

  for (int i = 0; i < 18; i++) {
    rect->vertexColor[i] = rect->randColor;
  }
}

static struct rectangle *newRectangle() {
  struct rectangle *re = calloc(1, sizeof(struct rectangle));
  re->vertexCounts = 6;

  re->position = (GLfloat **)calloc(4, sizeof(GLfloat *));
  re->uv = (GLfloat **)calloc(4, sizeof(GLfloat *));

  re->width = 1.0f;
  re->height = 1.0f;

  // we want to modified the existing position data instead of allocating new
  // memory space
  // re->pivot = (GLfloat **)calloc(1, sizeof(GLfloat *));

  //  re->xMax = (GLfloat **)calloc(2, sizeof(GLfloat *));
  //  re->xMin = (GLfloat **)calloc(2, sizeof(GLfloat *));
  //  re->yMax = (GLfloat **)calloc(2, sizeof(GLfloat *));
  //  re->yMin = (GLfloat **)calloc(2, sizeof(GLfloat *));

  re->xMax = (unsigned int *)calloc(2, sizeof(unsigned int));
  re->xMin = (unsigned int *)calloc(2, sizeof(unsigned int));
  re->yMax = (unsigned int *)calloc(2, sizeof(unsigned int));
  re->yMin = (unsigned int *)calloc(2, sizeof(unsigned int));

  for (int i = 0; i < 4; i++) {
    re->position[i] = (GLfloat *)calloc(3, sizeof(GLfloat));
    re->uv[i] = (GLfloat *)calloc(2, sizeof(GLfloat));
  }

  //  re->position[0][0] = 0.5f;
  //  re->position[0][1] = 0.5f;
  //  re->position[0][2] = 0.0f;
  //
  //  re->position[1][0] = -0.5f;
  //  re->position[1][1] = 0.5f;
  //  re->position[1][2] = 0.0f;
  //
  //  re->position[2][0] = -0.5f;
  //  re->position[2][1] = -0.5f;
  //  re->position[2][2] = 0.0f;
  //
  //  re->position[3][0] = 0.5f;
  //  re->position[3][1] = -0.5f;
  //  re->position[3][2] = 0.0f;

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

  // re->xMax[0] = re->position[0];
  // re->xMax[1] = re->position[3];

  // re->xMin[0] = re->position[1];
  // re->xMin[1] = re->position[2];

  // re->yMax[0] = re->position[0];
  // re->yMax[1] = re->position[1];

  // re->yMin[0] = re->position[2];
  // re->yMin[1] = re->position[3];

  // re->pivot = re->position[0];

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
      (unsigned int *)calloc(re->vertexCounts, sizeof(unsigned int));

  re->vertexOrder[0] = 0;
  re->vertexOrder[1] = 1;
  re->vertexOrder[2] = 2;

  re->vertexOrder[3] = 2;
  re->vertexOrder[4] = 3;
  re->vertexOrder[5] = 0;

  re->vertexPosition = (GLfloat *)calloc(18, sizeof(GLfloat));
  re->vertexUV = (GLfloat *)calloc(12, sizeof(GLfloat));
  re->vertexColor = (GLfloat *)calloc(18, sizeof(GLfloat));

  constructRectangleVertex(re);
  genRectRandColor(re);

  // re->position = (GLfloat *)calloc(18, sizeof(GLfloat));

  // re->position[0] = 0.5f;
  // re->position[1] = -0.5f;
  // re->position[2] = 0.0f;

  // re->position[3] = 0.5f;
  // re->position[4] = 0.5f;
  // re->position[5] = 0.0f;

  // re->position[6] = -0.5f;
  // re->position[7] = 0.5f;
  // re->position[8] = 0.0f;

  // re->position[9] = -0.5f;
  // re->position[10] = -0.5f;
  // re->position[11] = 0.0f;

  // re->position[12] = 0.5f;
  // re->position[13] = -0.5f;
  // re->position[14] = 0.0f;

  // re->position[15] = -0.5f;
  // re->position[16] = 0.5f;
  // re->position[17] = 0.0f;

  return re;
}

static void moveRectangle(struct rectangle *rect, GLfloat x, GLfloat y) {
  for (int i = 0; i < 4; i++) {
    rect->position[i][0] += x;
    rect->position[i][1] += y;
  }

  constructRectangleVertex(rect);
}

static void moveRectangleTo(struct rectangle *rect, GLfloat x, GLfloat y) {
  GLfloat mX = x - rect->position[rect->pivot][0];
  GLfloat mY = y - rect->position[rect->pivot][1];
  moveRectangle(rect, mX, mY);
}

static void setRectWidth(struct rectangle *rect, GLfloat width) {
  for (int i = 0; i < 2; i++) {
    rect->position[rect->xMin[i]][0] = rect->position[rect->xMax[0]][0] - width;
    // rect->uv[rect->xMin[i]][0] =
    // rect->uv[rect->xMax[0]][0] - (width / (GLfloat)__GL_VIEWPORT);
    // rect->uv[i][0] = rect->xMax[0][0] - width;
  }

  rect->width = width;
  constructRectangleVertex(rect);
}

static void setRectHeight(struct rectangle *rect, GLfloat height) {
  for (int i = 0; i < 2; i++) {
    rect->position[rect->yMin[i]][1] =
        rect->position[rect->yMax[0]][1] - height;
    // rect->uv[rect->yMin[i]][0] =
    // rect->uv[rect->yMax[0]][0] - (height / (GLfloat)__GL_VIEWPORT);
    // rect->yMin[i][1] = rect->yMax[0][0] - height;
  }

  rect->height = height;
  constructRectangleVertex(rect);
}

static void initRectUVScale(struct rectangle *rect) {
  // GLfloat scaleFactor = rect->width > rect->height ? rect->height /
  // rect->width
  //: rect->width / rect->height;

  if (rect->width > rect->height) {
    GLfloat scaleFactor = rect->height / rect->width;
    rect->uv[rect->yMax[0]][1] = scaleFactor;
    rect->uv[rect->yMax[1]][1] = scaleFactor;
  } else {
    GLfloat scaleFactor = rect->width / rect->height;
    rect->uv[rect->xMax[0]][0] = scaleFactor;
    rect->uv[rect->xMax[1]][0] = scaleFactor;
  }
}

static void freeRectangle(struct rectangle *re) {
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

struct patternModel {
  unsigned int sizeX;
  unsigned int sizeY;

  struct rectangle **units;
  unsigned int numUnits;
  int vertexCounts;

  GLfloat *vertexPosition;
  GLfloat *vertexUV;
  GLfloat *vertexColor;

  gchar *texturePath;

  GLuint vao;
  GLuint positionVBO;
  GLuint colorVBO;
};

static void initPatternGLData(struct patternModel *pattern) {
  for (int i = 0; i < pattern->numUnits; i++) {
    int vertexVectorDataCounts = pattern->units[i]->vertexCounts * 3;
    struct rectangle *rect = pattern->units[i];

    for (int j = 0; j < vertexVectorDataCounts; j++) {
      pattern->vertexPosition[(i * vertexVectorDataCounts) + j] =
          rect->vertexPosition[j];
      // pattern->vertexColor[(i * vertexDataCounts) + j] =
      // rect->vertexColor[j];
      pattern->vertexColor[(i * vertexVectorDataCounts) + j] = 1.0f;
    }

    int vertexUVDataCounts = pattern->units[i]->vertexCounts * 2;
    for (int j = 0; j < vertexUVDataCounts; j++) {
      pattern->vertexUV[(i * vertexUVDataCounts) + j] = rect->vertexUV[j];
      // pattern->vertexColor[(i * vertexDataCounts) + j] =
      // rect->vertexColor[j];
      // pattern->vertexColor[(i * vertexUVDataCounts) + j] = 1.0f;
    }
  }
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
  // setVBOData(&pattern->colorVBO, pattern->vertexCounts, 3,
  // pattern->vertexColor);

  // for (int i = 0; i < pattern->vertexCounts * 3; i++) {
  //  pattern->vertexColor[i] = fit01(pattern->vertexColor[i], min, max);
  //}
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

  initPatternGLData(pattern);
  // combinePatternUnitsColor(pattern, 0.0f, 1.0f);
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
  pattern->texturePath = NULL;

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
