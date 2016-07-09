#include <gtk-3.0/gtk/gtk.h>

#include <GL/glew.h>
#include <stdarg.h>

#include "header/gl_helper.h"
#include "header/pattern_alpha.h"
#include "header/constant.h"

#include "../../general/src/header/general_helper.h"

struct PatternControl {
  void *(*initPatternControl)(GtkWindow *mainWindow, GtkContainer *container,
                              GtkGLArea *glArea, GLuint shaderProgram);
  // void (*freePattern)(void *control);

  void (*constructPatternModel)(struct PatternControl *control);
  void (*freePatternModel)(struct PatternControl *control);

  void *patternControl;
  gchar *patternName;
};

static void initPatternControl(struct PatternControl *control,
                               GtkWindow *mainWindow, GtkContainer *container,
                               GtkGLArea *glArea, GLuint shaderProgram) {
  control->patternControl =
      control->initPatternControl(mainWindow, container, glArea, shaderProgram);
}

static void constructPatternModel(struct PatternControl *control) {
  control->constructPatternModel(control->patternControl);
}

static void freePatternModel(struct PatternControl *control) {
  control->freePatternModel(control->patternControl);
}

// static void freePattern(struct PatternControl *control) {
//  control->freePattern(control);
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////

static GLuint constructShaderProgram(const char *vertFile,
                                     const char *fragFile) {
  const char *vertexShader = readFile(vertFile);
  const char *fragmentShader = readFile(fragFile);

  GLuint shaderProgram = glCreateProgram();
  generateShader(&shaderProgram, vertexShader, GL_VERTEX_SHADER);
  generateShader(&shaderProgram, fragmentShader, GL_FRAGMENT_SHADER);

  __LinkProgram(shaderProgram);

  return shaderProgram;
}

static void glRealize(GtkGLArea *area) {
  g_print("initialize GL area\n");
  gtk_gl_area_make_current(area);

  glewExperimental = GL_TRUE;
  glewInit();

  glHint(GL_TEXTURE_COMPRESSION_HINT, GL_NICEST);
  glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

  const GLubyte *renderer = glGetString(GL_RENDERER);
  const GLubyte *version = glGetString(GL_VERSION);
  g_print("OpenGL Renderer: %s\n", renderer);
  g_print("OpenGL Version: %s\n", version);

  if (gtk_gl_area_get_error(area) != NULL) {
    g_print("GL Area initialization failed!\n");
    return;
  }

  // enable depth testing
  glEnable(GL_DEPTH_TEST);
  // depth testing interprets a smaller value as "closer"
  glDepthFunc(GL_LESS);

  // enable back face culling
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);
   glFrontFace(GL_CCW);

  glClearColor(0, 0, 0, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();
}

struct PatternType {
  struct PatternControl **patternCollection;
  unsigned int currentID;

  GtkWindow *window;
  GtkContainer *mainBox;
  GtkContainer *container;
  GtkGLArea *glArea;
  GLuint shaderProgram;
};

struct PatternType *patternTypeNew(struct PatternControl **pattern,
                                   GtkWindow *window, GtkContainer *mainBox,
                                   GtkContainer *container, GtkGLArea *glArea,
                                   GLuint shaderProgram) {
  struct PatternType *re = defenseCalloc(1, sizeof(struct PatternType));

  re->patternCollection = pattern;
  re->currentID = 0;

  re->window = window;
  re->mainBox = mainBox;
  re->container = container;
  re->glArea = glArea;
  re->shaderProgram = shaderProgram;

  return re;
}

static void comboBoxChanged(GtkComboBox *widget, void *userData) {
  struct PatternType *user = (struct PatternType *)userData;
  struct PatternControl **patternCollection =
      (struct PatternControl **)user->patternCollection;

  g_print("current: %d, %s\n", user->currentID,
          patternCollection[user->currentID]->patternName);

  // freePattern(pattern[user->currentID]);
  int newPatternID = gtk_combo_box_get_active(widget);

  // freePatternModel(*patternCollection);

  ////_print("current: %d, %s\n", user->currentID,
  ////       pattern[user->currentID]->patternName);
  //// g_print("new: %d, %s\n", newPatternID,
  //// pattern[newPatternID]->patternName);

  //  gtk_widget_hide(GTK_WIDGET(user->mainBox));
  //  freePattern(pattern[user->currentID]);
  //   gtk_widget_show_all(GTK_WIDGET(user->mainBox));
  //
  //  initPattern(pattern[newPatternID], user->window, user->container,
  //              user->glArea, user->shaderProgram);

  user->currentID = newPatternID;
}

static void activate(struct PatternControl **patternCollection) {
  // building the general gtk components
  GtkWidget *mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(mainWindow), "Tile Texture Generator");
  gtk_window_set_position(GTK_WINDOW(mainWindow), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable(GTK_WINDOW(mainWindow), FALSE);
  g_signal_connect(GTK_WIDGET(mainWindow), "destroy", G_CALLBACK(gtk_main_quit),
                   NULL);

  GtkWidget *mainBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, BOX_SPACE);
  gtk_widget_set_margin_bottom(GTK_WIDGET(mainBox), WIDGET_MARGIN);
  gtk_widget_set_margin_top(GTK_WIDGET(mainBox), WIDGET_MARGIN);
  gtk_widget_set_margin_start(GTK_WIDGET(mainBox), WIDGET_MARGIN);
  gtk_widget_set_margin_end(GTK_WIDGET(mainBox), WIDGET_MARGIN);
  gtk_container_add(GTK_CONTAINER(mainWindow), mainBox);

  GtkWidget *mainGL = gtk_gl_area_new();
  gtk_container_add(GTK_CONTAINER(mainBox), mainGL);
  gtk_gl_area_set_required_version(GTK_GL_AREA(mainGL), GL_VERSION_MAJOR,
                                   GL_VERSION_MINOR);
  gtk_widget_set_size_request(mainGL, WINDOW_HEIGHT, WINDOW_HEIGHT);

  // instead of entering GL render loop, we queue the render command when we
  // actually need it
  gtk_gl_area_set_auto_render(GTK_GL_AREA(mainGL), FALSE);

  // function for initializing Gtk GL area
  g_signal_connect(mainGL, "realize", G_CALLBACK(glRealize), NULL);

  GtkWidget *controlerBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, BOX_SPACE);
  gtk_widget_set_margin_bottom(controlerBox, WIDGET_MARGIN);
  gtk_widget_set_margin_top(controlerBox, WIDGET_MARGIN);
  gtk_widget_set_margin_start(controlerBox, WIDGET_MARGIN);
  gtk_widget_set_margin_end(controlerBox, WIDGET_MARGIN);

  gtk_widget_set_size_request(controlerBox, CONTROL_BOX_WIDTH, WINDOW_HEIGHT);
  gtk_container_add(GTK_CONTAINER(mainBox), controlerBox);

  GtkWidget *patternType = gtk_combo_box_text_new();
  gtk_widget_set_size_request(patternType, CONTROL_BOX_WIDTH, 35);
  gtk_container_add(GTK_CONTAINER(controlerBox), patternType);

  gtk_widget_show_all(GTK_WIDGET(mainWindow));

  // we need to fully initialize the gtk components before we perform further
  // construct

  struct PatternControl **pattern = patternCollection;

  while (*pattern != NULL) {
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(patternType),
                                   (*pattern)->patternName);
    pattern++;
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(patternType), 0);

  GLuint mainShaderProgram =
      constructShaderProgram(VERTEX_SHADER, FRAGMENT_SHADER);

  struct PatternType *user = patternTypeNew(
      patternCollection, GTK_WINDOW(mainWindow), GTK_CONTAINER(mainBox),
      GTK_CONTAINER(controlerBox), GTK_GL_AREA(mainGL), mainShaderProgram);

  g_signal_connect(patternType, "changed", G_CALLBACK(comboBoxChanged),
                   (void *)user);

  //  initPattern(pattern, GTK_WINDOW(mainWindow), GTK_CONTAINER(controlerBox),
  //              GTK_GL_AREA(mainGL), mainShaderProgram);

  initPatternControl(*patternCollection, GTK_WINDOW(mainWindow),
                     GTK_CONTAINER(controlerBox), GTK_GL_AREA(mainGL),
                     mainShaderProgram);

  constructPatternModel(*patternCollection);

  // freePattern((struct PatternControl *)patternAlpha);
  // freePattern(*patternCollection);

  // initPattern(patternCollection[1], GTK_WINDOW(mainWindow),
  //            GTK_CONTAINER(controlerBox), GTK_GL_AREA(mainGL),
  //            mainShaderProgram);

  // initialization of the pattern
  //  initPattern(GTK_WINDOW(mainWindow), GTK_CONTAINER(controlerBox),
  //              GTK_GL_AREA(mainGL), mainShaderProgram);
}

struct PatternControl **collectPattern(const unsigned int numArgs, ...) {
  struct PatternControl **re = (struct PatternControl **)defenseCalloc(
      1, sizeof(struct PatternControl *) * (numArgs + 1));

  va_list list;

  va_start(list, numArgs);

  for (int i = 0; i < numArgs; i++) {
    re[i] = va_arg(list, struct PatternControl *);
  }

  re[numArgs] = NULL;
  return re;
}

int main(int argc, char **argv) {
  gtk_init(&argc, &argv);

  void *patternAlpha = patternAlphaNew("Pattern 01");
  // void *patternBeta = patternAlphaNew("Pattern 02");

  struct PatternControl **patternCollection = collectPattern(1, patternAlpha);

  activate(patternCollection);
  gtk_main();

  return 0;
}
