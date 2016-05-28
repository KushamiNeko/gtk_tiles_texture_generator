struct rectangle {
  GLfloat **position;
  GLfloat **uv;

  GLfloat width;
  GLfloat height;

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
  float re = (src * newRange) + newMin;

  return re;
}

static void genRectRandColor(struct rectangle *rect) {
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

  re->xMax = (unsigned int *)calloc(2, sizeof(unsigned int));
  re->xMin = (unsigned int *)calloc(2, sizeof(unsigned int));
  re->yMax = (unsigned int *)calloc(2, sizeof(unsigned int));
  re->yMin = (unsigned int *)calloc(2, sizeof(unsigned int));

  for (int i = 0; i < 4; i++) {
    re->position[i] = (GLfloat *)calloc(3, sizeof(GLfloat));
    re->uv[i] = (GLfloat *)calloc(2, sizeof(GLfloat));
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
  }

  rect->width = width;
  constructRectangleVertex(rect);
}

static void setRectHeight(struct rectangle *rect, GLfloat height) {
  for (int i = 0; i < 2; i++) {
    rect->position[rect->yMin[i]][1] =
        rect->position[rect->yMax[0]][1] - height;
  }

  rect->height = height;
  constructRectangleVertex(rect);
}

static void initRectUVScale(struct rectangle *rect) {
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
