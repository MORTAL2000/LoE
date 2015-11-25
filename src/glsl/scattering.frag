// Copyright (c) 2015, Tamas Csala
// Based on GLtracy's shader

#version 330

#include "sky.frag"
#include "dof.frag"
#include "glow.frag"

out vec4 fragColor;

uniform ivec2 uTexSize;
uniform vec2 uResolution;
uniform vec3 uCamPos;
uniform mat3 uCameraMatrix;

// math const
const float PI = 3.14159265359;
const float DEG_TO_RAD = PI / 180.0;

// scatter const
const float K_R = 0.166;
const float K_M = 0.0025;
const float E = 14.3;                   // light intensity
const vec3  C_R = vec3(0.3, 0.7, 1.0);  // 1 / wavelength ^ 4
const float G_M = -0.85;                // Mie g

float R_INNER = uTexSize.x / 2 / PI;
float R = 1.02 * R_INNER;
float MAX = 10.0 * R;
float SCALE_H = 8.0 / (R - R_INNER);
float SCALE_L = 0.35 / (R - R_INNER);

const int NUM_OUT_SCATTER = 5;
const float FNUM_OUT_SCATTER = 5.0;

const int NUM_IN_SCATTER = 5;
const float FNUM_IN_SCATTER = 5.0;

// ray direction
vec3 ray_dir(float fov, vec2 size, vec2 pos) {
  vec2 xy = pos - size * 0.5;

  float cot_half_fov = tan((90.0 - fov * 0.5) * DEG_TO_RAD);
  float z = size.y * 0.5 * cot_half_fov;

  return normalize(vec3(xy, -z));
}

// ray intersects sphere
// x = -(b/2) +- sqrt((b/2)^2 - c) when 'a' is 1.0
vec2 ray_vs_sphere(vec3 rayOrigin, vec3 rayDir, float r) {
  // float a = dot(rayDir, rayDir); // so a == 1.0;
  float b_half = dot(rayOrigin, rayDir);
  float c = dot(rayOrigin, rayOrigin) - r * r;

  float discriminant_per_four = b_half * b_half - c;
  if (discriminant_per_four < 0.0) {
    return vec2(MAX, -MAX);
  }
  float d_half = sqrt(discriminant_per_four);

  return vec2(-b_half - d_half, -b_half + d_half);
}

// Mie
// g : (-0.75, -0.999)
//      3 * (1 - g^2)                  1 + c^2
// F = ----------------- * -------------------------------
//      2 * (2 + g^2)        (1 + g^2 - 2 * g * c)^(3/2)
float phase_mie(float g, float c, float cc) {
  float gg = g * g;

  float a = (1.0 - gg) * (1.0 + cc);

  float b = 1.0 + gg - 2.0 * g * c;
  b *= sqrt(b);
  b *= 2.0 + gg;

  return 1.5 * a / b;
}

// Reyleigh
// g : 0
// F = 3/4 * (1 + c^2)
float phase_reyleigh(float cc) {
  return 0.75 * (1.0 + cc);
}

float density(vec3 p){
  return exp(-(length(p) - R_INNER) * SCALE_H);
}

float optic(vec3 p, vec3 q) {
  vec3 step = (q - p) / FNUM_OUT_SCATTER;
  vec3 v = p + step * 0.5;

  float sum = 0.0;
  for (int i = 0; i < NUM_OUT_SCATTER; i++) {
    sum += density(v);
    v += step;
  }
  sum *= length(step) * SCALE_L;

  return sum;
}

vec3 in_scatter(vec3 o, vec3 dir, vec2 e, vec3 l) {
  float len = (e.y - e.x) / FNUM_IN_SCATTER;
  vec3 step = dir * len;
  vec3 p = o + dir * max(e.x, 0);
  vec3 v = p + step / 2.0;

  vec3 sum = vec3(0.0);
  for (int i = 0; i < NUM_IN_SCATTER; i++) {
    vec2 f = ray_vs_sphere(v, l, R);
    vec3 u = v + l * f.y;

    float n = (optic(p, v) + optic(v, u)) * (PI * 4.0);
    sum += density(v) * exp(-n * (K_R * C_R + K_M));

    v += step;
  }
  sum *= len * SCALE_L;

  float c  = dot(dir, -l);
  float cc = c * c;
  vec3 phase = K_R*C_R*phase_reyleigh(cc) + K_M*phase_mie(G_M, c, cc);

  float start_density = min(density(o)*max(dot(normalize(o), l) + 0.2, 0), 0.15);
  vec3 random_stuff_that_makes_the_output_look_good =
    25 * start_density * (len * SCALE_L / (len * SCALE_L + 1)) * C_R;

  float scale = smoothstep(0, R_INNER / 16, DistanceFromCamera());
  return scale * (random_stuff_that_makes_the_output_look_good + E*sum*phase);
}

vec3 Scattering() {
  vec3 rayDir = inverse(uCameraMatrix)
              * ray_dir(60.0, uResolution, gl_FragCoord.xy);

  vec2 e = ray_vs_sphere(uCamPos, rayDir, R);
  if (e.x > e.y || e.y < 0) {
    return vec3(0);
  }

  e.y = min(e.y, DistanceFromCamera());
  return in_scatter(uCamPos, rayDir, e, SunPos ());
}

void main() {
  FetchNeighbours();
  vec3 color = Glow() + DoF(CurrentPixel()) + Scattering();
  color = ToneMap(color);

  fragColor = vec4(clamp(color, vec3(0), vec3(1)), 1);
}
