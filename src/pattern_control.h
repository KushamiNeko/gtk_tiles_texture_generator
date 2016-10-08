#ifndef PATTERN_CONTROL_H
#define PATTERN_CONTROL_H

#include <GL/glew.h>
#include <gtk-3.0/gtk/gtk.h>

#include <glib-2.0/glib.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "constant.h"

#include "../../general/src/general_helper.h"
#include "../../general/src/gl_helper.h"

#include "control_struct.h"
#include "pattern_model.h"

// void *initPatternControl(GtkWindow *mainWindow, GtkContainer *container,
//                         GtkGLArea *glArea, GLuint shaderProgram);

void patternControlWireframeSwitchToggled(GtkToggleButton *toggleButton,
                                          void *userData);

void patternControlWireframeColorSet(GtkColorButton *widget, void *userData);

void patternControlColorRangeChanged(GtkRange *range, void *userData);

void patternControlColorSeedChanged(GtkRange *range, void *userData);

void patternControlUvScaleChanged(GtkRange *range, void *userData);

void patternControlRandUVSeedChanged(GtkRange *range, void *userData);

void patternControlUvRotateToggled(GtkToggleButton *toggleButton,
                                   void *userData);

void patternControlOffsetControlChanged(GtkRange *range, void *userData);

void patternControlOffsetTypeChanged(GtkComboBox *widget, void *userData);

void patternControlOffsetControlTypeChanged(GtkComboBox *widget,
                                            void *userData);

void patternControlOffsetDirectionChanged(GtkComboBox *widget, void *userData);

void patternControlNumCpyChanged(GtkRange *range, void *userData);

void patternControlPatternTypeChanged(GtkComboBox *widget, void *userData);

void patternControlWireframeWidthChanged(GtkRange *range, void *userData);

void patternControlEntryBufferInserted(GtkEntryBuffer *buffer, guint position,
                                       gchar *chars, guint nChars,
                                       void *userData);

void patternControlDimensionButtonClicked(GtkButton *button, void *userData);

void patternControlTextureInfoButtonClicked(GtkButton *button, void *userData);

void patternControlRenderPathInfoButtonClicked(GtkButton *button,
                                               void *userData);

void patternControlRenderButtonClicked(GtkButton *button, void *userData);

gboolean patternControlGlRender(GtkGLArea *area, GdkGLContext *context,
                                void *userData);

void *patternControlDataInit(GtkGLArea *glArea, GLuint shaderProgram,
                             struct PatternModel *pattern);
#endif
