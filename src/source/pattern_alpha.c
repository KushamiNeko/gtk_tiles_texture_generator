#include "../header/pattern_alpha.h"
#include "pattern_alpha_data.c"

#include "../header/constant.h"

struct patternData {
  GtkGLArea *glArea;
  GLuint shaderProgram;

  struct patternModel *pattern;
};

static struct patternData *newPatternData(GtkGLArea *glArea,
                                          GLuint shaderProgram,
                                          struct patternModel *pattern) {
  struct patternData *re = calloc(1, sizeof(struct patternData));

  re->glArea = glArea;
  re->shaderProgram = shaderProgram;
  re->pattern = pattern;

  return re;
}

void freePatternData(struct patternData *data) {
  glDeleteProgram(data->shaderProgram);
  freePatternModel(data->pattern);
}

struct widgetList {
  GtkWidget *widget;
  struct widgetList *next;
};

struct widgetList *newWidgetList() {
  struct widgetList *re = calloc(1, sizeof(struct widgetList));
  re->widget = NULL;
  re->next = NULL;

  return re;
}

void addWidgetList(struct widgetList **list, GtkWidget *widget) {
  struct widgetList *re = calloc(1, sizeof(struct widgetList));
  re->widget = widget;
  re->next = *list;

  *list = re;

  // struct widgetList **p = &list;
  // while (1) {
  //  if ((*p)->next == NULL) {
  //    (*p)->next = re;
  //    return;
  //  }

  //  p = &((*p)->next);
  //}
}

void freeWidgetList(struct widgetList *list) {
  struct widgetList **current = &list;
  struct widgetList **next = &((*current)->next);

  while (1) {
    gtk_widget_destroy((*current)->widget);
    free(*current);
    current = next;
    if (*current == NULL) {
      return;
    }

    next = &((*current)->next);
  }
}

struct controlData {
  struct patternData *patternData;
  struct widgetList *support;

  GtkWindow *mainWindow;

  GtkWidget *widthEntry;
  GtkWidget *heightEntry;

  GtkWidget *numCpySlider;

  GtkWidget *colorSeedSlider;
  GtkWidget *colorMinSlider;
  GtkWidget *colorMaxSlider;

  GtkWidget *textureInfoLabel;
};

void freeControlData(struct controlData *data) {
  freePatternData(data->patternData);

  //  gtk_widget_destroy(data->widthEntry);
  //  gtk_widget_destroy(data->heightEntry);
  //  gtk_widget_destroy(data->numCpySlider);
  //  gtk_widget_destroy(data->colorSeedSlider);
  //  gtk_widget_destroy(data->colorMinSlider);
  //  gtk_widget_destroy(data->colorMaxSlider);
  //  gtk_widget_destroy(data->textureInfoLabel);

  freeWidgetList(data->support);

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();
}

static void colorSeedChanged(GtkRange *range, void *userData) {
  struct controlData *control = (struct controlData *)userData;
  struct patternData *userPattern = control->patternData;

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  // re-construct the color of whole pattern units
  initPatternRandColor(userPattern->pattern, colorMin, colorMax);
  // directly update the specific memory allocated for the data
  setVBOData(&userPattern->pattern->colorVBO,
             userPattern->pattern->vertexCounts, 3,
             userPattern->pattern->vertexColor);

  // queue openGL render
  gtk_gl_area_queue_render(userPattern->glArea);
}

static void numCpyChanged(GtkRange *range, void *userData) {
  struct controlData *control = (struct controlData *)userData;
  struct patternData *userPattern = control->patternData;
  unsigned int cpy = (unsigned int)gtk_range_get_value(range);

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  struct patternModel *pattern =
      patternConstruct(userPattern->glArea, userPattern->pattern->sizeX,
                       userPattern->pattern->sizeY, cpy);

  freePatternModel(userPattern->pattern);

  userPattern->pattern = pattern;
  // re-construct the color of whole pattern units
  initPatternRandColor(userPattern->pattern, colorMin, colorMax);

  setVBOData(&userPattern->pattern->colorVBO,
             userPattern->pattern->vertexCounts, 3,
             userPattern->pattern->vertexColor);
  // queue openGL render
  gtk_gl_area_queue_render(userPattern->glArea);
}

static void colorRangeChanged(GtkRange *range, void *userData) {
  struct controlData *control = (struct controlData *)userData;
  struct patternData *userPattern = control->patternData;

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  // only re-generate GL color data and fit it into new range
  fitPatternRandColor(userPattern->pattern, colorMin, colorMax);
  setVBOData(&userPattern->pattern->colorVBO,
             userPattern->pattern->vertexCounts, 3,
             userPattern->pattern->vertexColor);

  // queue openGL render
  gtk_gl_area_queue_render(userPattern->glArea);
}

static void dimensionButtonClicked(GtkButton *button, void *userData) {
  struct controlData *control = (struct controlData *)userData;
  struct patternData *userPattern = control->patternData;

  const gchar *widthChar = gtk_entry_get_text(GTK_ENTRY(control->widthEntry));
  const gchar *heightChar = gtk_entry_get_text(GTK_ENTRY(control->heightEntry));

  if (strlen(widthChar) == 0 || strlen(heightChar) == 0) {
    return;
  }

  GLfloat width = atof(widthChar);
  GLfloat height = atof(heightChar);

  double colorMin = gtk_range_get_value(GTK_RANGE(control->colorMinSlider));
  double colorMax = gtk_range_get_value(GTK_RANGE(control->colorMaxSlider));

  struct patternModel *pattern =
      patternConstruct(userPattern->glArea, width, height, 1);

  freePatternModel(userPattern->pattern);

  userPattern->pattern = pattern;

  // only re-generate GL color data and fit it into new range
  fitPatternRandColor(userPattern->pattern, colorMin, colorMax);
  setVBOData(&userPattern->pattern->colorVBO,
             userPattern->pattern->vertexCounts, 3,
             userPattern->pattern->vertexColor);

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
  struct controlData *control = (struct controlData *)userData;
  struct patternData *userPattern = control->patternData;

  gchar *filePath = getTextureFile(control->mainWindow);
  gchar *baseName = getBaseName(filePath);

  if (filePath) {
    userPattern->pattern->texturePath = filePath;
    gtk_label_set_text(GTK_LABEL(control->textureInfoLabel), baseName);
  }
}

static void *addSeparator(GtkContainer *container,
                          const GtkOrientation orientation,
                          const unsigned int width, const unsigned int height) {
  GtkWidget *separator = gtk_separator_new(orientation);
  gtk_widget_set_size_request(separator, width, height);
  gtk_container_add(container, separator);

  return separator;
}

static struct controlData *initControl(GtkWindow *mainWindow,
                                       GtkContainer *container, void *user) {
  // initialize random generator here instead of every time the value changed
  srand(time(NULL));

  // alloc memory for controlData used by signal function
  struct controlData *control = calloc(1, sizeof(struct controlData));

  control->patternData = (struct patternData *)user;

  control->support = NULL;
  struct widgetList **support = &(control->support);

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

  controlBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, BOX_SPACE);
  addWidgetList(support, controlBox);

  gtk_container_add(container, controlBox);

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

  textureInfoLabel = gtk_label_new("choose a texture file");
  gtk_label_set_max_width_chars(GTK_LABEL(textureInfoLabel), 30);
  gtk_label_set_line_wrap(GTK_LABEL(textureInfoLabel), TRUE);
  gtk_label_set_line_wrap_mode(GTK_LABEL(textureInfoLabel), PANGO_WRAP_CHAR);
  gtk_container_add(GTK_CONTAINER(controlBox), textureInfoLabel);

  control->textureInfoLabel = textureInfoLabel;

  textureInfoButton = gtk_button_new_with_label("Choose File");
  gtk_container_add(GTK_CONTAINER(controlBox), textureInfoButton);

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

  gtk_widget_show_all(controlBox);

  return control;
}

static gboolean glRender(GtkGLArea *area, GdkGLContext *context,
                         void *userData) {
  struct patternData *user = (struct patternData *)userData;
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(user->shaderProgram);
  glBindVertexArray(user->pattern->vao);
  glDrawArrays(GL_TRIANGLES, 0, user->pattern->vertexCounts);

  glFlush();
  return TRUE;
}

void initPattern(GtkWindow *mainWindow, GtkContainer *container,
                 GtkGLArea *glArea, GLuint shaderProgram) {
  struct patternModel *pattern = patternConstruct(glArea, 50, 50, 1);
  void *user = (void *)newPatternData(glArea, shaderProgram, pattern);
  struct controlData *control = initControl(mainWindow, container, user);

  g_signal_connect(glArea, "render", G_CALLBACK(glRender), user);
}
