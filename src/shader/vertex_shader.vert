#version 410

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;
layout(location = 2) in vec2 vertex_st;

uniform int wireframeDraw;
layout(location = 3) in vec3 vertex_wireframe;

out vec3 color;

out vec2 texture_coordinates;

void main() {
  color = vertex_color;

  texture_coordinates = vertex_st;

  if (wireframeDraw == 1) {
    gl_Position = vec4(vertex_wireframe, 1.0);
  } else {
    gl_Position = vec4(vertex_position, 1.0);
  }
}
