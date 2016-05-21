#ifndef GL_HELPER_H
#define GL_HELPER_H

#define __GL_DEBUG 1
#define __GL_VIEWPORT 2

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include <stb_image.h>

GLuint generateVBO(const GLuint *vao, const int point_count,
                   const int vector_size, const GLfloat *data_array,
                   const int loc);

void setVBOData(const GLuint *vbo, const int point_count, const int vector_size,
                const GLfloat *dataArray);

char *readShader(const char *file);

void generateShader(const GLuint *shader_program, const char *shader_file,
                    const GLenum shader_type);
#if __GL_DEBUG
void programLinkCheck(GLuint program);
// {
//  g_print("glLinkProgram check\n");
//  glLinkProgram(program);
//
//  // error of program link check
//  int params = -1;
//  glGetProgramiv(program, GL_LINK_STATUS, &params);
//  if (GL_TRUE != params) {
//    g_printf("ERROR: could not link shader program with index %u\n", program);
//    _print_program_info_log(program);
//    exit(1);
//  }
//}

void shaderCompileCheck(GLuint shader);
//{
//  g_printf("glCompileShader check\n");
//  glCompileShader(shader);
//
//  // check for shader compile error
//  int params = -1;
//  glGetShaderiv(shader, GL_COMPILE_STATUS, &params);
//  if (GL_TRUE != params) {
//    g_printf("ERROR: GL Sahder %i did not compile\n", shader);
//    _print_shader_info_log(shader);
//    exit(1);
//  }
//}

#define __LinkProgram(expr) programLinkCheck(expr)
#define __CompileShader(expr) shaderCompileCheck(expr)
#else
#define __LinkProgram(expr) glLinkProgram(expr)
#define __CompileShader(expr) glCompileShader(expr)
#endif

#endif
