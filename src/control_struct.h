#ifndef CONTROL_STRUCT_H
#define CONTROL_STRUCT_H

struct ControlData {
  void *patternData;

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

// struct PatternData {
//  GtkGLArea *glArea;
//
//  gchar *textureFile;
//  GLuint tex;
//  GLint texLoc;
//
//  gchar *renderPath;
//
//  GLuint shaderProgram;
//
//  struct PatternModel *pattern;
//
//  GLuint wireframeSwitch;
//
//  GLint wireframeDrawUniformLoc;
//  GLint wireframeDraw;
//
//  GLint wireframeColorUniformLoc;
//  GLfloat *wireframeColor;
//};
//
// struct PatternData *patternDataNew(GtkGLArea *glArea, GLuint shaderProgram,
//                                   struct PatternModel *pattern);

#endif
