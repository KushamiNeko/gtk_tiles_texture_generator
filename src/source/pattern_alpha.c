#include "../header/pattern_alpha.h"

struct PatternData {
  GtkGLArea *glArea;

  gchar *textureFile;
  GLuint tex;
  GLint texLoc;

  GLuint shaderProgram;

  struct PatternModel *pattern;

  GLuint wireframeSwitch;

  GLint wireframeDrawUniformLoc;
  GLint wireframeDraw;

  GLint wireframeColorUniformLoc;
  GLfloat *wireframeColor;
};

static void patternDataInitTexMap(struct PatternData *patternData) {
  //  if (loadTexture(patternData->textureFile, &(patternData->shaderProgram),
  //                  GL_TEXTURE0, "diff_tex") != 1) {
  //    g_print("Texture loading failed: %s\n", patternData->textureFile);
  //  }

  if (loadTexture(patternData->textureFile, &(patternData->shaderProgram),
                  GL_TEXTURE0, &(patternData->tex),
                  &(patternData->texLoc)) != 1) {
    g_print("Texture loading failed: %s\n", patternData->textureFile);
  }

  //  if (loadTexture(patternData->textureFile, GL_TEXTURE0, patternData->tex)
  //  !=
  //      1) {
  //    printf("failed to load texture: %s\n", patternData->textureFile);
  //  }
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

// static void patternDataFree(struct PatternData *data) {
//  patternModelFree(data->pattern);
//  // free(data);
//}

struct ControlData {
  struct PatternData *patternData;

  GtkWindow *mainWindow;
  GtkWidget *controlBox;

  GtkWidget *widthEntry;
  GtkWidget *heightEntry;

  GtkWidget *numCpySlider;

  GtkWidget *offsetTypeComboBox;

  GtkWidget *offsetControlTypeLabel;
  GtkWidget *offsetControlTypeComboBox;

  GtkWidget *offsetControlLabel;
  GtkWidget *offsetControlSlider;

  GtkWidget *wireframeColorChooser;

  GtkWidget *colorSeedSlider;
  GtkWidget *colorMinSlider;
  GtkWidget *colorMaxSlider;

  GtkWidget *textureInfoLabel;

  GtkWidget *uvScaleSlider;
  GtkWidget *uvRotateCheckButton;
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

  if (active) {
    patternModelRandomizeUVRotate(user->pattern);
  } else {
    patternModelRandomizeUV(user->pattern);
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

  if (active) {
    patternModelRandomizeUVRotate(user->pattern);
  } else {
    patternModelRandomizeUV(user->pattern);
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

static void patternOffsetModulo2(struct PatternData *user, GLfloat amount) {
  patternModelInitUnitsPosition(user->pattern);

  for (int i = 0; i < user->pattern->numHeight; i++) {
    if (i % 2 != 0) {
      continue;
    }

    for (int j = 0; j < user->pattern->numWidth; j++) {
      struct Rectangle *rect =
          user->pattern->units[(i * user->pattern->numWidth) + j];

      rectangleMove(rect, amount, 0);
    }
  }
}

static void patternOffsetAccumulate(struct PatternData *user, GLfloat amount) {
  patternModelInitUnitsPosition(user->pattern);

  for (int i = 0; i < user->pattern->numHeight; i++) {
    for (int j = 0; j < user->pattern->numWidth; j++) {
      struct Rectangle *rect =
          user->pattern->units[(i * user->pattern->numWidth) + j];

      rectangleMove(rect, amount * i, 0);
    }
  }
}

static void patternOffsetRandom(struct PatternData *user) {
  patternModelInitUnitsPosition(user->pattern);
  double width = (GLfloat)__GL_VIEWPORT / user->pattern->numWidth;

  for (int i = 0; i < user->pattern->numHeight; i++) {
    double randNum = (GLfloat)rand() / (GLfloat)RAND_MAX;
    randNum = (randNum * 2) - 1;

    for (int j = 0; j < user->pattern->numWidth; j++) {
      struct Rectangle *rect =
          user->pattern->units[(i * user->pattern->numWidth) + j];

      rectangleMove(rect, width * randNum, 0);
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

    patternOffsetRandom(user);

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
  double offset = gtk_range_get_value(range);

  gint offsetType =
      gtk_combo_box_get_active(GTK_COMBO_BOX(control->offsetTypeComboBox));

  double offsetAmount;

  gint offsetControlType = gtk_combo_box_get_active(
      GTK_COMBO_BOX(control->offsetControlTypeComboBox));

  if (offsetControlType == 0) {
    offsetAmount = width / offset;
  } else if (offsetControlType == 1) {
    offsetAmount = width * offset;
  }

  if (offsetType == 0) {
    patternOffsetModulo2(user, offsetAmount);
  } else if (offsetType == 1) {
    patternOffsetAccumulate(user, offsetAmount);
  } else if (offsetType == 2) {
    patternOffsetRandom(user);
  }

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  patternModelSeamlessModelConstruct(user->pattern, user->glArea);

  setVBOData(&user->pattern->positionVBO, user->pattern->vertexCounts, 3,
             user->pattern->vertexPosition);

  fitSeamlessModelColor(user->pattern, colorMin, colorMax);

  // queue openGL render
  gtk_gl_area_queue_render(user->glArea);
}

static void numCpyChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

  unsigned int cpy = (unsigned int)gtk_range_get_value(range);

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  double scaleFactor = normalizeUVScaleRange(
      gtk_range_get_value(GTK_RANGE(control->uvScaleSlider)));

  struct PatternModel *pattern = patternModelNew(
      user->glArea, user->pattern->sizeX, user->pattern->sizeY, cpy);

  patternModelFree(user->pattern);
  user->pattern = pattern;

  double width = (GLfloat)__GL_VIEWPORT / user->pattern->numWidth;
  double offset = gtk_range_get_value(GTK_RANGE(control->offsetControlSlider));

  gint offsetType =
      gtk_combo_box_get_active(GTK_COMBO_BOX(control->offsetTypeComboBox));

  double offsetAmount;

  gint offsetControlType = gtk_combo_box_get_active(
      GTK_COMBO_BOX(control->offsetControlTypeComboBox));

  if (offsetControlType == 0) {
    offsetAmount = width / offset;
  } else if (offsetControlType == 1) {
    offsetAmount = width * offset;
  }

  if (offsetType == 0) {
    patternOffsetModulo2(user, offsetAmount);
  } else if (offsetType == 1) {
    patternOffsetAccumulate(user, offsetAmount);
  } else if (offsetType == 2) {
    patternOffsetRandom(user);
  }

  // re-construct the color of whole pattern units
  patternModelRandomizeColor(user->pattern);
  patternModelFitColor(user->pattern, colorMin, colorMax);
  patternModelRandomizeUV(user->pattern);
  patternModelScaleUV(user->pattern, scaleFactor);

  patternModelSeamlessModelConstruct(user->pattern, user->glArea);

  setVBOData(&user->pattern->positionVBO, user->pattern->vertexCounts, 3,
             user->pattern->vertexPosition);

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

static void dimensionButtonClicked(GtkButton *button, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

  gtk_range_set_value(GTK_RANGE(control->numCpySlider), 1.0f);
  gtk_range_set_value(GTK_RANGE(control->offsetControlSlider), 0.0f);

  const gchar *widthChar = gtk_entry_get_text(GTK_ENTRY(control->widthEntry));
  const gchar *heightChar = gtk_entry_get_text(GTK_ENTRY(control->heightEntry));

  if (strlen(widthChar) == 0 || strlen(heightChar) == 0) {
    return;
  }

  double width = atof(widthChar);
  double height = atof(heightChar);

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  double scaleFactor = normalizeUVScaleRange(
      gtk_range_get_value(GTK_RANGE(control->uvScaleSlider)));

  struct PatternModel *pattern =
      patternModelNew(user->glArea, width, height, 1);

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

static gchar *getTextureFile(GtkWindow *parentWindow) {
  GtkWidget *dialog = gtk_file_chooser_dialog_new(
      "Open File", parentWindow, GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel",
      GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);

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

  gchar *filePath = getTextureFile(control->mainWindow);
  if (filePath == NULL) {
    return;
  }

  gchar *baseName = pathGetBase(filePath);

  if (filePath) {
    user->textureFile = filePath;
    gtk_label_set_text(GTK_LABEL(control->textureInfoLabel), baseName);

    gtk_gl_area_make_current(user->glArea);
    glUseProgram(user->shaderProgram);

    GLuint *tex = &user->tex;
    glDeleteTextures(1, tex);

    patternDataInitTexMap(user);
    gtk_gl_area_queue_render(user->glArea);
  }
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

  GtkWidget *dimensionBox;

  GtkWidget *widthLabel;
  GtkWidget *widthEntry;
  GtkWidget *heightLabel;
  GtkWidget *heightEntry;

  GtkWidget *dimensionButton;
  GtkWidget *numCpyLabel;
  GtkWidget *numCpySlider;

  GtkWidget *wireframeColorLabel;
  GtkWidget *wireframeSwitch;
  GtkWidget *wireframeColorChooser;

  GtkWidget *offsetTypeLabel;
  GtkWidget *offsetTypeComboBox;
  GtkWidget *offsetControlTypeLabel;
  GtkWidget *offsetControlTypeComboBox;
  GtkWidget *offsetControlLabel;
  GtkWidget *offsetControlSlider;

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

  // GtkWidget *uvRotateLabel;
  GtkWidget *uvRotateCheckButton;

  scrollWindow = gtk_scrolled_window_new(NULL, NULL);
  // set the horizontal policy of scrolled window to never to make the
  // underlying
  // widget shrink a little bit on the right end margin
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollWindow),
                                 GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  gtk_widget_set_vexpand(scrollWindow, TRUE);
  // gtk_widget_set_hexpand(scrollWindow, TRUE);

  gtk_container_add(container, scrollWindow);

  controlBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, BOX_SPACE);
  gtk_container_add(GTK_CONTAINER(scrollWindow), controlBox);

  // gtk_widget_set_margin_start(controlBox, 30);
  gtk_widget_set_margin_end(controlBox, SCROLLBAR_MARGIN);

  // gtk_container_add(container, controlBox);
  control->controlBox = controlBox;

  addSeparator(GTK_CONTAINER(controlBox), GTK_ORIENTATION_HORIZONTAL,
               CONTROL_BOX_WIDTH, SEPARATOR_WIDTH);

  dimensionBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, BOX_SPACE * 2);
  gtk_container_add(GTK_CONTAINER(controlBox), dimensionBox);

  gtk_widget_set_halign(dimensionBox, GTK_ALIGN_CENTER);

  widthLabel = gtk_label_new("Width: ");
  gtk_container_add(GTK_CONTAINER(dimensionBox), widthLabel);

  widthEntry = gtk_entry_new();
  gtk_container_add(GTK_CONTAINER(dimensionBox), widthEntry);
  control->widthEntry = widthEntry;

  heightLabel = gtk_label_new("Height: ");
  gtk_container_add(GTK_CONTAINER(dimensionBox), heightLabel);

  heightEntry = gtk_entry_new();
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

  offsetTypeLabel = gtk_label_new("Offset Type: ");
  gtk_widget_set_halign(offsetTypeLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), offsetTypeLabel);

  offsetTypeComboBox = gtk_combo_box_text_new();
  gtk_container_add(GTK_CONTAINER(controlBox), offsetTypeComboBox);

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
  gtk_container_add(GTK_CONTAINER(controlBox), offsetControlTypeLabel);

  control->offsetControlTypeLabel = offsetControlTypeLabel;

  offsetControlTypeComboBox = gtk_combo_box_text_new();
  gtk_container_add(GTK_CONTAINER(controlBox), offsetControlTypeComboBox);

  control->offsetControlTypeComboBox = offsetControlTypeComboBox;

  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(offsetControlTypeComboBox),
                                 "Accurate");

  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(offsetControlTypeComboBox),
                                 "Manual");

  gtk_combo_box_set_active(GTK_COMBO_BOX(offsetControlTypeComboBox), 0);

  offsetControlLabel = gtk_label_new("Offset Control: ");
  gtk_widget_set_halign(offsetControlLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), offsetControlLabel);

  control->offsetControlLabel = offsetControlLabel;

  offsetControlSlider;
  // offsetControlSlider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
  //                                               -1.0f, 1.0f, 0.000001f);

  offsetControlSlider =
      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1.0f, 7.0f, 1.0f);

  control->offsetControlSlider = offsetControlSlider;

  // gtk_range_set_value(GTK_RANGE(offsetControlSlider), 0.0f);
  gtk_range_set_value(GTK_RANGE(offsetControlSlider), 1.0f);

  gtk_container_add(GTK_CONTAINER(controlBox), offsetControlSlider);

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

  // signal connection

  g_signal_connect(numCpySlider, "value-changed", G_CALLBACK(numCpyChanged),
                   control);

  g_signal_connect(dimensionButton, "clicked",
                   G_CALLBACK(dimensionButtonClicked), control);

  g_signal_connect(offsetTypeComboBox, "changed", G_CALLBACK(offsetTypeChanged),
                   control);

  g_signal_connect(offsetControlTypeComboBox, "changed",
                   G_CALLBACK(offsetControlTypeChanged), control);

  g_signal_connect(offsetControlSlider, "value-changed",
                   G_CALLBACK(offsetControlChanged), control);

  g_signal_connect(wireframeSwitch, "toggled",
                   G_CALLBACK(wireframeSwitchToggled), control);

  g_signal_connect(wireframeColorChooser, "color-set",
                   G_CALLBACK(wireframeColorSet), control);

  g_signal_connect(colorSeedSlider, "value-changed",
                   G_CALLBACK(colorSeedChanged), control);

  g_signal_connect(colorMinSlider, "value-changed",
                   G_CALLBACK(colorRangeChanged), control);

  g_signal_connect(colorMaxSlider, "value-changed",
                   G_CALLBACK(colorRangeChanged), control);

  g_signal_connect(textureInfoButton, "clicked",
                   G_CALLBACK(textureInfoButtonClicked), control);

  g_signal_connect(randUVSeedSlider, "value-changed",
                   G_CALLBACK(randUVSeedChanged), control);

  g_signal_connect(uvScaleSlider, "value-changed", G_CALLBACK(uvScaleChanged),
                   control);

  g_signal_connect(uvRotateCheckButton, "toggled", G_CALLBACK(uvRotateToggled),
                   control);

  // gtk_widget_show_all(controlBox);
  gtk_widget_show_all(scrollWindow);

  return control;
}

static gboolean glRender(GtkGLArea *area, GdkGLContext *context,
                         void *userData) {
  // struct PatternData *user = (struct PatternData *)userData;
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *user = control->patternData;

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
      user->wireframeDraw = 1;
      glBindVertexArray(user->pattern->vao);
      glUniform1i(user->wireframeDrawUniformLoc, user->wireframeDraw);
      glDrawArrays(GL_LINES, 0, user->pattern->vertexCounts);

      if (user->pattern->seamlessModel != NULL) {
        glBindVertexArray(user->pattern->seamlessModel->vao);
        glUniform1i(user->wireframeDrawUniformLoc, user->wireframeDraw);
        glDrawArrays(GL_LINES, 0, user->pattern->seamlessModel->vertexCounts);
      }
    }
  }

  // drawing wireframe
  // glDrawArrays(GL_LINES, 0, user->pattern->vertexCounts);

  glFlush();
  return TRUE;
}

static void *initPatternControl(GtkWindow *mainWindow, GtkContainer *container,
                                GtkGLArea *glArea, GLuint shaderProgram) {
  gtk_gl_area_make_current(glArea);

  // struct PatternModel *pattern = patternModelNew(glArea, 50, 50, 1);
  void *user = (void *)patternDataNew(glArea, shaderProgram, NULL);
  struct ControlData *control = initControl(mainWindow, container, user);

  g_signal_connect(glArea, "render", G_CALLBACK(glRender), control);

  glUseProgram(shaderProgram);
  gtk_gl_area_attach_buffers(glArea);
  gtk_gl_area_queue_render(glArea);

  return (void *)control;
}

static void constructPatternModel(void *control) {
  struct ControlData *userControl = (struct ControlData *)control;
  struct PatternData *user = userControl->patternData;

  struct PatternModel *pattern = patternModelNew(user->glArea, 50, 50, 1);

  user->pattern = pattern;
  gtk_gl_area_queue_render(user->glArea);
}

static void freePatternModel(void *control) {
  struct ControlData *userControl = (struct ControlData *)control;
  struct PatternData *user = userControl->patternData;
  patternModelFree(user->pattern);

  user->pattern = NULL;
  gtk_gl_area_queue_render(user->glArea);
}

struct PatternAlpha {
  void *(*initPatternControl)(GtkWindow *mainWindow, GtkContainer *container,
                              GtkGLArea *glArea, GLuint shaderProgram);
  // void (*freePattern)(void *control);
  void (*constructPatternModel)(void *control);
  void (*freePatternModel)(void *control);

  void *patternControl;

  const char *patternName;
};

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

void *patternAlphaNew(const char *patternName) {
  struct PatternAlpha *re =
      (struct PatternAlpha *)defenseMalloc(sizeof(struct PatternAlpha));
  re->initPatternControl = initPatternControl;
  re->constructPatternModel = constructPatternModel;
  re->freePatternModel = freePatternModel;

  re->patternControl = NULL;

  re->patternName = patternName;

  return (void *)re;
}
