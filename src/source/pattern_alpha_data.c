struct rectangle {
  GLfloat **position;
  GLfloat **uv;

  GLfloat *pivot;
  GLfloat **xMax;
  GLfloat **xMin;
  GLfloat **yMax;
  GLfloat **yMin;

  // GLfloat *position;

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
  }
}

static void setRectRandColor(struct rectangle *rect, double seed) {
  srand(time(NULL) * seed);
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

  // we want to modified the existing position data instead of allocating new
  // memory space
  // re->pivot = (GLfloat **)calloc(1, sizeof(GLfloat *));

  re->xMax = (GLfloat **)calloc(2, sizeof(GLfloat *));
  re->xMin = (GLfloat **)calloc(2, sizeof(GLfloat *));
  re->yMax = (GLfloat **)calloc(2, sizeof(GLfloat *));
  re->yMin = (GLfloat **)calloc(2, sizeof(GLfloat *));

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

  re->xMax[0] = re->position[0];
  re->xMax[1] = re->position[3];

  re->xMin[0] = re->position[1];
  re->xMin[1] = re->position[2];

  re->yMax[0] = re->position[0];
  re->yMax[1] = re->position[1];

  re->yMin[0] = re->position[2];
  re->yMin[1] = re->position[3];

  re->pivot = re->position[0];

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
  setRectRandColor(re, time(NULL));

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
  GLfloat mX = x - rect->pivot[0];
  GLfloat mY = y - rect->pivot[1];
  moveRectangle(rect, mX, mY);
}

static void setRectWidth(struct rectangle *rect, GLfloat width) {
  for (int i = 0; i < 2; i++) {
    rect->xMin[i][0] = rect->xMax[0][0] - width;
  }

  constructRectangleVertex(rect);
}

static void setRectHeight(struct rectangle *rect, GLfloat height) {
  for (int i = 0; i < 2; i++) {
    rect->yMin[i][1] = rect->yMax[0][0] - height;
  }

  constructRectangleVertex(rect);
}

static void freeRectangle(struct rectangle *re) {
  free(re->position);
  free(re->uv);
  free(re->xMin);
  free(re->yMin);
  free(re->vertexOrder);
  free(re->vertexPosition);
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

  GLuint vao;
  GLuint positionVBO;
  GLuint colorVBO;
};

static void combinePatternUnits(struct patternModel *pattern) {
  for (int i = 0; i < pattern->numUnits; i++) {
    int vertexDataCounts = pattern->units[i]->vertexCounts * 3;
    struct rectangle *rect = pattern->units[i];

    for (int j = 0; j < vertexDataCounts; j++) {
      pattern->vertexPosition[(i * vertexDataCounts) + j] =
          rect->vertexPosition[j];
      pattern->vertexColor[(i * vertexDataCounts) + j] = rect->vertexColor[j];
    }
  }
}

static void setPatternRandColor(struct patternModel *pattern, double seed) {
  // setRectRandColor(pattern->units[0], seed);
  for (int i = 0; i < pattern->numUnits; i++) {
    setRectRandColor(pattern->units[i], seed * (i + 1));
  }

  combinePatternUnits(pattern);
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
      setRectRandColor(rect, (h + 1) * (w + 1));

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

  combinePatternUnits(pattern);
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
  free(pattern->vertexPosition);
  free(pattern->vertexColor);
  free(pattern->vertexUV);
  free(pattern);
}
