#pragma once

#include "Utils/stdafx.h"

extern LPDIRECT3DDEVICE9 g_pd3dDevice;

void glInit();
void glPopMatrix();
void glPushMatrix();
void glSetMatrix(D3DXMATRIX * matrix);
void glScalef(float x, float y, float z);
void glTranslatef(float x, float y, float z);
void glRotatef(float x, float y, float z);
void glApply();
