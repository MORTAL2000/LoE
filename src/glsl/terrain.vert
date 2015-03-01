// Copyright (c) 2014, Tamas Csala

#version 440

#include "engine/cdlod_terrain.vert"

uniform mat4 uProjectionMatrix, uCameraMatrix, uModelMatrix;
uniform vec2 CDLODTerrain_uTexSize;

out vec3  w_vNormal;
out vec3  c_vPos, w_vPos;
out vec2  vTexCoord;
out mat3  vNormalMatrix;
out float vLevel, vMorph;

int CDLODTerrain_uLevel;
float CDLODTerrain_fetchHeight(vec2 tex_coord);

void main() {
  vec4 temp = CDLODTerrain_worldPos();
  vec3 w_pos = temp.xyz;
  vMorph = temp.w;
  w_pos.xz = clamp(w_pos.xz, vec2(1), CDLODTerrain_uTexSize - vec2(1));
  w_pos.y = CDLODTerrain_fetchHeight(w_pos.xz);

  vec2 tex_coord = CDLODTerrain_texCoord(w_pos);
  vec3 offseted_w_pos = (uModelMatrix * vec4(w_pos, 1)).xyz;


  w_vPos = offseted_w_pos;
  vTexCoord = tex_coord;

  vec4 c_pos = uCameraMatrix * vec4(offseted_w_pos, 1);
  c_vPos = vec3(c_pos);

  vec3 w_normal = CDLODTerrain_normal(w_pos);
  w_vNormal = w_normal;

  vNormalMatrix = CDLODTerrain_normalMatrix(w_normal);
  vLevel = CDLODTerrain_uLevel;
  gl_Position = uProjectionMatrix * c_pos;
}
