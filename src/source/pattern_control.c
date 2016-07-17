#include "../header/pattern_control.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../../third_party/stb/stb_image_write.h"

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

static struct PatternData *patternDataNew(GtkGLArea *glArea,
                                          GLuint shaderProgram,
                                          struct PatternModel *pattern) {
  struct PatternData *re =
      (struct PatternData *)defenseMalloc(sizeof(struct PatternData));

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

  re->wireframeColor = defenseMalloc(3 * sizeof(GLfloat));
  re->wireframeColor[0] = 1.0f;
  re->wireframeColor[1] = 1.0f;
  re->wireframeColor[2] = 1.0f;

  gtk_gl_area_make_current(glArea);
  glUseProgram(shaderProgram);
  glUniform3fv(re->wireframeColorUniformLoc, 1, re->wireframeColor);
  // patternDataInitTexMap(re);

  return re;
}

struct ControlData {
  struct PatternData *patternData;

  GtkWindow *mainWindow;
  GtkWidget *controlBox;

  GtkWidget *patternTypeComboBox;

  GtkWidget *widthEntry;
  GtkWidget *heightEntry;

  GtkWidget *numCpySlider;

  //////////////////////////////////////////////////////////////

  GtkWidget *uniqueControlStack;

  //////////////////////////////////////////////////////////////

  GtkWidget *offsetTypeComboBox;

  //  GtkWidget *leanCheckButton;
  //  GtkWidget *leanControlSlider;

  GtkWidget *offsetDirectionComboBox;

  GtkWidget *offsetControlTypeLabel;
  GtkWidget *offsetControlTypeComboBox;

  GtkWidget *offsetControlLabel;
  GtkWidget *offsetControlSlider;

  //////////////////////////////////////////////////////////////

  GtkWidget *wireframeColorChooser;
  GtkWidget *wireframeWidthSlider;

  GtkWidget *colorSeedSlider;
  GtkWidget *colorMinSlider;
  GtkWidget *colorMaxSlider;

  GtkWidget *textureInfoLabel;

  GtkWidget *uvScaleSlider;
  GtkWidget *uvRotateCheckButton;

  GtkWidget *renderPathInfoLabel;

  GtkWidget *renderColorCheckButton;
  GtkWidget *renderWireframeCheckButton;

  GtkWidget *renderSizeEntry;
  GtkWidget *renderButton;
};

static void wireframeSwitchToggled(GtkToggleButton *toggleButton,
                                   void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

  user->wireframeSwitch = !user->wireframeSwitch;
  gtk_gl_area_queue_render(user->glArea);
}

static void wireframeColorSet(GtkColorButton *widget, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

  GdkRGBA color;
  gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(widget), &color);

  user->wireframeColor[0] = color.red;
  user->wireframeColor[1] = color.green;
  user->wireframeColor[2] = color.blue;

  gtk_gl_area_make_current(user->glArea);
  glUseProgram(user->shaderProgram);
  glUniform3fv(user->wireframeColorUniformLoc, 1, user->wireframeColor);

  gtk_gl_area_queue_render(user->glArea);
}

static void fitSeamlessModelColor(struct PatternModel *pattern, double colorMin,
                                  double colorMax) {
  if (pattern->seamlessModel) {
    patternModelFitColor(pattern->seamlessModel, colorMin, colorMax);
    setVBOData(&pattern->seamlessModel->colorVBO,
               pattern->seamlessModel->vertexCounts, 3,
               pattern->seamlessModel->vertexColor);
  }
}

static void colorSeedChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  // re-construct the color of whole pattern units
  patternModelRandomizeColor(user->pattern);

  patternModelSeamlessModelConstruct(user->pattern, user->glArea);

  patternModelFitColor(user->pattern, colorMin, colorMax);

  // directly update the specific memory allocated for the data
  setVBOData(&user->pattern->colorVBO, user->pattern->vertexCounts, 3,
             user->pattern->vertexColor);

  fitSeamlessModelColor(user->pattern, colorMin, colorMax);

  // queue openGL render
  gtk_gl_area_queue_render(user->glArea);
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

static void randUVSeedChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  gboolean active = gtk_toggle_button_get_active(
      GTK_TOGGLE_BUTTON(control->uvRotateCheckButton));

  patternModelRandomizeUV(user->pattern);

  if (active) {
    patternModelRandomizeUVRotate(user->pattern);
  }

  double scaleRangeValue =
      gtk_range_get_value(GTK_RANGE(control->uvScaleSlider));
  double scaleFactor = normalizeUVScaleRange(scaleRangeValue);

  patternModelScaleUV(user->pattern, scaleFactor);

  patternModelSeamlessModelConstruct(user->pattern, user->glArea);

  setVBOData(&user->pattern->uvVBO, user->pattern->vertexCounts, 2,
             user->pattern->vertexUV);

  fitSeamlessModelColor(user->pattern, colorMin, colorMax);

  gtk_gl_area_queue_render(user->glArea);
}

static void uvScaleChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

  double rangeValue = gtk_range_get_value(GTK_RANGE(range));
  double scaleFactor = normalizeUVScaleRange(rangeValue);

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  patternModelScaleUV(user->pattern, scaleFactor);

  patternModelSeamlessModelConstruct(user->pattern, user->glArea);

  setVBOData(&user->pattern->uvVBO, user->pattern->vertexCounts, 2,
             user->pattern->vertexUV);

  fitSeamlessModelColor(user->pattern, colorMin, colorMax);

  gtk_gl_area_queue_render(user->glArea);
}

static double normalizeUVRotateRange(double rangeValue) {
  rangeValue = (rangeValue * 2) - 1;
  double rotateFactor = rangeValue * 360;

  return rotateFactor;
}

static void uvRotateToggled(GtkToggleButton *toggleButton, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  gboolean active = gtk_toggle_button_get_active(toggleButton);

  patternModelRandomizeUV(user->pattern);

  if (active) {
    patternModelRandomizeUVRotate(user->pattern);
  }

  double scaleRangeValue =
      gtk_range_get_value(GTK_RANGE(control->uvScaleSlider));
  double scaleFactor = normalizeUVScaleRange(scaleRangeValue);

  patternModelScaleUV(user->pattern, scaleFactor);

  patternModelSeamlessModelConstruct(user->pattern, user->glArea);

  fitSeamlessModelColor(user->pattern, colorMin, colorMax);

  setVBOData(&user->pattern->uvVBO, user->pattern->vertexCounts, 2,
             user->pattern->vertexUV);

  gtk_gl_area_queue_render(user->glArea);
}

///////////////////////////////////////////////////////////////////////

static void patternOffsetModulo2(struct PatternData *user, const int direction,
                                 const GLfloat amount) {
  patternModelInitUnitsPosition(user->pattern);

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
  double width = (GLfloat)__GL_VIEWPORT / user->pattern->numWidth;

  if (direction == 0) {
    for (int i = 0; i < user->pattern->numHeight; i++) {
      double randNum = (GLfloat)rand() / (GLfloat)RAND_MAX;
      randNum = (randNum * 2) - 1;

      for (int j = 0; j < user->pattern->numWidth; j++) {
        struct Rectangle *rect =
            user->pattern->units[(i * user->pattern->numWidth) + j];

        rectangleMove(rect, width * randNum, 0);
      }
    }
  } else if (direction == 1) {
    for (int i = 0; i < user->pattern->numWidth; i++) {
      double randNum = (GLfloat)rand() / (GLfloat)RAND_MAX;
      randNum = (randNum * 2) - 1;

      for (int j = 0; j < user->pattern->numHeight; j++) {
        struct Rectangle *rect =
            user->pattern->units[(j * user->pattern->numWidth) + i];

        rectangleMove(rect, 0, width * randNum);
      }
    }
  }
}

static void offsetTypeChanged(GtkComboBox *widget, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

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

  gint direction =
      gtk_combo_box_get_active(GTK_COMBO_BOX(control->offsetDirectionComboBox));

    patternOffsetRandom(user, direction);

    double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
    double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

    patternModelSeamlessModelConstruct(user->pattern, user->glArea);

    fitSeamlessModelColor(user->pattern, colorMin, colorMax);
  } else {
    gtk_combo_box_set_active(GTK_COMBO_BOX(control->offsetControlTypeComboBox),
                             0);

    gtk_widget_set_sensitive(GTK_WIDGET(control->offsetControlTypeLabel), TRUE);

    gtk_widget_set_sensitive(GTK_WIDGET(control->offsetControlTypeComboBox),
                             TRUE);

    gtk_label_set_text(GTK_LABEL(control->offsetControlLabel),
                       "Offset Control: ");
  }

  setVBOData(&user->pattern->positionVBO, user->pattern->vertexCounts, 3,
             user->pattern->vertexPosition);

  setVBOData(&user->pattern->wireframeVBO, user->pattern->wireframeVertexCounts,
             3, user->pattern->vertexWireframe);

  gtk_gl_area_queue_render(user->glArea);
}

static void offsetControlTypeChanged(GtkComboBox *widget, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

  gint offsetControlType = gtk_combo_box_get_active(widget);
  // g_print("offset control type: %d\n", offsetControlType);

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

  gtk_gl_area_queue_render(user->glArea);
}

static void offsetControlChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

  double width = (GLfloat)__GL_VIEWPORT / user->pattern->numWidth;
  double height = (GLfloat)__GL_VIEWPORT / user->pattern->numHeight;
  double offset = gtk_range_get_value(range);

  gint offsetType =
      gtk_combo_box_get_active(GTK_COMBO_BOX(control->offsetTypeComboBox));

  double offsetAmount;

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

  patternModelSeamlessModelConstruct(user->pattern, user->glArea);

  setVBOData(&user->pattern->positionVBO, user->pattern->vertexCounts, 3,
             user->pattern->vertexPosition);

  setVBOData(&user->pattern->wireframeVBO, user->pattern->wireframeVertexCounts,
             3, user->pattern->vertexWireframe);

  fitSeamlessModelColor(user->pattern, colorMin, colorMax);

  // queue openGL render
  gtk_gl_area_queue_render(user->glArea);
}

static void offsetDirectionChanged(GtkComboBox *widget, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;

  offsetControlChanged(GTK_RANGE(control->offsetControlSlider), userData);
}

///////////////////////////////////////////////////////////////////////

static void numCpyChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

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

  double width = (GLfloat)__GL_VIEWPORT / user->pattern->numWidth;
  double height = (GLfloat)__GL_VIEWPORT / user->pattern->numHeight;
  double offset = gtk_range_get_value(GTK_RANGE(control->offsetControlSlider));

  gint offsetType =
      gtk_combo_box_get_active(GTK_COMBO_BOX(control->offsetTypeComboBox));

  double offsetAmount;

  gint offsetControlType = gtk_combo_box_get_active(
      GTK_COMBO_BOX(control->offsetControlTypeComboBox));

  // if (offsetControlType == 0) {
  //  offsetAmount = width / offset;
  //} else if (offsetControlType == 1) {
  //  offsetAmount = width * offset;
  //}

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

  patternModelSeamlessModelConstruct(user->pattern, user->glArea);

  setVBOData(&user->pattern->positionVBO, user->pattern->vertexCounts, 3,
             user->pattern->vertexPosition);

  setVBOData(&user->pattern->wireframeVBO, user->pattern->wireframeVertexCounts,
             3, user->pattern->vertexWireframe);

  setVBOData(&user->pattern->colorVBO, user->pattern->vertexCounts, 3,
             user->pattern->vertexColor);

  setVBOData(&user->pattern->uvVBO, user->pattern->vertexCounts, 2,
             user->pattern->vertexUV);

  fitSeamlessModelColor(user->pattern, colorMin, colorMax);

  // queue openGL render
  gtk_gl_area_queue_render(user->glArea);
}

static void colorRangeChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  // only re-generate GL color data and fit it into new range
  patternModelFitColor(user->pattern, colorMin, colorMax);

  setVBOData(&user->pattern->colorVBO, user->pattern->vertexCounts, 3,
             user->pattern->vertexColor);

  fitSeamlessModelColor(user->pattern, colorMin, colorMax);

  // queue openGL render
  gtk_gl_area_queue_render(user->glArea);
}

static void wireframeWidthChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

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

static void entryBufferInserted(GtkEntryBuffer *buffer, guint position,
                                gchar *chars, guint nChars, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

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

static void dimensionButtonClicked(GtkButton *button, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

  gtk_range_set_value(GTK_RANGE(control->numCpySlider), 1.0f);
  gtk_range_set_value(GTK_RANGE(control->offsetControlSlider), 0.0f);

  const gchar *widthChar = gtk_entry_get_text(GTK_ENTRY(control->widthEntry));
  const gchar *heightChar = gtk_entry_get_text(GTK_ENTRY(control->heightEntry));

  if (strlen(widthChar) == 0 || strlen(heightChar) == 0) {
    messageDialog(control->mainWindow, "Warning!",
                  "Please enter the desired width and height for the pattern!");
    return;
  }

  unsigned int width = (unsigned int)atof(widthChar);
  unsigned int height = (unsigned int)atof(heightChar);

  if (width == 0 || height == 0) {
    messageDialog(control->mainWindow, "Warning!",
                  "The Width or Height Entry contain invalid characters!");

    gtk_entry_set_text(GTK_ENTRY(control->widthEntry), "");
    gtk_entry_set_text(GTK_ENTRY(control->heightEntry), "");

    return;
  }

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  double scaleFactor = normalizeUVScaleRange(
      gtk_range_get_value(GTK_RANGE(control->uvScaleSlider)));

  gint patternIndex =
      gtk_combo_box_get_active(GTK_COMBO_BOX(control->patternTypeComboBox));

  struct PatternModel *pattern =
      patternModelNew(user->glArea, width, height, 1, patternIndex);

  patternModelFree(user->pattern);

  user->pattern = pattern;

  // only re-generate GL color data and fit it into new range
  patternModelRandomizeColor(user->pattern);
  patternModelFitColor(user->pattern, colorMin, colorMax);
  patternModelRandomizeUV(user->pattern);
  patternModelScaleUV(user->pattern, scaleFactor);

  setVBOData(&user->pattern->colorVBO, user->pattern->vertexCounts, 3,
             user->pattern->vertexColor);

  setVBOData(&user->pattern->uvVBO, user->pattern->vertexCounts, 2,
             user->pattern->vertexUV);

  gtk_gl_area_queue_render(user->glArea);
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

static void textureInfoButtonClicked(GtkButton *button, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

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

    GLuint *tex = &user->tex;
    glDeleteTextures(1, tex);

    gboolean result;
    result = patternDataInitTexMap(user);
    if (result != TRUE) {
      messageDialog(control->mainWindow, "Warning!", "Invalid Texture File!");
    }

    gtk_gl_area_queue_render(user->glArea);
  }
}

static void renderPathInfoButtonClicked(GtkButton *button, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

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

static void renderButtonClicked(GtkButton *button, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

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

  GLuint fb;
  glGenFramebuffers(1, &fb);
  glBindFramebuffer(GL_FRAMEBUFFER, fb);

  GLuint colorTex;
  glGenTextures(1, &colorTex);
  glBindTexture(GL_TEXTURE_2D, colorTex);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
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

  if (user->pattern != NULL) {
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

      colorImage = (unsigned char *)defenseMalloc(renderSize * renderSize * 3);

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

      wireframeImage =
          (unsigned char *)defenseMalloc(renderSize * renderSize * 3);

      glBindFramebuffer(GL_FRAMEBUFFER, fb);
      glReadPixels(0, 0, renderSize, renderSize, GL_RGB, GL_UNSIGNED_BYTE,
                   wireframeImage);
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

  gtk_gl_area_make_current(user->glArea);
  gtk_gl_area_queue_render(user->glArea);

  g_print("Finish rendering!\n");
}

static void addSeparator(GtkContainer *container,
                         const GtkOrientation orientation,
                         const unsigned int width, const unsigned int height) {
  GtkWidget *separator = gtk_separator_new(orientation);
  gtk_widget_set_size_request(separator, width, height);
  gtk_container_add(container, separator);
}

static struct ControlData *initControl(GtkWindow *mainWindow,
                                       GtkContainer *container, void *user) {
  // initialize random generator here instead of every time the value changed
  srand(time(NULL));

  // alloc memory for controlData used by signal function
  struct ControlData *control = defenseMalloc(sizeof(struct ControlData));

  control->patternData = (struct PatternData *)user;

  control->mainWindow = mainWindow;

  GtkWidget *scrollWindow;

  GtkWidget *controlBox;

  GtkWidget *patternTypeComboBox;

  GtkWidget *dimensionBox;

  GtkWidget *widthLabel;
  GtkWidget *widthEntry;
  GtkEntryBuffer *widthEntryBuffer;

  GtkWidget *heightLabel;
  GtkWidget *heightEntry;
  GtkEntryBuffer *heightEntryBuffer;

  GtkWidget *dimensionButton;
  GtkWidget *numCpyLabel;
  GtkWidget *numCpySlider;

  GtkWidget *wireframeColorLabel;
  GtkWidget *wireframeSwitch;
  GtkWidget *wireframeColorChooser;

  GtkWidget *wireframeWidthLabel;
  GtkWidget *wireframeWidthSlider;

  //////////////////////////////////////////////////////////////

  GtkWidget *uniqueControlStack;

  //////////////////////////////////////////////////////////////

  GtkWidget *pattern01Box;

  //  GtkWidget *leanCheckButton;
  //  GtkWidget *leanLabel;
  //  GtkWidget *leanControlSlider;

  GtkWidget *offsetDirectionLabel;
  GtkWidget *offsetDirectionComboBox;

  GtkWidget *offsetTypeLabel;
  GtkWidget *offsetTypeComboBox;

  GtkWidget *offsetControlTypeLabel;
  GtkWidget *offsetControlTypeComboBox;

  GtkWidget *offsetControlLabel;
  GtkWidget *offsetControlSlider;

  //////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////

  GtkWidget *colorSeedLabel;
  GtkWidget *colorSeedSlider;

  GtkWidget *colorMinLabel;
  GtkWidget *colorMinSlider;

  GtkWidget *colorMaxLabel;
  GtkWidget *colorMaxSlider;

  GtkWidget *textureLabel;
  GtkWidget *textureInfoLabel;
  GtkWidget *textureInfoButton;

  GtkWidget *randUVSeedLabel;
  GtkWidget *randUVSeedSlider;

  GtkWidget *uvScaleLabel;
  GtkWidget *uvScaleSlider;

  GtkWidget *uvRotateCheckButton;

  GtkWidget *renderPathLabel;
  GtkWidget *renderPathInfoLabel;
  GtkWidget *renderPathInfoButton;

  GtkWidget *renderColorCheckButton;
  GtkWidget *renderWireframeCheckButton;

  GtkWidget *renderSizeBox;
  GtkWidget *renderSizeLabel;
  GtkWidget *renderSizeEntry;
  GtkEntryBuffer *renderSizeEntryBuffer;

  GtkWidget *renderButton;

  scrollWindow = gtk_scrolled_window_new(NULL, NULL);
  // set the horizontal policy of scrolled window to never to make the
  // underlying
  // widget shrink a little bit on the right end margin

  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollWindow),
                                 GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  gtk_widget_set_vexpand(scrollWindow, TRUE);

  gtk_container_add(container, scrollWindow);

  controlBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, BOX_SPACE);
  gtk_container_add(GTK_CONTAINER(scrollWindow), controlBox);

  gtk_widget_set_margin_end(controlBox, SCROLLBAR_MARGIN);

  control->controlBox = controlBox;

  ///////////////////////////////////////////////////////////////////////

  gchar *pattern01 = "Pattern01";
  gchar *pattern02 = "Pattern02";
  gchar *pattern03 = "Pattern03";

  patternTypeComboBox = gtk_combo_box_text_new();
  gtk_widget_set_size_request(patternTypeComboBox, CONTROL_BOX_WIDTH, 35);
  gtk_container_add(GTK_CONTAINER(controlBox), patternTypeComboBox);

  control->patternTypeComboBox = patternTypeComboBox;

  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(patternTypeComboBox),
                                 pattern01);

  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(patternTypeComboBox),
                                 pattern02);

  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(patternTypeComboBox),
                                 pattern03);

  ///////////////////////////////////////////////////////////////////////

  gtk_combo_box_set_active(GTK_COMBO_BOX(patternTypeComboBox), 0);

  addSeparator(GTK_CONTAINER(controlBox), GTK_ORIENTATION_HORIZONTAL,
               CONTROL_BOX_WIDTH, SEPARATOR_WIDTH);

  dimensionBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, BOX_SPACE * 2);
  gtk_container_add(GTK_CONTAINER(controlBox), dimensionBox);

  gtk_widget_set_halign(dimensionBox, GTK_ALIGN_CENTER);

  widthLabel = gtk_label_new("Width: ");
  gtk_container_add(GTK_CONTAINER(dimensionBox), widthLabel);

  widthEntryBuffer = gtk_entry_buffer_new("", 0);

  widthEntry = gtk_entry_new_with_buffer(widthEntryBuffer);
  gtk_container_add(GTK_CONTAINER(dimensionBox), widthEntry);
  control->widthEntry = widthEntry;

  heightLabel = gtk_label_new("Height: ");
  gtk_container_add(GTK_CONTAINER(dimensionBox), heightLabel);

  heightEntryBuffer = gtk_entry_buffer_new("", 0);

  heightEntry = gtk_entry_new_with_buffer(heightEntryBuffer);
  gtk_container_add(GTK_CONTAINER(dimensionBox), heightEntry);
  control->heightEntry = heightEntry;

  dimensionButton = gtk_button_new_with_label("Generate!");
  gtk_container_add(GTK_CONTAINER(controlBox), dimensionButton);

  numCpyLabel = gtk_label_new("Number copys: ");
  gtk_widget_set_halign(numCpyLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), numCpyLabel);

  numCpySlider =
      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1.0f, 20.0f, 1.00f);
  control->numCpySlider = numCpySlider;

  gtk_container_add(GTK_CONTAINER(controlBox), numCpySlider);

  addSeparator(GTK_CONTAINER(controlBox), GTK_ORIENTATION_HORIZONTAL,
               CONTROL_BOX_WIDTH, SEPARATOR_WIDTH);

  ///////////////////////////////////////////////////////////////////////

  uniqueControlStack = gtk_stack_new();
  gtk_container_add(GTK_CONTAINER(controlBox), uniqueControlStack);

  ///////////////////////////////////////////////////////////////////////

  pattern01Box = gtk_box_new(GTK_ORIENTATION_VERTICAL, BOX_SPACE);
  gtk_stack_add_named(GTK_STACK(uniqueControlStack), pattern01Box, pattern01);

  //  leanCheckButton = gtk_check_button_new_with_label(" Rectangle Lean");
  //  gtk_container_add(GTK_CONTAINER(pattern01Box), leanCheckButton);
  //
  //  control->leanCheckButton = leanCheckButton;
  //
  //  leanLabel = gtk_label_new("Rectangle Lean Degree: ");
  //  gtk_container_add(GTK_CONTAINER(pattern01Box), leanLabel);
  //
  //  leanControlSlider =
  //      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1.0f, 180.0f,
  //      1.0f);
  //  gtk_scale_set_digits(GTK_SCALE(offsetControlSlider), 0);
  //
  //  control->leanControlSlider = leanControlSlider;
  //
  //  gtk_range_set_value(GTK_RANGE(leanControlSlider), 90.0f);
  //
  //  gtk_container_add(GTK_CONTAINER(pattern01Box), leanControlSlider);
  //
  //  addSeparator(GTK_CONTAINER(pattern01Box), GTK_ORIENTATION_HORIZONTAL,
  //               CONTROL_BOX_WIDTH, SEPARATOR_WIDTH);

  offsetDirectionLabel = gtk_label_new("Offset Direction: ");
  gtk_widget_set_halign(offsetDirectionLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(pattern01Box), offsetDirectionLabel);

  offsetDirectionComboBox = gtk_combo_box_text_new();
  gtk_container_add(GTK_CONTAINER(pattern01Box), offsetDirectionComboBox);

  control->offsetDirectionComboBox = offsetDirectionComboBox;

  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(offsetDirectionComboBox),
                                 "Horizontal");
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(offsetDirectionComboBox),
                                 "Vertical");

  gtk_combo_box_set_active(GTK_COMBO_BOX(offsetDirectionComboBox), 0);

  offsetTypeLabel = gtk_label_new("Offset Type: ");
  gtk_widget_set_halign(offsetTypeLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(pattern01Box), offsetTypeLabel);

  offsetTypeComboBox = gtk_combo_box_text_new();
  gtk_container_add(GTK_CONTAINER(pattern01Box), offsetTypeComboBox);

  control->offsetTypeComboBox = offsetTypeComboBox;

  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(offsetTypeComboBox),
                                 "Modulo 2");
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(offsetTypeComboBox),
                                 "Accumulate");
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(offsetTypeComboBox),
                                 "Random");

  gtk_combo_box_set_active(GTK_COMBO_BOX(offsetTypeComboBox), 0);

  offsetControlTypeLabel = gtk_label_new("Offset Control Type: ");
  gtk_widget_set_halign(offsetControlTypeLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(pattern01Box), offsetControlTypeLabel);

  control->offsetControlTypeLabel = offsetControlTypeLabel;

  offsetControlTypeComboBox = gtk_combo_box_text_new();
  gtk_container_add(GTK_CONTAINER(pattern01Box), offsetControlTypeComboBox);

  control->offsetControlTypeComboBox = offsetControlTypeComboBox;

  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(offsetControlTypeComboBox),
                                 "Accurate");

  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(offsetControlTypeComboBox),
                                 "Manual");

  gtk_combo_box_set_active(GTK_COMBO_BOX(offsetControlTypeComboBox), 0);

  offsetControlLabel = gtk_label_new("Offset Control: ");
  gtk_widget_set_halign(offsetControlLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(pattern01Box), offsetControlLabel);

  control->offsetControlLabel = offsetControlLabel;

  offsetControlSlider =
      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1.0f, 7.0f, 1.0f);
  gtk_scale_set_digits(GTK_SCALE(offsetControlSlider), 0);

  control->offsetControlSlider = offsetControlSlider;

  gtk_range_set_value(GTK_RANGE(offsetControlSlider), 1.0f);

  gtk_container_add(GTK_CONTAINER(pattern01Box), offsetControlSlider);

  ///////////////////////////////////////////////////////////////////////

  addSeparator(GTK_CONTAINER(controlBox), GTK_ORIENTATION_HORIZONTAL,
               CONTROL_BOX_WIDTH, SEPARATOR_WIDTH);

  wireframeSwitch = gtk_check_button_new_with_label(" Show Wireframe");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wireframeSwitch),
                               control->patternData->wireframeSwitch);
  gtk_container_add(GTK_CONTAINER(controlBox), wireframeSwitch);

  wireframeColorLabel = gtk_label_new("Wireframe Color: ");
  gtk_widget_set_halign(wireframeColorLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), wireframeColorLabel);

  GdkRGBA color = {1.0f, 1.0f, 1.0f, 1.0f};

  wireframeColorChooser = gtk_color_button_new_with_rgba(&color);
  gtk_container_add(GTK_CONTAINER(controlBox), wireframeColorChooser);

  control->wireframeColorChooser = wireframeColorChooser;

  wireframeWidthLabel = gtk_label_new("Wireframe Width: ");
  gtk_widget_set_halign(wireframeWidthLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), wireframeWidthLabel);

  wireframeWidthSlider =
      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1.0f, 5.0f, 1.0f);

  gtk_range_set_value(GTK_RANGE(wireframeWidthSlider), 1.0f);

  control->wireframeWidthSlider = wireframeWidthSlider;

  gtk_container_add(GTK_CONTAINER(controlBox), wireframeWidthSlider);

  addSeparator(GTK_CONTAINER(controlBox), GTK_ORIENTATION_HORIZONTAL,
               CONTROL_BOX_WIDTH, SEPARATOR_WIDTH);

  textureLabel = gtk_label_new("Texture:");
  gtk_widget_set_halign(textureLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), textureLabel);

  textureInfoLabel = gtk_label_new(DEFAULT_TEXTURE);
  gtk_label_set_max_width_chars(GTK_LABEL(textureInfoLabel), 30);
  gtk_label_set_line_wrap(GTK_LABEL(textureInfoLabel), TRUE);
  gtk_label_set_line_wrap_mode(GTK_LABEL(textureInfoLabel), PANGO_WRAP_CHAR);
  gtk_container_add(GTK_CONTAINER(controlBox), textureInfoLabel);

  control->textureInfoLabel = textureInfoLabel;

  textureInfoButton = gtk_button_new_with_label("Choose File: ");
  gtk_container_add(GTK_CONTAINER(controlBox), textureInfoButton);

  randUVSeedLabel = gtk_label_new("Random UV Offset: ");
  gtk_widget_set_halign(randUVSeedLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), randUVSeedLabel);

  randUVSeedSlider =
      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0f, 1.0f, 0.001f);
  gtk_scale_set_digits(GTK_SCALE(randUVSeedSlider), 3);
  gtk_range_set_value(GTK_RANGE(randUVSeedSlider), 0.0f);

  gtk_container_add(GTK_CONTAINER(controlBox), randUVSeedSlider);

  uvScaleLabel = gtk_label_new("UV Scale: ");
  gtk_widget_set_halign(uvScaleLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), uvScaleLabel);

  uvScaleSlider =
      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0f, 2.0f, 0.001f);
  gtk_scale_set_digits(GTK_SCALE(uvScaleSlider), 3);
  gtk_range_set_value(GTK_RANGE(uvScaleSlider), 1.0f);
  control->uvScaleSlider = uvScaleSlider;

  gtk_container_add(GTK_CONTAINER(controlBox), uvScaleSlider);

  uvRotateCheckButton =
      gtk_check_button_new_with_label(" Randomize UV Rotation");

  control->uvRotateCheckButton = uvRotateCheckButton;

  gtk_container_add(GTK_CONTAINER(controlBox), uvRotateCheckButton);

  addSeparator(GTK_CONTAINER(controlBox), GTK_ORIENTATION_HORIZONTAL,
               CONTROL_BOX_WIDTH, SEPARATOR_WIDTH);

  colorSeedLabel = gtk_label_new("Random Color Seed: ");
  gtk_widget_set_halign(colorSeedLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), colorSeedLabel);

  colorSeedSlider =
      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0f, 1.0f, 0.001f);

  gtk_scale_set_digits(GTK_SCALE(colorSeedSlider), 3);

  control->colorSeedSlider = colorSeedSlider;

  gtk_container_add(GTK_CONTAINER(controlBox), colorSeedSlider);

  colorMinLabel = gtk_label_new("Random Color Min: ");
  gtk_widget_set_halign(colorMinLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), colorMinLabel);

  colorMinSlider =
      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0f, 1.0f, 0.001f);
  gtk_scale_set_digits(GTK_SCALE(colorMinSlider), 3);
  gtk_range_set_value(GTK_RANGE(colorMinSlider), 1.0f);
  control->colorMinSlider = colorMinSlider;

  gtk_container_add(GTK_CONTAINER(controlBox), colorMinSlider);

  colorMaxLabel = gtk_label_new("Random Color Max: ");
  gtk_widget_set_halign(colorMaxLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), colorMaxLabel);

  colorMaxSlider =
      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1.0f, 2.0f, 0.001f);
  gtk_scale_set_digits(GTK_SCALE(colorMaxSlider), 3);
  gtk_range_set_value(GTK_RANGE(colorMaxSlider), 1.0f);
  control->colorMaxSlider = colorMaxSlider;

  gtk_container_add(GTK_CONTAINER(controlBox), colorMaxSlider);

  addSeparator(GTK_CONTAINER(controlBox), GTK_ORIENTATION_HORIZONTAL,
               CONTROL_BOX_WIDTH, SEPARATOR_WIDTH);

  renderPathLabel = gtk_label_new("Render Path: ");
  gtk_widget_set_halign(renderPathLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), renderPathLabel);

  renderPathInfoLabel = gtk_label_new("Undefined!");
  gtk_label_set_max_width_chars(GTK_LABEL(renderPathInfoLabel), 30);
  gtk_label_set_line_wrap(GTK_LABEL(renderPathInfoLabel), TRUE);
  gtk_label_set_line_wrap_mode(GTK_LABEL(renderPathInfoLabel), PANGO_WRAP_CHAR);

  gtk_container_add(GTK_CONTAINER(controlBox), renderPathInfoLabel);

  control->renderPathInfoLabel = renderPathInfoLabel;

  renderPathInfoButton = gtk_button_new_with_label("Choose Render Path: ");
  gtk_container_add(GTK_CONTAINER(controlBox), renderPathInfoButton);

  renderColorCheckButton =
      gtk_check_button_new_with_label(" Render Color Image");

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(renderColorCheckButton), TRUE);

  gtk_container_add(GTK_CONTAINER(controlBox), renderColorCheckButton);

  control->renderColorCheckButton = renderColorCheckButton;

  renderWireframeCheckButton =
      gtk_check_button_new_with_label(" Render Wireframe Image");

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(renderWireframeCheckButton),
                               TRUE);

  gtk_container_add(GTK_CONTAINER(controlBox), renderWireframeCheckButton);

  control->renderWireframeCheckButton = renderWireframeCheckButton;

  addSeparator(GTK_CONTAINER(controlBox), GTK_ORIENTATION_HORIZONTAL,
               CONTROL_BOX_WIDTH, SEPARATOR_WIDTH);

  renderSizeBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, BOX_SPACE * 2);
  gtk_container_add(GTK_CONTAINER(controlBox), renderSizeBox);

  renderSizeLabel = gtk_label_new("Render Size: ");
  gtk_widget_set_halign(renderSizeLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(renderSizeBox), renderSizeLabel);

  renderSizeEntryBuffer = gtk_entry_buffer_new("", 0);

  renderSizeEntry = gtk_entry_new_with_buffer(renderSizeEntryBuffer);
  gtk_container_add(GTK_CONTAINER(renderSizeBox), renderSizeEntry);
  control->renderSizeEntry = renderSizeEntry;

  renderButton = gtk_button_new_with_label("Render the Texture!");

  control->renderButton = renderButton;

  gtk_container_add(GTK_CONTAINER(controlBox), renderButton);

  // signal connection

  g_signal_connect(widthEntryBuffer, "inserted-text",
                   G_CALLBACK(entryBufferInserted), control);

  g_signal_connect(heightEntryBuffer, "inserted-text",
                   G_CALLBACK(entryBufferInserted), control);

  g_signal_connect(numCpySlider, "value-changed", G_CALLBACK(numCpyChanged),
                   control);

  g_signal_connect(dimensionButton, "clicked",
                   G_CALLBACK(dimensionButtonClicked), control);

  g_signal_connect(offsetTypeComboBox, "changed", G_CALLBACK(offsetTypeChanged),
                   control);

  g_signal_connect(offsetDirectionComboBox, "changed",
                   G_CALLBACK(offsetDirectionChanged), control);

  g_signal_connect(offsetControlTypeComboBox, "changed",
                   G_CALLBACK(offsetControlTypeChanged), control);

  g_signal_connect(offsetControlSlider, "value-changed",
                   G_CALLBACK(offsetControlChanged), control);

  g_signal_connect(wireframeSwitch, "toggled",
                   G_CALLBACK(wireframeSwitchToggled), control);

  g_signal_connect(wireframeWidthSlider, "value-changed",
                   G_CALLBACK(wireframeWidthChanged), control);

  g_signal_connect(wireframeColorChooser, "color-set",
                   G_CALLBACK(wireframeColorSet), control);

  g_signal_connect(textureInfoButton, "clicked",
                   G_CALLBACK(textureInfoButtonClicked), control);

  g_signal_connect(randUVSeedSlider, "value-changed",
                   G_CALLBACK(randUVSeedChanged), control);

  g_signal_connect(uvScaleSlider, "value-changed", G_CALLBACK(uvScaleChanged),
                   control);

  g_signal_connect(uvRotateCheckButton, "toggled", G_CALLBACK(uvRotateToggled),
                   control);

  g_signal_connect(colorSeedSlider, "value-changed",
                   G_CALLBACK(colorSeedChanged), control);

  g_signal_connect(colorMinSlider, "value-changed",
                   G_CALLBACK(colorRangeChanged), control);

  g_signal_connect(colorMaxSlider, "value-changed",
                   G_CALLBACK(colorRangeChanged), control);

  g_signal_connect(renderSizeEntryBuffer, "inserted-text",
                   G_CALLBACK(entryBufferInserted), control);

  g_signal_connect(renderPathInfoButton, "clicked",
                   G_CALLBACK(renderPathInfoButtonClicked), control);

  g_signal_connect(renderButton, "clicked", G_CALLBACK(renderButtonClicked),
                   control);

  gtk_widget_show_all(scrollWindow);

  return control;
}

static gboolean glRender(GtkGLArea *area, GdkGLContext *context,
                         void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

  glViewport(0, 0, WINDOW_HEIGHT, WINDOW_HEIGHT);
  glScissor(0, 0, WINDOW_HEIGHT, WINDOW_HEIGHT);

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  gtk_gl_area_make_current(user->glArea);
  glUseProgram(user->shaderProgram);

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
          gtk_range_get_value(GTK_RANGE(control->wireframeWidthSlider)) * 2;
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

void *initPatternControl(GtkWindow *mainWindow, GtkContainer *container,
                         GtkGLArea *glArea, GLuint shaderProgram) {
  gtk_gl_area_make_current(glArea);

  struct PatternModel *pattern = patternModelNew(glArea, 50, 50, 1, 0);
  void *user = (void *)patternDataNew(glArea, shaderProgram, pattern);
  struct ControlData *control = initControl(mainWindow, container, user);

  g_signal_connect(glArea, "render", G_CALLBACK(glRender), control);

  glUseProgram(shaderProgram);
  gtk_gl_area_attach_buffers(glArea);
  gtk_gl_area_queue_render(glArea);

  return (void *)control;
}

// static void constructPatternModel(void *control) {
//  struct ControlData *userControl = (struct ControlData *)control;
//  struct PatternData *user = userControl->patternData;
//
//  struct PatternModel *pattern = patternModelNew(user->glArea, 50, 50, 1);
//
//  user->pattern = pattern;
//  gtk_gl_area_queue_render(user->glArea);
//}
//
// static void freePatternModel(void *control) {
//  struct ControlData *userControl = (struct ControlData *)control;
//  struct PatternData *user = userControl->patternData;
//  patternModelFree(user->pattern);
//
//  user->pattern = NULL;
//  gtk_gl_area_queue_render(user->glArea);
//}
//
// struct PatternAlpha {
//  void *(*initPatternControl)(GtkWindow *mainWindow, GtkContainer *container,
//                              GtkGLArea *glArea, GLuint shaderProgram);
//  // void (*freePattern)(void *control);
//  void (*constructPatternModel)(void *control);
//  void (*freePatternModel)(void *control);
//
//  void *patternControl;
//
//  const char *patternName;
//};

// static void controlDataFree(struct ControlData *data) {
//  // Gtk use reference counting to automatically clean up the resourse
//  // therefore, we can clean up the control panel simply by destroying the
//  // container
//
//  //  gtk_widget_hide(data->controlBox);
//  //  gtk_widget_destroy(data->controlBox);
//  //
//  //  patternDataFree(data->patternData);
//  //  free(data);
//
//  // glClearColor(0.0, 0.0, 0.0, 1.0);
//  // glClear(GL_COLOR_BUFFER_BIT);
//  // glFlush();
//
//  //  struct PatternModel *pattern =
//  //      patternModelNew(user->glArea, user->pattern->sizeX,
//  //                      user->pattern->sizeY, cpy);
//  //
//  //  patternModelFree(user->pattern);
//  //
//  //  user->pattern = pattern;
//}
//
// static void freePattern(void *pattern) {
//  struct PatternAlpha *fr = (struct PatternAlpha *)pattern;
//
//  //  controlDataFree((struct ControlData *)fr->patternControl);
//  //  fr->patternControl = NULL;
//}

// static void patternAlphaFree(void *pattern) {
//  struct PatternAlpha *fr = (struct PatternAlpha *)pattern;
//  controlDataFree((struct ControlData *)fr->patternControl);
//  free(fr);
//}

// void *patternAlphaNew(const char *patternName) {
//  struct PatternAlpha *re =
//      (struct PatternAlpha *)defenseMalloc(sizeof(struct PatternAlpha));
//  re->initPatternControl = initPatternControl;
//  re->constructPatternModel = constructPatternModel;
//  re->freePatternModel = freePatternModel;
//
//  re->patternControl = NULL;
//
//  re->patternName = patternName;
//
//  return (void *)re;
//}
