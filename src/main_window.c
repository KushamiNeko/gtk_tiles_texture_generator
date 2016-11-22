#include "main_window.h"

#ifdef UNIT_TESTING
#include <cmockery/cmockery_override.h>
#endif

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
  struct ControlData *control =
      DEFENSE_MALLOC(sizeof(struct ControlData), mallocFailAbort, NULL);

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
  //gchar *pattern02 = "Pattern02";
  //gchar *pattern03 = "Pattern03";

  patternTypeComboBox = gtk_combo_box_text_new();
  gtk_widget_set_size_request(patternTypeComboBox, CONTROL_BOX_WIDTH, 35);
  gtk_container_add(GTK_CONTAINER(controlBox), patternTypeComboBox);

  control->patternTypeComboBox = patternTypeComboBox;

  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(patternTypeComboBox),
                                 pattern01);

  //  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(patternTypeComboBox),
  //                                 pattern02);
  //
  //  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(patternTypeComboBox),
  //                                 pattern03);

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

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wireframeSwitch), TRUE);
  // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wireframeSwitch),
  //                             control->patternData->wireframeSwitch);
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

  g_signal_connect(patternTypeComboBox, "changed",
                   G_CALLBACK(patternControlPatternTypeChanged), control);

  g_signal_connect(widthEntryBuffer, "inserted-text",
                   G_CALLBACK(patternControlEntryBufferInserted), control);

  g_signal_connect(heightEntryBuffer, "inserted-text",
                   G_CALLBACK(patternControlEntryBufferInserted), control);

  g_signal_connect(numCpySlider, "value-changed",
                   G_CALLBACK(patternControlNumCpyChanged), control);

  g_signal_connect(dimensionButton, "clicked",
                   G_CALLBACK(patternControlDimensionButtonClicked), control);

  g_signal_connect(offsetTypeComboBox, "changed",
                   G_CALLBACK(patternControlOffsetTypeChanged), control);

  g_signal_connect(offsetDirectionComboBox, "changed",
                   G_CALLBACK(patternControlOffsetDirectionChanged), control);

  g_signal_connect(offsetControlTypeComboBox, "changed",
                   G_CALLBACK(patternControlOffsetControlTypeChanged), control);

  g_signal_connect(offsetControlSlider, "value-changed",
                   G_CALLBACK(patternControlOffsetControlChanged), control);

  g_signal_connect(wireframeSwitch, "toggled",
                   G_CALLBACK(patternControlWireframeSwitchToggled), control);

  g_signal_connect(wireframeWidthSlider, "value-changed",
                   G_CALLBACK(patternControlWireframeWidthChanged), control);

  g_signal_connect(wireframeColorChooser, "color-set",
                   G_CALLBACK(patternControlWireframeColorSet), control);

  g_signal_connect(textureInfoButton, "clicked",
                   G_CALLBACK(patternControlTextureInfoButtonClicked), control);

  g_signal_connect(randUVSeedSlider, "value-changed",
                   G_CALLBACK(patternControlRandUVSeedChanged), control);

  g_signal_connect(uvScaleSlider, "value-changed",
                   G_CALLBACK(patternControlUvScaleChanged), control);

  g_signal_connect(uvRotateCheckButton, "toggled",
                   G_CALLBACK(patternControlUvRotateToggled), control);

  g_signal_connect(colorSeedSlider, "value-changed",
                   G_CALLBACK(patternControlColorSeedChanged), control);

  g_signal_connect(colorMinSlider, "value-changed",
                   G_CALLBACK(patternControlColorRangeChanged), control);

  g_signal_connect(colorMaxSlider, "value-changed",
                   G_CALLBACK(patternControlColorRangeChanged), control);

  g_signal_connect(renderSizeEntryBuffer, "inserted-text",
                   G_CALLBACK(patternControlEntryBufferInserted), control);

  g_signal_connect(renderPathInfoButton, "clicked",
                   G_CALLBACK(patternControlRenderPathInfoButtonClicked),
                   control);

  g_signal_connect(renderButton, "clicked",
                   G_CALLBACK(patternControlRenderButtonClicked), control);

  gtk_widget_show_all(scrollWindow);

  return control;
}

void *initPatternControl(GtkWindow *mainWindow, GtkContainer *container,
                         GtkGLArea *glArea, GLuint shaderProgram) {
  gtk_gl_area_make_current(glArea);

  struct PatternModel *pattern = patternModelNew(glArea, 50, 50, 1, 0);
  void *user = (void *)patternControlDataInit(glArea, shaderProgram, pattern);
  struct ControlData *control = initControl(mainWindow, container, user);

  g_signal_connect(glArea, "render", G_CALLBACK(patternControlGlRender),
                   control);

  glUseProgram(shaderProgram);
  gtk_gl_area_attach_buffers(glArea);

  glFlush();
  gtk_gl_area_queue_render(glArea);

  return (void *)control;
}
