#include "../header/gl_helper.h"

GLuint generateVBO(const GLuint *vao, const int pointCounts,
                   const int vectorSize, const GLfloat *dataArray,
                   const int loc) {
  // generate vbo
  GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * pointCounts * vectorSize,
               dataArray, GL_STATIC_DRAW);

  // bind to the specific vao
  glBindVertexArray(*vao);
  // to modified the vertex buffer objects, we need to bind the specific vbo
  // first
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // watch out the index and the vector size
  glVertexAttribPointer(loc, vectorSize, GL_FLOAT, GL_FALSE, 0, NULL);
  // enable the requested index to be used
  glEnableVertexAttribArray(loc);

  return vbo;
}

void setVBOData(const GLuint *vbo, const int pointCounts, const int vectorSize,
                const GLfloat *dataArray) {
  glBindBuffer(GL_ARRAY_BUFFER, *vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * pointCounts * vectorSize,
               dataArray, GL_STATIC_DRAW);
  // switch (dataType) {
  //  case GL_DOUBLE:
  //    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * pointCounts *
  //    vectorSize,
  //                 dataArray, GL_STATIC_DRAW);
  //    break;
  //  case GL_FLOAT:
  //    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * pointCounts *
  //    vectorSize,
  //                 dataArray, GL_STATIC_DRAW);
  //    break;
  //}
}

void generateShader(const GLuint *shaderProgram, const char *shaderFile,
                    const GLenum shaderType) {
  GLuint shader;
  switch (shaderType) {
    case GL_VERTEX_SHADER: {
      shader = glCreateShader(GL_VERTEX_SHADER);
      break;
    }
    case GL_FRAGMENT_SHADER: {
      shader = glCreateShader(GL_FRAGMENT_SHADER);
      break;
    }
  }
  glShaderSource(shader, 1, &shaderFile, NULL);

  // original GL compile shader function call
  // glCompileShader(fs);

  // wrapper function with gl shader compile and error check
  __CompileShader(shader);
  glAttachShader(*shaderProgram, shader);
}

#if __GL_DEBUG
void programLinkCheck(GLuint program) {
  printf("glLinkProgram check\n");
  glLinkProgram(program);

  // error of program link check
  int params = -1;
  glGetProgramiv(program, GL_LINK_STATUS, &params);
  if (GL_TRUE != params) {
    printf("ERROR: could not link shader program with index %u\n", program);
    //_print_program_info_log(program);
    exit(1);
  }
}

void shaderCompileCheck(GLuint shader) {
  printf("glCompileShader check\n");
  glCompileShader(shader);

  // check for shader compile error
  int params = -1;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &params);
  if (GL_TRUE != params) {
    printf("ERROR: GL Sahder %i did not compile\n", shader);
    //_print_shader_info_log(shader);
    exit(1);
  }
}

#endif

char *readShader(const char *file) {
  // read file contents into a char *
  char *shader_code = NULL;
  long length;
  FILE *f = fopen(file, "rb");
  if (f) {
    fseek(f, 0, SEEK_END);
    length = ftell(f);
    rewind(f);
    shader_code = (char *)calloc(1, length * sizeof(char));
    if (shader_code) {
      fread(shader_code, sizeof(char), length, f);
    }
    fclose(f);

    // add string end point "\0" to the end of the array
    shader_code[length] = 0;
  }

  return shader_code;
}
