#include "../header/pattern_alpha.h"
#include "pattern_alpha_data.c"

#define WIDGET_MARGIN 10

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

static void colorSeedChanged(GtkRange *range, void *userData) {
  struct patternData *user = (struct patternData *)userData;
  double seed = gtk_range_get_value(range);

  setPatternRandColor(user->pattern, seed);
  // directly update the specific memory allocated for the data
  setVBOData(&user->pattern->colorVBO, user->pattern->vertexCounts, 3,
             user->pattern->vertexColor);

  // queue openGL render
  gtk_gl_area_queue_render(user->glArea);
}

static void numCopyChanged(GtkRange *range, void *userData) {
  struct patternData *user = (struct patternData *)userData;
  unsigned int cpy = (unsigned int)gtk_range_get_value(range);

  struct patternModel *pattern = patternConstruct(
      user->glArea, user->pattern->sizeX, user->pattern->sizeY, cpy);

  setPatternRandColor(pattern, cpy);
  freePatternModel(user->pattern);

  user->pattern = pattern;

  gtk_gl_area_queue_render(user->glArea);
}

static void *initControl(GtkContainer *container, void *user) {
  GtkWidget *controlBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, BOX_SPACE);

  gtk_container_add(container, controlBox);

  GtkWidget *numCopyLabel = gtk_label_new("number copys");
  gtk_widget_set_halign(numCopyLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), numCopyLabel);

  GtkWidget *numCopySlider =
      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1.0f, 15.0f, 1.00f);

  g_signal_connect(numCopySlider, "value-changed", G_CALLBACK(numCopyChanged),
                   user);

  gtk_container_add(GTK_CONTAINER(controlBox), numCopySlider);

  GtkWidget *randColorSeedLabel = gtk_label_new("random color seed");
  gtk_widget_set_halign(randColorSeedLabel, GTK_ALIGN_START);
  gtk_container_add(GTK_CONTAINER(controlBox), randColorSeedLabel);

  GtkWidget *randColorSeedSlider =
      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0f, 1.0f, 0.01f);

  g_signal_connect(randColorSeedSlider, "value-changed",
                   G_CALLBACK(colorSeedChanged), user);

  gtk_container_add(GTK_CONTAINER(controlBox), randColorSeedSlider);

  gtk_widget_show_all(controlBox);
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

void initPattern(GtkContainer *container, GtkGLArea *glArea,
                 GLuint shaderProgram) {
  struct patternModel *pattern = patternConstruct(glArea, 50, 25, 1);
  void *user = (void *)newPatternData(glArea, shaderProgram, pattern);
  initControl(container, user);

  g_signal_connect(glArea, "render", G_CALLBACK(glRender), user);
}
