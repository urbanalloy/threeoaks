// include file for screensaver template
#pragma once

#include "waterfield.h" 


void LoadSettings(LPDIRECT3DDEVICE9 device);
void SetAnimation();

struct WaterSettings
{
  //C_Sphere * sphere;
  WaterField * waterField;
  int effectType;
  int frame;
  int nextEffectTime;
  int nextTextureTime;
  int effectCount;
  float scaleX;
  bool isWireframe;
  bool isTextureMode;
  char szTextureSearchPath[1024];
};
