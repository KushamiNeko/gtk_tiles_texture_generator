#include "../header/pattern_alpha.h"

struct PatternData {
  GtkGLArea *glArea;

  gchar *textureFile;
  GLuint shaderProgram;

  struct PatternModel *pattern;
};

static void patternDataInitTexMap(struct PatternData *patternData) {
  if (loadTexture(patternData->textureFile, &(patternData->shaderProgram),
                  GL_TEXTURE0, "diff_tex") != 0) {
    g_print("Texture loading failed: %s\n", patternData->textureFile);
  }
}

static struct PatternData *patternDataNew(GtkGLArea *glArea,
                                          GLuint shaderProgram,
                                          struct PatternModel *pattern) {
  struct PatternData *re =
      (struct PatternData *)defenseCalloc(1, sizeof(struct PatternData));

  re->glArea = glArea;

  re->textureFile = DEFAULT_TEXTURE;

  re->shaderProgram = shaderProgram;
  re->pattern = pattern;

  patternDataInitTexMap(re);
  return re;
}

static void patternDataFree(struct PatternData *data) {
  // glDeleteProgram(data->shaderProgram);
  patternModelFree(data->pattern);
}

// struct widgetList {
//  GtkWidget *widget;
//  struct widgetList *next;
//};
//
// struct widgetList *newWidgetList() {
//  struct widgetList *re = calloc(1, sizeof(struct widgetList));
//  re->widget = NULL;
//  re->next = NULL;
//
//  return re;
//}
//
// static void addWidgetList(struct widgetList **list, GtkWidget *widget) {
//  struct widgetList *re = calloc(1, sizeof(struct widgetList));
//  re->widget = widget;
//  re->next = *list;
//
//  *list = re;
//
//  // struct widgetList **p = &list;
//  // while (1) {
//  //  if ((*p)->next == NULL) {
//  //    (*p)->next = re;
//  //    return;
//  //  }
//
//  //  p = &((*p)->next);
//  //}
//}
//
// static void freeWidgetList(struct widgetList *list) {
//  struct widgetList **current = &list;
//  struct widgetList **next = &((*current)->next);
//
//  while (1) {
//    gtk_widget_destroy((*current)->widget);
//    free(*current);
//    current = next;
//    if (*current == NULL) {
//      return;
//    }
//
//    next = &((*current)->next);
//  }
//}

struct ControlData {
  struct PatternData *patternData;
  // struct widgetList *support;

  GtkWindow *mainWindow;
  GtkWidget *controlBox;

  GtkWidget *widthEntry;
  GtkWidget *heightEntry;

  GtkWidget *numCpySlider;

  GtkWidget *colorSeedSlider;
  GtkWidget *colorMinSlider;
  GtkWidget *colorMaxSlider;

  GtkWidget *textureInfoLabel;

  GtkWidget *uvScaleSlider;
};

static void controlDataFree(struct ControlData *data) {
  patternDataFree(data->patternData);

  //  gtk_widget_destroy(data->widthEntry);
  //  gtk_widget_destroy(data->heightEntry);
  //  gtk_widget_destroy(data->numCpySlider);
  //  gtk_widget_destroy(data->colorSeedSlider);
  //  gtk_widget_destroy(data->colorMinSlider);
  //  gtk_widget_destroy(data->colorMaxSlider);
  //  gtk_widget_destroy(data->textureInfoLabel);

  // Gtk use reference counting to automatically clean up the resourse
  // therefore, we can clean up the control panel simply by destroying the
  // container

  gtk_widget_destroy(data->controlBox);
  // freeWidgetList(data->support);

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();
}

static void colorSeedChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *userPattern = control->patternData;

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  // re-construct the color of whole pattern units
  // initPatternRandColor(userPattern->pattern, colorMin, colorMax);
  patternModelRandomizeColor(userPattern->pattern);
  patternModelFitColor(userPattern->pattern, colorMin, colorMax);
  // directly update the specific memory allocated for the data
  setVBOData(&userPattern->pattern->colorVBO,
             userPattern->pattern->vertexCounts, 3,
             userPattern->pattern->vertexColor);

  // queue openGL render
  gtk_gl_area_queue_render(userPattern->glArea);
}

static void randUVSeedChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *userPattern = control->patternData;

  // initPatternRandUV(userPattern->pattern);
  patternModelRandomizeUV(userPattern->pattern);

  setVBOData(&userPattern->pattern->uvVBO, userPattern->pattern->vertexCounts,
             2, userPattern->pattern->vertexUV);

  gtk_gl_area_queue_render(userPattern->glArea);
}

static double normalizeUVScaleRange(double rangeValue) {
  double scaleFactor;
  if (rangeValue > 1.0f) {
    scaleFactor = fit01(rangeValue - 1.0f, 1.0f, 10.0f);
  } else if (rangeValue < 1.0f) {
    scaleFactor = fit01(rangeValue, 0.1f, 1.0f);
  } else {
    scaleFactor = rangeValue;
  }

  return scaleFactor;
}

static void uvScaleChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *userPattern = control->patternData;

  double rangeValue = gtk_range_get_value(GTK_RANGE(range));

  double scaleFactor = normalizeUVScaleRange(rangeValue);

  patternModelScaleUV(userPattern->pattern, scaleFactor);

  setVBOData(&userPattern->pattern->uvVBO, userPattern->pattern->vertexCounts,
             2, userPattern->pattern->vertexUV);

  gtk_gl_area_queue_render(userPattern->glArea);
}

static void numCpyChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *userPattern = control->patternData;
  unsigned int cpy = (unsigned int)gtk_range_get_value(range);

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  double scaleFactor = normalizeUVScaleRange(
      gtk_range_get_value(GTK_RANGE(control->uvScaleSlider)));

  struct PatternModel *pattern =
      patternModelNew(userPattern->glArea, userPattern->pattern->sizeX,
                      userPattern->pattern->sizeY, cpy);

  patternModelFree(userPattern->pattern);

  userPattern->pattern = pattern;
  // re-construct the color of whole pattern units
  patternModelRandomizeColor(userPattern->pattern);
  patternModelFitColor(userPattern->pattern, colorMin, colorMax);
  patternModelRandomizeUV(userPattern->pattern);
  patternModelScaleUV(userPattern->pattern, scaleFactor);

  setVBOData(&userPattern->pattern->colorVBO,
             userPattern->pattern->vertexCounts, 3,
             userPattern->pattern->vertexColor);

  setVBOData(&userPattern->pattern->uvVBO, userPattern->pattern->vertexCounts,
             2, userPattern->pattern->vertexUV);

  // queue openGL render
  gtk_gl_area_queue_render(userPattern->glArea);
}

static void colorRangeChanged(GtkRange *range, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *userPattern = control->patternData;

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  // only re-generate GL color data and fit it into new range
  patternModelFitColor(userPattern->pattern, colorMin, colorMax);
  setVBOData(&userPattern->pattern->colorVBO,
             userPattern->pattern->vertexCounts, 3,
             userPattern->pattern->vertexColor);

  // queue openGL render
  gtk_gl_area_queue_render(userPattern->glArea);
}

static void dimensionButtonClicked(GtkButton *button, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *userPattern = control->patternData;

  const gchar *widthChar = gtk_entry_get_text(GTK_ENTRY(control->widthEntry));
  const gchar *heightChar = gtk_entry_get_text(GTK_ENTRY(control->heightEntry));

  if (strlen(widthChar) == 0 || strlen(heightChar) == 0) {
    return;
  }

  GLfloat width = atof(widthChar);
  GLfloat height = atof(heightChar);

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  double scaleFactor = normalizeUVScaleRange(
      gtk_range_get_value(GTK_RANGE(control->uvScaleSlider)));

  struct PatternModel *pattern =
      patternModelNew(userPattern->glArea, width, height, 1);

  patternModelFree(userPattern->pattern);

  userPattern->pattern = pattern;

  // only re-generate GL color data and fit it into new range
  patternModelRandomizeColor(userPattern->pattern);
  patternModelFitColor(userPattern->pattern, colorMin, colorMax);
  patternModelRandomizeUV(userPattern->pattern);
  patternModelScaleUV(userPattern->pattern, scaleFactor);

  setVBOData(&userPattern->pattern->colorVBO,
             userPattern->pattern->vertexCounts, 3,
             userPattern->pattern->vertexColor);

  setVBOData(&userPattern->pattern->uvVBO, userPattern->pattern->vertexCounts,
             2, userPattern->pattern->vertexUV);

  gtk_range_set_value(GTK_RANGE(control->numCpySlider), 1.0f);

  gtk_gl_area_queue_render(userPattern->glArea);
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

static gchar *getBaseName(gchar *filePath) {
  gchar *baseName = NULL;

  for (gchar *i = filePath; strcmp(i, "\0"); i++) {
    if (*i == '/') {
      baseName = i + 1;
    }
  }
  return baseName;
}

static void textureInfoButtonClicked(GtkButton *button, void *userData) {
  struct ControlData *control = (struct ControlData *)userData;
  struct PatternData *userPattern = control->patternData;

  gchar *filePath = getTextureFile(control->mainWindow);
  if (filePath == NULL) {
    return;
  }

  gchar *baseName = getBaseName(filePath);

  if (filePath) {
    // userPattern->pattern->texturePath = filePath;
    userPattern->textureFile = filePath;
    gtk_label_set_text(GTK_LABEL(control->textureInfoLabel), baseName);
  }
}

static void addSeparator(GtkContainer *container,
                         const GtkOrientation orientation,
                         const unsigned int width, const unsigned int height) {
  GtkWidget *separator = gtk_separator_new(orientation);
  gtk_widget_set_size_request(separator, width, height);
  gtk_container_add(container, separator);

  // return separator;
}

static struct ControlData *initControl(GtkWindow *mainWindow,
                                       GtkContainer *container, void *user) {
  // initialize random generator here instead of every time the value changed
  srand(time(NULL));

  // alloc memory for controlData used by signal function
  struct ControlData *control = defenseCalloc(1, sizeof(struct ControlData));

  control->patternData = (struct PatternData *)user;

  //  control->support = NULL;
  //  struct widgetList **support = &(control->support);

  control->mainWindow = mainWindow;

  GtkWidget *controlBox;

  GtkWidget *dimensionBox;

  GtkWidget *widthLabel;
  GtkWidget *widthEntry;
  GtkWidget *heightLabel;
  GtkWidget *heightEntry;

  GtkWidget *dimensionButton;
  GtkWidget *numCpyLabel;
  GtkWidget *numCpySlider;

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

  controlBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, BOX_SPACE);
  // addWidgetList(support, controlBox);

  gtk_container_add(container, controlBox);
  control->controlBox = controlBox;

  addSeparator(GTK_CONTAINER(controlBox), GTK_ORIENTATION_HORIZONTAL,
               CONTROL_BOX_WIDTH, SEPARATOR_WIDTH);

  // addWidgetList(support, dimensionSeparator);

  dimensionBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, BOX_SPACE * 2);
  gtk_container_add(GTK_CONTAINER(controlBox), dimensionBox);
  // addWidgetList(support, dimensionBox);

  gtk_widget_set_halign(dimensionBox, GTK_ALIGN_CENTER);

  widthLabel = gtk_label_new("width: ");
  gtk_container_add(GTK_CONTAINER(dimensionBox), widthLabel);
  // addWidgetList(support, widthLabel);

  widthEntry = gtk_entry_new();
  gtk_container_add(GTK_CONTAINER(dimensionBox), widthEntry);
  control->widthEntry = widthEntry;

  heightLabel = gtk_label_new("height: ");
  gtk_container_add(GTK_CONTAINER(dimensionBox), heightLabel);
  // addWidgetList(support, heightLabel);

  heightEntry = gtk_entry_new();
  gtk_container_add(GTK_CONTAINER(dimensionBox), heightEntry);
  control->heightEntry = heightEntry;

  dimensionButton = gtk_button_new_with_label("generate!");
  gtk_container_add(GTK_CONTAINER(controlBox), dimensionButton);
  // addWidgetList(support, dimensionButton);

  numCpyLabel = gtk_label_new("number copys");
  gtk_widget_set_halign(numCpyLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), numCpyLabel);
  // addWidgetList(support, numCpyLabel);

  numCpySlider =
      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1.0f, 20.0f, 1.00f);
  control->numCpySlider = numCpySlider;

  gtk_container_add(GTK_CONTAINER(controlBox), numCpySlider);

  addSeparator(GTK_CONTAINER(controlBox), GTK_ORIENTATION_HORIZONTAL,
               CONTROL_BOX_WIDTH, SEPARATOR_WIDTH);

  // addWidgetList(support, colorSeparator);

  colorSeedLabel = gtk_label_new("random color seed");
  gtk_widget_set_halign(colorSeedLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), colorSeedLabel);
  // addWidgetList(support, colorSeedLabel);

  colorSeedSlider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0f,
                                             1.0f, 0.000001f);
  control->colorSeedSlider = colorSeedSlider;

  gtk_container_add(GTK_CONTAINER(controlBox), colorSeedSlider);

  colorMinLabel = gtk_label_new("random color min");
  gtk_widget_set_halign(colorMinLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), colorMinLabel);
  // addWidgetList(support, colorMinLabel);

  colorMinSlider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0f,
                                            1.0f, 0.000001f);
  gtk_range_set_value(GTK_RANGE(colorMinSlider), 1.0f);
  control->colorMinSlider = colorMinSlider;

  gtk_container_add(GTK_CONTAINER(controlBox), colorMinSlider);

  colorMaxLabel = gtk_label_new("random color max");
  gtk_widget_set_halign(colorMaxLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), colorMaxLabel);
  // addWidgetList(support, colorMaxLabel);

  colorMaxSlider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1.0f,
                                            2.0f, 0.000001f);
  gtk_range_set_value(GTK_RANGE(colorMaxSlider), 1.0f);
  control->colorMaxSlider = colorMaxSlider;

  gtk_container_add(GTK_CONTAINER(controlBox), colorMaxSlider);

  addSeparator(GTK_CONTAINER(controlBox), GTK_ORIENTATION_HORIZONTAL,
               CONTROL_BOX_WIDTH, SEPARATOR_WIDTH);

  // addWidgetList(support, textureSeparator);

  textureLabel = gtk_label_new("Texture:");
  gtk_widget_set_halign(textureLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), textureLabel);
  // addWidgetList(support, textureLabel);

  // textureInfoLabel = gtk_label_new("choose a texture file");
  textureInfoLabel = gtk_label_new(DEFAULT_TEXTURE);
  gtk_label_set_max_width_chars(GTK_LABEL(textureInfoLabel), 30);
  gtk_label_set_line_wrap(GTK_LABEL(textureInfoLabel), TRUE);
  gtk_label_set_line_wrap_mode(GTK_LABEL(textureInfoLabel), PANGO_WRAP_CHAR);
  gtk_container_add(GTK_CONTAINER(controlBox), textureInfoLabel);

  control->textureInfoLabel = textureInfoLabel;

  textureInfoButton = gtk_button_new_with_label("Choose File");
  gtk_container_add(GTK_CONTAINER(controlBox), textureInfoButton);

  randUVSeedLabel = gtk_label_new("Random UV Offset");
  gtk_widget_set_halign(randUVSeedLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), randUVSeedLabel);
  // addWidgetList(support, colorMinLabel);

  randUVSeedSlider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0f,
                                              1.0f, 0.000001f);
  gtk_range_set_value(GTK_RANGE(randUVSeedSlider), 0.0f);

  gtk_container_add(GTK_CONTAINER(controlBox), randUVSeedSlider);

  uvScaleLabel = gtk_label_new("UV Scale");
  gtk_widget_set_halign(uvScaleLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), uvScaleLabel);
  // addWidgetList(support, colorMinLabel);

  uvScaleSlider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0f,
                                           2.0f, 0.000001f);
  gtk_range_set_value(GTK_RANGE(uvScaleSlider), 1.0f);
  control->uvScaleSlider = uvScaleSlider;

  gtk_container_add(GTK_CONTAINER(controlBox), uvScaleSlider);

  // addWidgetList(support, textureInfoButton);

  g_signal_connect(numCpySlider, "value-changed", G_CALLBACK(numCpyChanged),
                   control);
  g_signal_connect(dimensionButton, "clicked",
                   G_CALLBACK(dimensionButtonClicked), control);
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

  gtk_widget_show_all(controlBox);

  return control;
}

static gboolean glRender(GtkGLArea *area, GdkGLContext *context,
                         void *userData) {
  struct PatternData *user = (struct PatternData *)userData;
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(user->shaderProgram);
  glBindVertexArray(user->pattern->vao);
  glDrawArrays(GL_TRIANGLES, 0, user->pattern->vertexCounts);

  // drawing wireframe
  // glDrawArrays(GL_LINES, 0, user->pattern->vertexCounts);

  glFlush();
  return TRUE;
}

void initPattern(GtkWindow *mainWindow, GtkContainer *container,
                 GtkGLArea *glArea, GLuint shaderProgram) {
  struct PatternModel *pattern = patternModelNew(glArea, 50, 50, 1);
  void *user = (void *)patternDataNew(glArea, shaderProgram, pattern);
  struct ControlData *control = initControl(mainWindow, container, user);

  g_signal_connect(glArea, "render", G_CALLBACK(glRender), user);
}
