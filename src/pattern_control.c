#include "pattern_control.h"

#ifdef UNIT_TESTING
#include <cmockery/cmockery_override.h>
#endif

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../third_party/stb/stb_image_write.h"

struct PatternData {
  GtkGLArea *glArea;

  gchar *textureFile;
  GLuint tex;
  GLint texLoc;

  gchar *renderPath;

  GLuint shaderProgram;

  struct PatternModel *pattern;

  GLuint wireframeSwitch;

  GLint wireframeDrawUniformLoc;
  GLint wireframeDraw;

  GLint wireframeColorUniformLoc;
  GLfloat *wireframeColor;
};

static gboolean patternDataInitTexMap(struct PatternData *patternData) {
  if (loadTexture(patternData->textureFile, &(patternData->shaderProgram),
                  GL_TEXTURE0, &(patternData->tex),
                  &(patternData->texLoc)) != 1) {
    return FALSE;
  }
  return TRUE;
}

void *patternControlDataInit(GtkGLArea *glArea, GLuint shaderProgram,
                             struct PatternModel *pattern) {
  struct PatternData *re = (struct PatternData *)DEFENSE_MALLOC(
      sizeof(struct PatternData), mallocFailAbort, NULL);

  re->glArea = glArea;

  re->textureFile = DEFAULT_TEXTURE;

  gtk_gl_area_make_current(glArea);
  glUseProgram(shaderProgram);

  glGenTextures(1, &(re->tex));

  re->texLoc = glGetUniformLocation(shaderProgram, "diff_tex");

  patternDataInitTexMap(re);

  re->renderPath = NULL;

  re->shaderProgram = shaderProgram;
  re->pattern = pattern;

  re->wireframeSwitch = 1;

  re->wireframeDrawUniformLoc =
      glGetUniformLocation(shaderProgram, "wireframeDraw");

  re->wireframeDraw = 0;

  gtk_gl_area_make_current(glArea);
  glUseProgram(shaderProgram);
  glUniform1i(re->wireframeDrawUniformLoc, re->wireframeDraw);

  re->wireframeColorUniformLoc =
      glGetUniformLocation(shaderProgram, "wireframeColor");

  re->wireframeColor =
      DEFENSE_MALLOC(3 * sizeof(GLfloat), mallocFailAbort, NULL);
  // memset(re->wireframeColor, 1, 3 * sizeof(GLfloat));

  re->wireframeColor[0] = 1.0f;
  re->wireframeColor[1] = 1.0f;
  re->wireframeColor[2] = 1.0f;

  gtk_gl_area_make_current(glArea);
  glUseProgram(shaderProgram);
  glUniform3fv(re->wireframeColorUniformLoc, 1, re->wireframeColor);
  // patternDataInitTexMap(re);

  return (void *)re;
}

void patternControlWireframeSwitchToggled(GtkToggleButton *toggleButton,
                                          void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = (struct PatternData *)control->patternData;

  user->wireframeSwitch = !user->wireframeSwitch;

  gtk_gl_area_make_current(user->glArea);
  glFlush();

  gtk_gl_area_queue_render(user->glArea);
}

void patternControlWireframeColorSet(GtkColorButton *widget, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = (struct PatternData *)control->patternData;

  GdkRGBA color;
  gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(widget), &color);

  user->wireframeColor[0] = color.red;
  user->wireframeColor[1] = color.green;
  user->wireframeColor[2] = color.blue;

  gtk_gl_area_make_current(user->glArea);
  glUseProgram(user->shaderProgram);
  glUniform3fv(user->wireframeColorUniformLoc, 1, user->wireframeColor);

  glFlush();

  gtk_gl_area_queue_render(user->glArea);
}

static void constructSeamlessModel(struct PatternData *user, double colorMin,
                                   double colorMax) {
  patternModelSeamlessModelConstruct(user->pattern, user->glArea);

  if (user->pattern->seamlessModel) {
    patternModelFitColor(user->pattern->seamlessModel, colorMin, colorMax);

    setVBOData(&user->pattern->seamlessModel->colorVBO,
               user->pattern->seamlessModel->vertexCounts, 3,
               user->pattern->seamlessModel->vertexColor);
  }
}

void patternControlColorRangeChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = (struct PatternData *)control->patternData;

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  // only re-generate GL color data and fit it into new range
  patternModelFitColor(user->pattern, colorMin, colorMax);

  // patternModelSeamlessModelConstruct(user->pattern, user->glArea);

  gtk_gl_area_make_current(user->glArea);

  constructSeamlessModel(user, colorMin, colorMax);

  setVBOData(&user->pattern->colorVBO, user->pattern->vertexCounts, 3,
             user->pattern->vertexColor);

  // setVBOData(&user->pattern->seamlessModel->colorVBO,
  //           user->pattern->seamlessModel->vertexCounts, 3,
  //           user->pattern->seamlessModel->vertexColor);

  // fitSeamlessModelColor(user->pattern, colorMin, colorMax);

  glFlush();

  // queue openGL render
  gtk_gl_area_queue_render(user->glArea);
}

void patternControlColorSeedChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = (struct PatternData *)control->patternData;

  //  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  //  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  // re-construct the color of whole pattern units
  patternModelRandomizeColor(user->pattern);

  patternControlColorRangeChanged(GTK_RANGE(control->colorMinSlider), userData);

  // patternModelSeamlessModelConstruct(user->pattern, user->glArea);

  //  patternModelFitColor(user->pattern, colorMin, colorMax);
  //
  //  // patternModelFitColor(pattern->seamlessModel, colorMin, colorMax);
  //
  //  gtk_gl_area_make_current(user->glArea);
  //
  //  // directly update the specific memory allocated for the data
  //  setVBOData(&user->pattern->colorVBO, user->pattern->vertexCounts, 3,
  //             user->pattern->vertexColor);
  //
  //  constructSeamlessModel(user, colorMin, colorMax);
  //
  //  // setVBOData(&pattern->seamlessModel->colorVBO,
  //  //           pattern->seamlessModel->vertexCounts, 3,
  //  //           pattern->seamlessModel->vertexColor);
  //
  //  glFlush();
  //
  //  // queue openGL render
  //  gtk_gl_area_queue_render(user->glArea);
}

static double normalizeUVScaleRange(double rangeValue) {
  rangeValue = (rangeValue - 2) * -1;
  double scaleFactor;
  if (rangeValue > 1.0f) {
    scaleFactor = fit01(rangeValue - 1.0f, 1.0f, 5.0f);
  } else if (rangeValue < 1.0f) {
    scaleFactor = fit01(rangeValue, 0.1f, 1.0f);
  } else {
    scaleFactor = rangeValue;
  }

  return scaleFactor;
}

void patternControlUvScaleChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = (struct PatternData *)control->patternData;

  double rangeValue = gtk_range_get_value(GTK_RANGE(range));
  double scaleFactor = normalizeUVScaleRange(rangeValue);

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  patternModelScaleUV(user->pattern, scaleFactor);

  // patternModelSeamlessModelConstruct(user->pattern, user->glArea);

  gtk_gl_area_make_current(user->glArea);

  constructSeamlessModel(user, colorMin, colorMax);

  setVBOData(&user->pattern->uvVBO, user->pattern->vertexCounts, 2,
             user->pattern->vertexUV);

  // setVBOData(&user->pattern->seamlessModel->colorVBO,
  //           user->pattern->seamlessModel->vertexCounts, 3,
  //           user->pattern->seamlessModel->vertexColor);

  // fitSeamlessModelColor(user->pattern, colorMin, colorMax);

  glFlush();

  gtk_gl_area_queue_render(user->glArea);
}

void patternControlRandUVSeedChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = (struct PatternData *)control->patternData;

  // double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  // double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  gboolean active = gtk_toggle_button_get_active(
      GTK_TOGGLE_BUTTON(control->uvRotateCheckButton));

  patternModelRandomizeUV(user->pattern);

  if (active) {
    patternModelRandomizeUVRotate(user->pattern);
  }

  patternControlUvScaleChanged(GTK_RANGE(control->uvScaleSlider), userData);

  //  double scaleRangeValue =
  //      gtk_range_get_value(GTK_RANGE(control->uvScaleSlider));
  //  double scaleFactor = normalizeUVScaleRange(scaleRangeValue);
  //
  //  patternModelScaleUV(user->pattern, scaleFactor);
  //
  //  //  patternModelSeamlessModelConstruct(user->pattern, user->glArea,
  //  colorMin,
  //  //                                     colorMax);
  //
  //  gtk_gl_area_make_current(user->glArea);
  //
  //  setVBOData(&user->pattern->uvVBO, user->pattern->vertexCounts, 2,
  //             user->pattern->vertexUV);
  //
  //  // setVBOData(&user->pattern->seamlessModel->colorVBO,
  //  //           user->pattern->seamlessModel->vertexCounts, 3,
  //  //           user->pattern->seamlessModel->vertexColor);
  //
  //  // fitSeamlessModelColor(user->pattern, colorMin, colorMax);
  //
  //  constructSeamlessModel(user, colorMin, colorMax);
  //
  //  glFlush();
  //
  //  gtk_gl_area_queue_render(user->glArea);
}

void patternControlUvRotateToggled(GtkToggleButton *toggleButton,
                                   void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = (struct PatternData *)control->patternData;

  //  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  //  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  gboolean active = gtk_toggle_button_get_active(toggleButton);

  if (active) {
    patternModelRandomizeUVRotate(user->pattern);
  } else {
    guint patternType =
        gtk_combo_box_get_active(GTK_COMBO_BOX(control->patternTypeComboBox));

    for (int h = 0; h < user->pattern->numHeight; h++) {
      for (int w = 0; w < user->pattern->numWidth; w++) {
        struct Rectangle *rect =
            user->pattern->units[(h * user->pattern->numWidth) + w];

        // rectangleInitUVProject(user->pattern->units[i]);
        rectangleInitUVProject(rect);

        if (patternType == 1) {
          if (w % 2 == 0) {
            rectangleRotateUV(rect, 45.0f);
          } else {
            rectangleRotateUV(rect, -45.0f);
          }
        }
      }
    }
  }

  patternModelRandomizeUV(user->pattern);

  patternControlUvScaleChanged(GTK_RANGE(control->uvScaleSlider), userData);

  /* double scaleRangeValue = */
  /*     gtk_range_get_value(GTK_RANGE(control->uvScaleSlider)); */
  /* double scaleFactor = normalizeUVScaleRange(scaleRangeValue); */

  /* patternModelScaleUV(user->pattern, scaleFactor); */

  /* patternModelSeamlessModelConstruct(user->pattern, user->glArea); */

  /* fitSeamlessModelColor(user->pattern, colorMin, colorMax); */

  /* setVBOData(&user->pattern->uvVBO, user->pattern->vertexCounts, 2, */
  /*            user->pattern->vertexUV); */

  //  gtk_gl_area_make_current(user->glArea);
  //
  //  glFlush();
  //
  //  gtk_gl_area_queue_render(user->glArea);
}

///////////////////////////////////////////////////////////////////////

static void patternOffsetModulo2(struct PatternData *user, const int direction,
                                 const GLfloat amount) {
  patternModelInitUnitsPosition(user->pattern);

  // g_print("amount: %f\n", amount);

  for (int i = 0; i < user->pattern->numHeight; i++) {
    for (int j = 0; j < user->pattern->numWidth; j++) {
      struct Rectangle *rect =
          user->pattern->units[(i * user->pattern->numWidth) + j];

      if (direction == 0) {
        if (i % 2 != 0) {
          continue;
        }

        rectangleMove(rect, amount, 0);
      } else if (direction == 1) {
        if (j % 2 != 0) {
          continue;
        }

        rectangleMove(rect, 0, amount);
      }

      // rectangleMove(rect, amount, 0);
    }
  }
}

static void patternOffsetAccumulate(struct PatternData *user,
                                    const int direction, const GLfloat amount) {
  patternModelInitUnitsPosition(user->pattern);

  // g_print("amount: %f\n", amount);

  if (direction == 0) {
    for (int i = 0; i < user->pattern->numHeight; i++) {
      for (int j = 0; j < user->pattern->numWidth; j++) {
        struct Rectangle *rect =
            user->pattern->units[(i * user->pattern->numWidth) + j];

        rectangleMove(rect, amount * i, 0);
      }
    }
  } else if (direction == 1) {
    for (int i = 0; i < user->pattern->numWidth; i++) {
      for (int j = 0; j < user->pattern->numHeight; j++) {
        struct Rectangle *rect =
            user->pattern->units[(j * user->pattern->numWidth) + i];

        rectangleMove(rect, 0, amount * i);
      }
    }
  }
}

static void patternOffsetRandom(struct PatternData *user, const int direction) {
  patternModelInitUnitsPosition(user->pattern);
  double width =
      (double)GL_HELPER_VIEWPORT_SIZE / (double)user->pattern->numWidth;

  if (direction == 0) {
    for (int i = 0; i < user->pattern->numHeight; i++) {
      double randNum = (double)rand() / (double)RAND_MAX;
      randNum = (randNum - 0.5f) * 2.0f;

      for (int j = 0; j < user->pattern->numWidth; j++) {
        struct Rectangle *rect =
            user->pattern->units[(i * user->pattern->numWidth) + j];

        rectangleMove(rect, width * randNum, 0);
      }
    }
  } else if (direction == 1) {
    for (int i = 0; i < user->pattern->numWidth; i++) {
      double randNum = (double)rand() / (double)RAND_MAX;
      randNum = (randNum - 0.5f) * 2.0f;

      for (int j = 0; j < user->pattern->numHeight; j++) {
        struct Rectangle *rect =
            user->pattern->units[(j * user->pattern->numWidth) + i];

        rectangleMove(rect, 0, width * randNum);
      }
    }
  }
}

void patternControlOffsetControlChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = (struct PatternData *)control->patternData;

  double width =
      (double)GL_HELPER_VIEWPORT_SIZE / (double)user->pattern->numWidth;
  double height =
      (double)GL_HELPER_VIEWPORT_SIZE / (double)user->pattern->numHeight;
  double offset = gtk_range_get_value(range);

  gint offsetType =
      gtk_combo_box_get_active(GTK_COMBO_BOX(control->offsetTypeComboBox));

  double offsetAmount = 0.0f;

  gint offsetControlType = gtk_combo_box_get_active(
      GTK_COMBO_BOX(control->offsetControlTypeComboBox));

  gint direction =
      gtk_combo_box_get_active(GTK_COMBO_BOX(control->offsetDirectionComboBox));

  if (offsetControlType == 0) {
    if (direction == 0) {
      offsetAmount = width / offset;
    } else if (direction == 1) {
      offsetAmount = height / offset;
    }
  } else if (offsetControlType == 1) {
    if (direction == 0) {
      offsetAmount = width * offset;
    } else if (direction == 1) {
      offsetAmount = height * offset;
    }
  }

  if (offsetType == 0) {
    patternOffsetModulo2(user, direction, offsetAmount);
  } else if (offsetType == 1) {
    patternOffsetAccumulate(user, direction, offsetAmount);
  } else if (offsetType == 2) {
    patternOffsetRandom(user, direction);
  }

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  // glFlush();

  gtk_gl_area_make_current(user->glArea);
  // patternModelSeamlessModelConstruct(user->pattern, user->glArea);
  constructSeamlessModel(user, colorMin, colorMax);

  setVBOData(&user->pattern->positionVBO, user->pattern->vertexCounts, 3,
             user->pattern->vertexPosition);

  setVBOData(&user->pattern->wireframeVBO, user->pattern->wireframeVertexCounts,
             3, user->pattern->vertexWireframe);

  // setVBOData(&user->pattern->seamlessModel->colorVBO,
  //           user->pattern->seamlessModel->vertexCounts, 3,
  //           user->pattern->seamlessModel->vertexColor);

  // fitSeamlessModelColor(user->pattern, colorMin, colorMax);

  g_print("offset control change\n");
  glFlush();
  // glFinish();

  //// queue openGL render
  gtk_gl_area_queue_render(user->glArea);
}

void patternControlOffsetTypeChanged(GtkComboBox *widget, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = (struct PatternData *)control->patternData;

  gtk_range_set_value(GTK_RANGE(control->offsetControlSlider), 0.0f);

  patternModelInitUnitsPosition(user->pattern);

  if (user->pattern->seamlessModel) {
    patternModelFree(user->pattern->seamlessModel);
    user->pattern->seamlessModel = NULL;
  }

  // offsetControlLabel = gtk_label_new("Offset Control: ");

  gint offsetType = gtk_combo_box_get_active(widget);

  if (offsetType == 2) {
    gtk_combo_box_set_active(GTK_COMBO_BOX(control->offsetControlTypeComboBox),
                             1);

    gtk_label_set_text(GTK_LABEL(control->offsetControlLabel),
                       "Random Offset Seed: ");

    gtk_widget_set_sensitive(GTK_WIDGET(control->offsetControlTypeLabel),
                             FALSE);

    gtk_widget_set_sensitive(GTK_WIDGET(control->offsetControlTypeComboBox),
                             FALSE);

    /* gint direction = gtk_combo_box_get_active( */
    /*     GTK_COMBO_BOX(control->offsetDirectionComboBox)); */

    /* patternOffsetRandom(user, direction); */

    /* double colorMin =
     * gtk_range_get_value(GTK_RANGE(control->colorMinSlider)); */
    /* double colorMax =
     * gtk_range_get_value(GTK_RANGE(control->colorMaxSlider)); */

    /* patternModelSeamlessModelConstruct(user->pattern, user->glArea); */

    /* fitSeamlessModelColor(user->pattern, colorMin, colorMax); */
  } else {
    gtk_combo_box_set_active(GTK_COMBO_BOX(control->offsetControlTypeComboBox),
                             0);

    gtk_widget_set_sensitive(GTK_WIDGET(control->offsetControlTypeLabel), TRUE);

    gtk_widget_set_sensitive(GTK_WIDGET(control->offsetControlTypeComboBox),
                             TRUE);

    gtk_label_set_text(GTK_LABEL(control->offsetControlLabel),
                       "Offset Control: ");
  }

  patternControlOffsetControlChanged(GTK_RANGE(control->offsetControlSlider),
                                     userData);

  /* setVBOData(&user->pattern->positionVBO, user->pattern->vertexCounts, 3, */
  /*            user->pattern->vertexPosition); */

  /* setVBOData(&user->pattern->wireframeVBO,
   * user->pattern->wireframeVertexCounts, */
  /*            3, user->pattern->vertexWireframe); */

  /* gtk_gl_area_queue_render(user->glArea); */
}

void patternControlOffsetControlTypeChanged(GtkComboBox *widget,
                                            void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = (struct PatternData *)control->patternData;

  gint offsetControlType = gtk_combo_box_get_active(widget);

  if (offsetControlType == 0) {
    gtk_range_set_range(GTK_RANGE(control->offsetControlSlider), 1.0f, 7.0f);
    gtk_range_set_increments(GTK_RANGE(control->offsetControlSlider), 1.0f,
                             1.0f);

    gtk_scale_set_digits(GTK_SCALE(control->offsetControlSlider), 0);

    gtk_range_set_value(GTK_RANGE(control->offsetControlSlider), 1.0f);
  } else if (offsetControlType == 1) {
    gtk_range_set_range(GTK_RANGE(control->offsetControlSlider), -1.0f, 1.0f);

    gtk_range_set_increments(GTK_RANGE(control->offsetControlSlider), 0.001f,
                             0.001f);

    gtk_scale_set_digits(GTK_SCALE(control->offsetControlSlider), 3);

    gtk_range_set_value(GTK_RANGE(control->offsetControlSlider), 0.0f);
  }

  gtk_gl_area_make_current(user->glArea);
  glFlush();
  gtk_gl_area_queue_render(user->glArea);
}

void patternControlOffsetDirectionChanged(GtkComboBox *widget, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;

  patternControlOffsetControlChanged(GTK_RANGE(control->offsetControlSlider),
                                     userData);
}

///////////////////////////////////////////////////////////////////////

void patternControlNumCpyChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = (struct PatternData *)control->patternData;

  unsigned int cpy = (unsigned int)gtk_range_get_value(range);

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  double scaleFactor = normalizeUVScaleRange(
      gtk_range_get_value(GTK_RANGE(control->uvScaleSlider)));

  gint patternIndex =
      gtk_combo_box_get_active(GTK_COMBO_BOX(control->patternTypeComboBox));

  struct PatternModel *pattern =
      patternModelNew(user->glArea, user->pattern->sizeX, user->pattern->sizeY,
                      cpy, patternIndex);

  patternModelFree(user->pattern);
  user->pattern = pattern;

  double width =
      (double)GL_HELPER_VIEWPORT_SIZE / (double)user->pattern->numWidth;
  double height =
      (double)GL_HELPER_VIEWPORT_SIZE / (double)user->pattern->numHeight;
  double offset = gtk_range_get_value(GTK_RANGE(control->offsetControlSlider));

  gint offsetType =
      gtk_combo_box_get_active(GTK_COMBO_BOX(control->offsetTypeComboBox));

  double offsetAmount = 0.0f;

  gint offsetControlType = gtk_combo_box_get_active(
      GTK_COMBO_BOX(control->offsetControlTypeComboBox));

  gint direction =
      gtk_combo_box_get_active(GTK_COMBO_BOX(control->offsetDirectionComboBox));

  if (offsetControlType == 0) {
    if (direction == 0) {
      offsetAmount = width / offset;
    } else if (direction == 1) {
      offsetAmount = height / offset;
    }
  } else if (offsetControlType == 1) {
    if (direction == 0) {
      offsetAmount = width * offset;
    } else if (direction == 1) {
      offsetAmount = height * offset;
    }
  }

  if (offsetType == 0) {
    patternOffsetModulo2(user, direction, offsetAmount);
  } else if (offsetType == 1) {
    patternOffsetAccumulate(user, direction, offsetAmount);
  } else if (offsetType == 2) {
    patternOffsetRandom(user, direction);
  }

  gboolean active = gtk_toggle_button_get_active(
      GTK_TOGGLE_BUTTON(control->uvRotateCheckButton));

  patternModelRandomizeUV(user->pattern);

  if (active) {
    patternModelRandomizeUVRotate(user->pattern);
  }

  // re-construct the color of whole pattern units
  patternModelRandomizeColor(user->pattern);
  patternModelFitColor(user->pattern, colorMin, colorMax);
  // patternModelRandomizeUV(user->pattern);
  patternModelScaleUV(user->pattern, scaleFactor);

  gtk_gl_area_make_current(user->glArea);
  // glFlush();

  constructSeamlessModel(user, colorMin, colorMax);
  // patternModelSeamlessModelConstruct(user->pattern, user->glArea);

  setVBOData(&user->pattern->positionVBO, user->pattern->vertexCounts, 3,
             user->pattern->vertexPosition);

  setVBOData(&user->pattern->wireframeVBO, user->pattern->wireframeVertexCounts,
             3, user->pattern->vertexWireframe);

  setVBOData(&user->pattern->colorVBO, user->pattern->vertexCounts, 3,
             user->pattern->vertexColor);

  setVBOData(&user->pattern->uvVBO, user->pattern->vertexCounts, 2,
             user->pattern->vertexUV);

  // setVBOData(&user->pattern->seamlessModel->colorVBO,
  //           user->pattern->seamlessModel->vertexCounts, 3,
  //           user->pattern->seamlessModel->vertexColor);

  // fitSeamlessModelColor(user->pattern, colorMin, colorMax);

  glFlush();

  // queue openGL render
  gtk_gl_area_queue_render(user->glArea);
}

void patternControlPatternTypeChanged(GtkComboBox *widget, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = (struct PatternData *)control->patternData;

  gtk_range_set_value(GTK_RANGE(control->numCpySlider), 1.0f);
  gtk_range_set_value(GTK_RANGE(control->offsetControlSlider), 0.0f);

  gtk_range_set_value(GTK_RANGE(control->colorMinSlider), 1.0f);
  gtk_range_set_value(GTK_RANGE(control->colorMaxSlider), 1.0f);

  gtk_range_set_value(GTK_RANGE(control->uvScaleSlider), 1.0f);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(control->uvRotateCheckButton),
                               FALSE);

  guint patternType =
      gtk_combo_box_get_active(GTK_COMBO_BOX(control->patternTypeComboBox));

  if (patternType == 0) {
    gtk_combo_box_set_active(GTK_COMBO_BOX(control->offsetDirectionComboBox),
                             0);
    gtk_widget_set_sensitive(control->offsetDirectionComboBox, TRUE);
  } else if (patternType == 1) {
    gtk_combo_box_set_active(GTK_COMBO_BOX(control->offsetDirectionComboBox),
                             1);
    gtk_widget_set_sensitive(control->offsetDirectionComboBox, FALSE);
  }

  // numCpyChanged(GTK_RANGE(control->numCpySlider), userData);

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  // double scaleFactor = normalizeUVScaleRange(
  //     gtk_range_get_value(GTK_RANGE(control->uvScaleSlider)));

  gint patternIndex =
      gtk_combo_box_get_active(GTK_COMBO_BOX(control->patternTypeComboBox));

  struct PatternModel *pattern =
      patternModelNew(user->glArea, user->pattern->sizeX, user->pattern->sizeY,
                      1, patternIndex);

  patternModelFree(user->pattern);
  user->pattern = pattern;

  patternModelRandomizeUV(user->pattern);

  // re-construct the color of whole pattern units
  patternModelRandomizeColor(user->pattern);
  patternModelFitColor(user->pattern, colorMin, colorMax);
  //  // patternModelRandomizeUV(user->pattern);
  //  patternModelScaleUV(user->pattern, scaleFactor);

  gtk_gl_area_make_current(user->glArea);

  //  patternModelSeamlessModelConstruct(user->pattern, user->glArea);
  //
  //  fitSeamlessModelColor(user->pattern, colorMin, colorMax);
  constructSeamlessModel(user, colorMin, colorMax);

  setVBOData(&user->pattern->positionVBO, user->pattern->vertexCounts, 3,
             user->pattern->vertexPosition);

  setVBOData(&user->pattern->wireframeVBO, user->pattern->wireframeVertexCounts,
             3, user->pattern->vertexWireframe);

  setVBOData(&user->pattern->colorVBO, user->pattern->vertexCounts, 3,
             user->pattern->vertexColor);

  setVBOData(&user->pattern->uvVBO, user->pattern->vertexCounts, 2,
             user->pattern->vertexUV);

  glFlush();

  // queue openGL render
  gtk_gl_area_queue_render(user->glArea);
}

void patternControlWireframeWidthChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = (struct PatternData *)control->patternData;

  gtk_gl_area_make_current(user->glArea);
  glFlush();

  gtk_gl_area_queue_render(user->glArea);
}

static void messageDialog(GtkWindow *parentWindow, const gchar *title,
                          const gchar *message) {
  GtkWidget *dialog;
  dialog = gtk_dialog_new_with_buttons(
      title, parentWindow, GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
      "Ok", GTK_RESPONSE_NONE, NULL);

  GtkWidget *content;
  content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

  GtkWidget *label;
  label = gtk_label_new(message);

  gtk_widget_set_margin_bottom(label, WIDGET_MARGIN * 2);
  gtk_widget_set_margin_top(label, WIDGET_MARGIN * 2);
  gtk_widget_set_margin_start(label, WIDGET_MARGIN * 2);
  gtk_widget_set_margin_end(label, WIDGET_MARGIN * 2);

  gtk_container_add(GTK_CONTAINER(content), label);
  gtk_widget_show_all(content);

  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

void patternControlEntryBufferInserted(GtkEntryBuffer *buffer, guint position,
                                       gchar *chars, guint nChars,
                                       void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  // struct PatternData *user = (struct PatternData *)control->patternData;

  gchar *text = (gchar *)gtk_entry_buffer_get_text(buffer);
  size_t textLen = strlen(text);

  if (*chars == '.') {
    messageDialog(control->mainWindow, "Warning!",
                  "The Entries only take integer without decimal value!");

    gchar newText[textLen];
    memset(newText, '\0', textLen);

    strncpy(newText, text, textLen - 1);

    gtk_entry_buffer_set_text(buffer, newText, textLen - 1);
  }
}

void patternControlDimensionButtonClicked(GtkButton *button, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = (struct PatternData *)control->patternData;

  gtk_range_set_value(GTK_RANGE(control->numCpySlider), 1.0f);
  gtk_range_set_value(GTK_RANGE(control->offsetControlSlider), 0.0f);

  gtk_range_set_value(GTK_RANGE(control->colorMinSlider), 1.0f);
  gtk_range_set_value(GTK_RANGE(control->colorMaxSlider), 1.0f);

  gtk_range_set_value(GTK_RANGE(control->uvScaleSlider), 1.0f);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(control->uvRotateCheckButton),
                               FALSE);

  const gchar *widthChar = gtk_entry_get_text(GTK_ENTRY(control->widthEntry));
  const gchar *heightChar = gtk_entry_get_text(GTK_ENTRY(control->heightEntry));

  if (strlen(widthChar) == 0 || strlen(heightChar) == 0) {
    messageDialog(control->mainWindow, "Warning!",
                  "Please enter the desired width and height for the pattern!");
    return;
  }

  unsigned int newWidth = (unsigned int)atof(widthChar);
  unsigned int newHeight = (unsigned int)atof(heightChar);

  if (newWidth == 0 || newHeight == 0) {
    messageDialog(control->mainWindow, "Warning!",
                  "The Width or Height Entry contain invalid characters!");

    gtk_entry_set_text(GTK_ENTRY(control->widthEntry), "");
    gtk_entry_set_text(GTK_ENTRY(control->heightEntry), "");

    return;
  }

  user->pattern->sizeX = newWidth;
  user->pattern->sizeY = newHeight;

  patternControlNumCpyChanged(GTK_RANGE(control->numCpySlider), userData);

  //  struct ControlData *control = (struct ControlData *)userData;
  //  struct PatternData *user = (struct PatternData *)control->patternData;

  // unsigned int cpy = (unsigned int)gtk_range_get_value(range);

  //  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  //  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));
  //
  //  // double scaleFactor = normalizeUVScaleRange(
  //  //     gtk_range_get_value(GTK_RANGE(control->uvScaleSlider)));
  //
  //  gint patternIndex =
  //      gtk_combo_box_get_active(GTK_COMBO_BOX(control->patternTypeComboBox));
  //
  //  struct PatternModel *pattern =
  //      patternModelNew(user->glArea, newWidth, newHeight, 1, patternIndex);
  //
  //  patternModelFree(user->pattern);
  //  user->pattern = pattern;
  //
  //  // double width = (double)GL_HELPER_VIEWPORT_SIZE /
  //  // (double)user->pattern->numWidth;
  //  // double height = (double)GL_HELPER_VIEWPORT_SIZE /
  //  // (double)user->pattern->numHeight;
  //
  //  // double offset =
  //  // gtk_range_get_value(GTK_RANGE(control->offsetControlSlider));
  //
  //  // g_print("height: %f\n", height);
  //
  //  //  gint offsetType =
  //  // gtk_combo_box_get_active(GTK_COMBO_BOX(control->offsetTypeComboBox));
  //  //
  //  //  double offsetAmount;
  //  //
  //  //  gint offsetControlType = gtk_combo_box_get_active(
  //  //      GTK_COMBO_BOX(control->offsetControlTypeComboBox));
  //  //
  //  //  gint direction =
  //  //
  //  gtk_combo_box_get_active(GTK_COMBO_BOX(control->offsetDirectionComboBox));
  //  //
  //  //  if (offsetControlType == 0) {
  //  //    if (direction == 0) {
  //  //      offsetAmount = width / offset;
  //  //    } else if (direction == 1) {
  //  //      offsetAmount = height / offset;
  //  //    }
  //  //  } else if (offsetControlType == 1) {
  //  //    if (direction == 0) {
  //  //      offsetAmount = width * offset;
  //  //    } else if (direction == 1) {
  //  //      offsetAmount = height * offset;
  //  //    }
  //  //  }
  //  //
  //  //  if (offsetType == 0) {
  //  //    patternOffsetModulo2(user, direction, offsetAmount);
  //  //  } else if (offsetType == 1) {
  //  //    patternOffsetAccumulate(user, direction, offsetAmount);
  //  //  } else if (offsetType == 2) {
  //  //    patternOffsetRandom(user, direction);
  //  //  }
  //
  //  //  gboolean active = gtk_toggle_button_get_active(
  //  //      GTK_TOGGLE_BUTTON(control->uvRotateCheckButton));
  //  //
  //  patternModelRandomizeUV(user->pattern);
  //  //
  //  //  if (active) {
  //  //    patternModelRandomizeUVRotate(user->pattern);
  //  //  }
  //
  //  // re-construct the color of whole pattern units
  //  patternModelRandomizeColor(user->pattern);
  //  patternModelFitColor(user->pattern, colorMin, colorMax);
  //  //  // patternModelRandomizeUV(user->pattern);
  //  //  patternModelScaleUV(user->pattern, scaleFactor);
  //
  //  patternModelSeamlessModelConstruct(user->pattern, user->glArea);
  //
  //  fitSeamlessModelColor(user->pattern, colorMin, colorMax);
  //
  //  setVBOData(&user->pattern->positionVBO, user->pattern->vertexCounts, 3,
  //             user->pattern->vertexPosition);
  //
  //  setVBOData(&user->pattern->wireframeVBO,
  //  user->pattern->wireframeVertexCounts,
  //             3, user->pattern->vertexWireframe);
  //
  //  setVBOData(&user->pattern->colorVBO, user->pattern->vertexCounts, 3,
  //             user->pattern->vertexColor);
  //
  //  setVBOData(&user->pattern->uvVBO, user->pattern->vertexCounts, 2,
  //             user->pattern->vertexUV);
  //
  //  // setVBOData(&user->pattern->seamlessModel->colorVBO,
  //  //           user->pattern->seamlessModel->vertexCounts, 3,
  //  //           user->pattern->seamlessModel->vertexColor);
  //
  //  // fitSeamlessModelColor(user->pattern, colorMin, colorMax);
  //
  //  // queue openGL render
  //  gtk_gl_area_queue_render(user->glArea);

  //  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  //  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));
  //
  //  double scaleFactor = normalizeUVScaleRange(
  //      gtk_range_get_value(GTK_RANGE(control->uvScaleSlider)));
  //
  //  gint patternIndex =
  //      gtk_combo_box_get_active(GTK_COMBO_BOX(control->patternTypeComboBox));
  //
  //  struct PatternModel *pattern =
  //      patternModelNew(user->glArea, width, height, 1, patternIndex);
  //
  //  patternModelFree(user->pattern);
  //
  //  user->pattern = pattern;
  //
  //  // only re-generate GL color data and fit it into new range
  //  patternModelRandomizeColor(user->pattern);
  //  patternModelFitColor(user->pattern, colorMin, colorMax);
  //  patternModelRandomizeUV(user->pattern);
  //  patternModelScaleUV(user->pattern, scaleFactor);
  //
  //  setVBOData(&user->pattern->colorVBO, user->pattern->vertexCounts, 3,
  //             user->pattern->vertexColor);
  //
  //  setVBOData(&user->pattern->uvVBO, user->pattern->vertexCounts, 2,
  //             user->pattern->vertexUV);
  //
  //  gtk_gl_area_queue_render(user->glArea);
}

static gchar *getTextureFile(GtkWindow *parentWindow,
                             GtkFileChooserAction action, const gchar *title,
                             const gchar *acceptText, const gchar *cancelText) {
  GtkWidget *dialog = gtk_file_chooser_dialog_new(
      title, parentWindow, action, cancelText, GTK_RESPONSE_CANCEL, acceptText,
      GTK_RESPONSE_ACCEPT, NULL);

  gchar *fileName = NULL;
  gint response = gtk_dialog_run(GTK_DIALOG(dialog));
  if (response == GTK_RESPONSE_ACCEPT) {
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
    fileName = gtk_file_chooser_get_filename(chooser);
  }

  gtk_widget_destroy(dialog);
  return fileName;
}

void patternControlTextureInfoButtonClicked(GtkButton *button, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = (struct PatternData *)control->patternData;

  gchar *filePath =
      getTextureFile(control->mainWindow, GTK_FILE_CHOOSER_ACTION_OPEN,
                     "Open File", "Open", "Cancel");
  if (filePath == NULL) {
    return;
  }

  gchar *baseName = pathGetBase(filePath);

  if (filePath) {
    user->textureFile = filePath;
    gtk_label_set_text(GTK_LABEL(control->textureInfoLabel), baseName);
    free(baseName);

    gtk_gl_area_make_current(user->glArea);
    glUseProgram(user->shaderProgram);

    //GLuint *tex = &user->tex;
    //glDeleteTextures(1, tex);

    gboolean result;
    result = patternDataInitTexMap(user);
    if (result != TRUE) {
      messageDialog(control->mainWindow, "Warning!", "Invalid Texture File!");
    }

    glFlush();

    gtk_gl_area_queue_render(user->glArea);
  }
}

void patternControlRenderPathInfoButtonClicked(GtkButton *button,
                                               void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = (struct PatternData *)control->patternData;

  gchar *filePath =
      getTextureFile(control->mainWindow, GTK_FILE_CHOOSER_ACTION_SAVE,
                     "Save File", "Save as", "Cancel");
  if (filePath == NULL) {
    return;
  }

  user->renderPath = filePath;

  gchar *baseName = pathGetBase(filePath);
  gtk_label_set_text(GTK_LABEL(control->renderPathInfoLabel), baseName);
  free(baseName);
}

static void writePngImage(unsigned char *image, const char *fileName,
                          const unsigned int renderSize) {
  unsigned char *lastRow = image + (renderSize * 3 * (renderSize - 1));
  if (!stbi_write_png(fileName, renderSize, renderSize, 3, lastRow,
                      -3 * renderSize)) {
    g_print("error writing png file\n");
  }
}

void patternControlRenderButtonClicked(GtkButton *button, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = (struct PatternData *)control->patternData;

  const gchar *renderSizeChar =
      gtk_entry_get_text(GTK_ENTRY(control->renderSizeEntry));

  if (strlen(renderSizeChar) == 0) {
    messageDialog(control->mainWindow, "Warning!",
                  "Please enter the desired render size!");
    return;
  }

  unsigned int renderSize = (unsigned int)atof(renderSizeChar);

  if (renderSize == 0) {
    messageDialog(control->mainWindow, "Warning!",
                  "The Render Size Entry contain invalid characters!");

    gtk_entry_set_text(GTK_ENTRY(control->renderSizeEntry), "");
    return;
  }

  if (user->renderPath == NULL) {
    messageDialog(control->mainWindow, "Warning!",
                  "Please specify a valid render path!");
    return;
  }

  gboolean renderColor = gtk_toggle_button_get_active(
      GTK_TOGGLE_BUTTON(control->renderColorCheckButton));

  gboolean renderWireframe = gtk_toggle_button_get_active(
      GTK_TOGGLE_BUTTON(control->renderWireframeCheckButton));

  if (renderColor == FALSE && renderWireframe == FALSE) {
    messageDialog(control->mainWindow, "Warning!",
                  "Please specify the desired image to render!");
    return;
  }

  gchar *pathBase = pathRemoveExt(user->renderPath);
  gchar *colorPathBase = charJoin(pathBase, "color", "_");
  gchar *colorPath = charJoin(colorPathBase, ".png", "");
  free(colorPathBase);

  gchar *wireframePathBase = charJoin(pathBase, "wireframe", "_");
  gchar *wireframePath = charJoin(wireframePathBase, ".png", "");
  free(wireframePathBase);

  free(pathBase);

  g_print("Start rendering the texture...\n");
  g_print("Render Path Color: %s\n", colorPath);
  g_print("Render Path Wireframe: %s\n", wireframePath);
  g_print("Render Size: %d\n", renderSize);

  gtk_gl_area_make_current(user->glArea);

  glFlush();

  GLuint fb;
  glGenFramebuffers(1, &fb);
  glBindFramebuffer(GL_FRAMEBUFFER, fb);

  GLuint colorTex;
  glGenTextures(1, &colorTex);
  glBindTexture(GL_TEXTURE_2D, colorTex);

  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, renderSize, renderSize, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, NULL);

  glBindFramebuffer(GL_FRAMEBUFFER, fb);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         colorTex, 0);

  GLenum drawBufs[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, drawBufs);

  GLenum status;
  status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if (status != GL_FRAMEBUFFER_COMPLETE) {
    g_print("Failed to generate the framebuffer!\n");
  }

  gtk_gl_area_make_current(user->glArea);
  glUseProgram(user->shaderProgram);

  glViewport(0, 0, renderSize, renderSize);
  glScissor(0, 0, renderSize, renderSize);

  glBindFramebuffer(GL_FRAMEBUFFER, fb);

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  unsigned char *colorImage = NULL;
  unsigned char *wireframeImage = NULL;

  glFlush();

  if (user->pattern != NULL) {
    glUseProgram(user->shaderProgram);
    glBindTexture(GL_TEXTURE_2D, user->tex);

    if (renderColor) {
      user->wireframeDraw = 0;
      glUniform1i(user->wireframeDrawUniformLoc, user->wireframeDraw);

      glBindVertexArray(user->pattern->vao);
      glDrawArrays(GL_TRIANGLES, 0, user->pattern->vertexCounts);

      if (user->pattern->seamlessModel != NULL) {
        user->wireframeDraw = 0;
        glBindVertexArray(user->pattern->seamlessModel->vao);
        glDrawArrays(GL_TRIANGLES, 0,
                     user->pattern->seamlessModel->vertexCounts);
      }

      colorImage = (unsigned char *)DEFENSE_MALLOC(renderSize * renderSize * 3,
                                                   mallocFailAbort, NULL);

      glBindFramebuffer(GL_FRAMEBUFFER, fb);
      glReadPixels(0, 0, renderSize, renderSize, GL_RGB, GL_UNSIGNED_BYTE,
                   colorImage);

      glClearColor(0.0, 0.0, 0.0, 1.0);
      glClear(GL_COLOR_BUFFER_BIT);
    }

    if (renderWireframe) {
      user->wireframeDraw = 1;
      glBindVertexArray(user->pattern->vao);
      glUniform1i(user->wireframeDrawUniformLoc, user->wireframeDraw);
      // glDrawArrays(GL_LINES, 0, user->pattern->vertexCounts);
      glDrawArrays(GL_LINES, 0, user->pattern->wireframeVertexCounts);

      if (user->pattern->seamlessModel != NULL) {
        glBindVertexArray(user->pattern->seamlessModel->vao);
        glUniform1i(user->wireframeDrawUniformLoc, user->wireframeDraw);
        glDrawArrays(GL_LINES, 0,
                     user->pattern->seamlessModel->wireframeVertexCounts);
      }

      wireframeImage = (unsigned char *)DEFENSE_MALLOC(
          renderSize * renderSize * 3, mallocFailAbort, NULL);

      glBindFramebuffer(GL_FRAMEBUFFER, fb);
      glReadPixels(0, 0, renderSize, renderSize, GL_RGB, GL_UNSIGNED_BYTE,
                   wireframeImage);

      glClearColor(0.0, 0.0, 0.0, 1.0);
      glClear(GL_COLOR_BUFFER_BIT);
    }
  }

  if (colorImage != NULL) {
    writePngImage(colorImage, colorPath, renderSize);
  }

  if (wireframeImage != NULL) {
    writePngImage(wireframeImage, wireframePath, renderSize);
  }

  gtk_gl_area_make_current(user->glArea);

  glDeleteTextures(1, &colorTex);

  glViewport(0, 0, WINDOW_HEIGHT, WINDOW_HEIGHT);
  glScissor(0, 0, WINDOW_HEIGHT, WINDOW_HEIGHT);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDeleteFramebuffers(1, &fb);

  // gtk_gl_area_make_current(user->glArea);

  glFlush();

  gtk_gl_area_queue_render(user->glArea);

  g_print("Finish rendering!\n");
}

gboolean patternControlGlRender(GtkGLArea *area, GdkGLContext *context,
                                void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = (struct PatternData *)control->patternData;

  gtk_gl_area_make_current(user->glArea);

  glFlush();

  glViewport(0, 0, WINDOW_HEIGHT, WINDOW_HEIGHT);
  glScissor(0, 0, WINDOW_HEIGHT, WINDOW_HEIGHT);

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(user->shaderProgram);
  glBindTexture(GL_TEXTURE_2D, user->tex);

  if (user->pattern != NULL) {
    user->wireframeDraw = 0;
    glUniform1i(user->wireframeDrawUniformLoc, user->wireframeDraw);

    glBindVertexArray(user->pattern->vao);
    glDrawArrays(GL_TRIANGLES, 0, user->pattern->vertexCounts);

    if (user->pattern->seamlessModel != NULL) {
      user->wireframeDraw = 0;
      glBindVertexArray(user->pattern->seamlessModel->vao);
      glDrawArrays(GL_TRIANGLES, 0, user->pattern->seamlessModel->vertexCounts);
    }

    if (user->wireframeSwitch == 1) {
      double newWidth =
          gtk_range_get_value(GTK_RANGE(control->wireframeWidthSlider)) * 1;
      glLineWidth(newWidth);

      user->wireframeDraw = 1;
      glBindVertexArray(user->pattern->vao);
      glUniform1i(user->wireframeDrawUniformLoc, user->wireframeDraw);
      glDrawArrays(GL_LINES, 0, user->pattern->wireframeVertexCounts);

      if (user->pattern->seamlessModel != NULL) {
        glBindVertexArray(user->pattern->seamlessModel->vao);
        glUniform1i(user->wireframeDrawUniformLoc, user->wireframeDraw);
        glDrawArrays(GL_LINES, 0,
                     user->pattern->seamlessModel->wireframeVertexCounts);
      }
    }
  }

  glFlush();

  return TRUE;
}
