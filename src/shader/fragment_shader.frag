#version 410

in vec3 color;

in vec2 texture_coordinates;

out vec4 frag_colour;

uniform sampler2D diff_tex;

uniform int wireframeDraw;
uniform vec3 wireframeColor;

void main() {

  if (wireframeDraw == 1) {
    frag_colour = vec4(wireframeColor, 1.0);
  } else {
    vec4 diff_texel = texture(diff_tex, texture_coordinates);
    frag_colour = vec4(color * diff_texel.xyz, 1.0);
    //frag_colour = vec4(diff_texel.xyz, 1.0);
  }
}
