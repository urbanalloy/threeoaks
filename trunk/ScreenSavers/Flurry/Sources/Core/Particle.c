///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry
//
// Implementation of the Particle class
//
// Copyright (c) 2002, Calum Robinson
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// o Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// o Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// o Neither the name of the author nor the names of its contributors may
//   be used to endorse or promote products derived from this software
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////////////////////

#include "Std.h"
#include "Particle.h"

// the math was easier in 2D - so 2D it is
__private_extern__ void DrawParticle(Particle *p) 
{
	int i;
    float screenx = (p->x * info->sys_glWidth / p->z) + info->sys_glWidth * 0.5f;
    float screeny = (p->y * info->sys_glWidth / p->z) + info->sys_glHeight * 0.5f;
    float oldscreenx = (p->oldx * info->sys_glWidth / p->oldz) + info->sys_glWidth * 0.5f;
    float oldscreeny = (p->oldy * info->sys_glWidth / p->oldz) + info->sys_glHeight * 0.5f;

    if (p->z < 100.0f ||
		screenx > info->sys_glWidth + 100.0f || screenx < -100.0f ||
		screeny > info->sys_glHeight + 100.0f || screeny < -100.0f) {
	    // clipping tests; if clipped, reset to sane state
		InitParticle(p);
    }

    for (i = 0; i < 4; i++) {
		info->starfieldColor[info->starfieldColorIndex++] = p->r;
		info->starfieldColor[info->starfieldColorIndex++] = p->g;
		info->starfieldColor[info->starfieldColorIndex++] = p->b;
		info->starfieldColor[info->starfieldColorIndex++] = 1.0f;
	}
    
    if (++p->animFrame == 64) {
        p->animFrame = 0;
    }
    
    {
        float dx = (screenx-oldscreenx);
        float dy = (screeny-oldscreeny);
        float m = FastDistance2D(dx, dy);
        float u0 = (p->animFrame&&7) * 0.125f;
        float v0 = (p->animFrame>>3) * 0.125f;
        float u1 = u0 + 0.125f;
        float v1 = v0 + 0.125f;
        float size = (3500.0f*(info->sys_glWidth/1024.0f));
        float w = max(1.5f,size/p->z);
        float ow = max(1.5f,size/p->oldz);
        float d = FastDistance2D(dx, dy);
        float s, os, dxs, dys, dxos, dyos, dxm, dym;
        
        if(d) {
            s = w/d;
        } else {
            s = 0.0f;
        }
        
        if(d) {
            os = ow/d;
        } else {
            os = 0.0f;
        }
        
        m = 2.0f + s; 
        
        dxs = dx*s;
        dys = dy*s;
        dxos = dx*os;
        dyos = dy*os;
        dxm = dx*m;
        dym = dy*m;
                
        info->starfieldTextures[info->starfieldTexturesIndex++] = u0;
        info->starfieldTextures[info->starfieldTexturesIndex++] = v0;
        info->starfieldVertices[info->starfieldVerticesIndex++] = screenx+dxm-dys;
        info->starfieldVertices[info->starfieldVerticesIndex++] = screeny+dym+dxs;
        info->starfieldTextures[info->starfieldTexturesIndex++] = u0;
        info->starfieldTextures[info->starfieldTexturesIndex++] = v1;
        info->starfieldVertices[info->starfieldVerticesIndex++] = screenx+dxm+dys;
        info->starfieldVertices[info->starfieldVerticesIndex++] = screeny+dym-dxs;
        info->starfieldTextures[info->starfieldTexturesIndex++] = u1;
        info->starfieldTextures[info->starfieldTexturesIndex++] = v1;
        info->starfieldVertices[info->starfieldVerticesIndex++] = oldscreenx-dxm+dyos;
        info->starfieldVertices[info->starfieldVerticesIndex++] = oldscreeny-dym-dxos;
        info->starfieldTextures[info->starfieldTexturesIndex++] = u1;
        info->starfieldTextures[info->starfieldTexturesIndex++] = v0;
        info->starfieldVertices[info->starfieldVerticesIndex++] = oldscreenx-dxm-dyos;
        info->starfieldVertices[info->starfieldVerticesIndex++] = oldscreeny-dym+dxos;
    }
}


__private_extern__ void UpdateParticle(Particle *p)
{
	p->oldx = p->x;
	p->oldy = p->y;
	p->oldz = p->z;

	p->x += p->deltax * info->fDeltaTime;
	p->y += p->deltay * info->fDeltaTime;
	p->z += p->deltaz * info->fDeltaTime;
}


__private_extern__ void InitParticle(Particle *p)
{
	int r1, r2;
	p->oldz = RandFlt(2500.0f,22500.0f);
	r1 = rand();
	r2 = rand();
	p->oldx = ((float) (r1 % (int) info->sys_glWidth) - info->sys_glWidth * 0.5f) / 
							(info->sys_glWidth / p->oldz);
	p->oldy = (info->sys_glHeight * 0.5f - (float) (r2 % (int) info->sys_glHeight)) / 
							(info->sys_glWidth / p->oldz);
	p->deltax = 0.0f;
	p->deltay = 0.0f;
	p->deltaz = -starSpeed;
	p->x = p->oldx + p->deltax;
	p->y = p->oldy + p->deltay;
	p->z = p->oldz + p->deltaz;
	p->r = RandFlt(0.125f, 1.0f);
	p->g = RandFlt(0.125f, 1.0f);
	p->b = RandFlt(0.125f, 1.0f);
	p->animFrame = 0;
}
