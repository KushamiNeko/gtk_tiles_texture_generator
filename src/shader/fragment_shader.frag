#version 410

in vec3 color;
in vec2 texture_coordinates;

out vec4 frag_colour;

uniform sampler2D diff_tex;

void main() {
  vec4 diff_texel = texture(diff_tex, texture_coordinates);
  frag_colour = vec4(color * diff_texel.xyz, 1.0);
  //frag_colour = diff_texel;
  //frag_colour = vec4(texture_coordinates, 0.0, 1.0);
  //frag_colour = vec4(color, 1.0);
}
