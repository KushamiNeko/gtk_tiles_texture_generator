//#include <glib-2.0/glib.h>
#include <gtk-3.0/gtk/gtk.h>

#include <GL/glew.h>
//#include <stdarg.h>

#include "constant.h"

//#include "pattern_control.h"
#include "main_window.h"

#include "../../general/src/general_helper.h"

////////////////////////////////////////////////////////////////////////

static GLuint constructShaderProgram(const char *vertFile,
                                     const char *fragFile) {
  const char *vertexShader = fileReadContents(vertFile);
  const char *fragmentShader = fileReadContents(fragFile);

  GLuint shaderProgram = glCreateProgram();
  generateShader(&shaderProgram, vertexShader, GL_VERTEX_SHADER);
  generateShader(&shaderProgram, fragmentShader, GL_FRAGMENT_SHADER);

  LINK_GL_SHADER_PROGRAM(shaderProgram);

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
    // return;
    exit(EXIT_FAILURE);
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

static void activate() {
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

  gtk_widget_set_hexpand(mainGL, FALSE);
  gtk_widget_set_vexpand(mainGL, FALSE);
  gtk_widget_set_halign(mainGL, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(mainGL, GTK_ALIGN_CENTER);
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

  gtk_widget_show_all(GTK_WIDGET(mainWindow));

  GLuint mainShaderProgram =
      constructShaderProgram(VERTEX_SHADER, FRAGMENT_SHADER);

  initPatternControl(GTK_WINDOW(mainWindow), GTK_CONTAINER(controlerBox),
                     GTK_GL_AREA(mainGL), mainShaderProgram);

  // we need to fully initialize the gtk components before we perform further
  // construct
}

int main(int argc, char **argv) {
  gtk_init(&argc, &argv);

  activate();
  gtk_main();

  return 0;
}
