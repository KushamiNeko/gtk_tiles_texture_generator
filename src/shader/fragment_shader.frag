#version 410

in vec3 color;

in vec2 texture_coordinates;

out vec4 frag_colour;

uniform sampler2D diff_tex;

uniform int wireframeDraw;
uniform vec3 wireframeColor;

void bias(in float value, in float b, out float result) {
  result = (b > 0) ? pow(value, log(b) / log(0.5)) : 0;
}

// void gain(in float value, in float g, out float result) {
//  if (value < 0.5) {
//    bias(2 * value, 1 - g, result);
//  } else {
//    float br = 0;
//    bias(2 - (2 * value), 1 - g, br);
//    result = 2 - br;
//  }
//}

void main() {
  if (wireframeDraw == 1) {
    frag_colour = vec4(wireframeColor, 1.0);
  } else {
    vec4 diff_texel = texture(diff_tex, texture_coordinates);

    // frag_colour = vec4(color * diff_texel.xyz, 1.0);

    if (color.x < 1) {
      float r, g, b;
      float val = color.x / 2.0f;

      bias(val, diff_texel.x, r);
      bias(val, diff_texel.y, g);
      bias(val, diff_texel.z, b);

      frag_colour = vec4(r, g, b, 1.0);
    } else {
      frag_colour = vec4(color * diff_texel.xyz, 1.0);
    }

    // frag_colour = vec4(diff_texel.xyz, 1.0);
  }
}
