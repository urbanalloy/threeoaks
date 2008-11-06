/* 
 *  This source file is part of Drempels, a program that falls
 *  under the Gnu Public License (GPL) open-source license.  
 *  Use and distribution of this code is regulated by law under 
 *  this license.  For license details, visit:
 *    http://www.gnu.org/copyleft/gpl.html
 * 
 *  The Drempels open-source project is accessible at 
 *  sourceforge.net; the direct URL is:
 *    http://sourceforge.net/projects/drempels/
 *  
 *  Drempels was originally created by Ryan M. Geiss in 2001
 *  and was open-sourced in February 2005.  The original
 *  Drempels homepage is available here:
 *    http://www.geisswerks.com/drempels/
 *
 */

#include "math.h"
#include "time.h"
#include "gpoly.h"
#include <assert.h>
#include <stdio.h>

inline void iCubicInterp(float uL, float uR, float rL, float rR, 
				  signed __int32 *u, signed __int32 *du, signed __int32 *ddu, signed __int32 *dddu,
				  float dx)
{
	{
		float f[6];
		float fddu;
		f[0] = dx*dx*dx;
		f[1] = dx*dx;
		f[2] = uR - uL - rL*dx;
		f[3] = 3*dx*dx;
		f[4] = 2*dx;
		f[5] = rR - rL;

		*u = (int)(uL);
		*du = (int)(rL);
		float denom = (f[4]*f[0] - f[3]*f[1]);
		if (denom != 0)
		{
			fddu = (f[5]*f[0] - f[2]*f[3]) / denom;
			*ddu = (int)(fddu);
			*dddu = (int)(((f[2] - f[1]*fddu) / f[0]));
		}
	}
}

inline void fCubicInterp(float uL, float uR, float rL, float rR, 
				  float *u, float *du, float *ddu, float *dddu,
				  float dx)
{
	float f[6];
	float fddu;
	f[0] = dx*dx*dx;
	f[1] = dx*dx;
	f[2] = uR - uL - rL*dx;
	f[3] = 3*dx*dx;
	f[4] = 2*dx;
	f[5] = rR - rL;

	*u = uL;
	*du = rL;
	float denom = (f[4]*f[0] - f[3]*f[1]);
	if (denom != 0)
	{
		fddu = (f[5]*f[0] - f[2]*f[3]) / denom;
		*ddu = fddu;
		*dddu = (f[2] - f[1]*fddu) / f[0];
	}
}


void BlitWarp256AndMix(td_cellcornerinfo cell0, 
					td_cellcornerinfo cell1, 
					td_cellcornerinfo cell2, 
					td_cellcornerinfo cell3, 
					int x0, int y0, 
					int x1, int y1, 
					unsigned char *old_dest,
					int old_dest_mult,
					int new_mult,
					unsigned char *dest, 
					int W, 
					int H, 
					unsigned char *tex,
					bool bHighQuality)
{
	if (!tex) return;
	if (!dest) return;

	//  (x0y0)
	//   u0v0 ------ u1v1
	//    |            |
	//    |            |
	//   u2v2 ------->uv3 
	//				(x1y1)

	unsigned int *dest32 = (unsigned int *)dest;
	unsigned int *tex32 = (unsigned int *)tex;

	int dx = x1 - x0;
	int _dx = x1 - x0;
	int _dy = y1 - y0;
	float fdy = (float)(y1 - y0);
	float fdx = (float)(x1 - x0);
	int dest_pos;
	float min;

	min = (cell0.u < cell1.u) ? cell0.u : cell1.u;
	min = (min < cell2.u) ? min : cell2.u;
	min = (min < cell3.u) ? min : cell3.u;
	if (min < 0) 
	{
		float ufix = (2.0f - (int)(min/(INTFACTOR*256))) * INTFACTOR*256.0f;
		cell0.u += ufix;
		cell1.u += ufix;
		cell2.u += ufix;
		cell3.u += ufix;
	}

	min = (cell0.v < cell1.v) ? cell0.v : cell1.v;
	min = (min < cell2.v) ? min : cell2.v;
	min = (min < cell3.v) ? min : cell3.v;
	if (min < 0) 
	{
		float vfix = (2.0f - (int)(min/(INTFACTOR*256))) * INTFACTOR*256.0f;
		cell0.v += vfix;
		cell1.v += vfix;
		cell2.v += vfix;
		cell3.v += vfix;
	}

	dest_pos = (y0*W + x0)*4;
	old_dest += dest_pos;

	float uL   ;
	float duL  ;
	float dduL ;
	float ddduL;
	float rL   ;
	float drL  ;
	float ddrL ;
	float dddrL;
	float vL   ;
	float dvL  ;
	float ddvL ;
	float dddvL;
	float sL   ;
	float dsL  ;
	float ddsL ;
	float dddsL;
	float uR   ;
	float duR  ;
	float dduR ;
	float ddduR;
	float rR   ;
	float drR  ;
	float ddrR ;
	float dddrR;
	float vR   ;
	float dvR  ;
	float ddvR ;
	float dddvR;
	float sR   ;
	float dsR  ;
	float ddsR ;
	float dddsR;
	float uL_now;
	float rL_now;
	float vL_now;
	float sL_now;
	float uR_now;
	float rR_now;
	float vR_now;
	float sR_now;

	// next 24 vars are with respect to y:
	fCubicInterp(cell0.u, cell2.u, cell0.dudy, cell2.dudy, &uL, &duL, &dduL, &ddduL, fdy);
	fCubicInterp(cell0.r, cell2.r, cell0.drdy, cell2.drdy, &rL, &drL, &ddrL, &dddrL, fdy);
	fCubicInterp(cell0.v, cell2.v, cell0.dvdy, cell2.dvdy, &vL, &dvL, &ddvL, &dddvL, fdy);
	fCubicInterp(cell0.s, cell2.s, cell0.dsdy, cell2.dsdy, &sL, &dsL, &ddsL, &dddsL, fdy);
	fCubicInterp(cell1.u, cell3.u, cell1.dudy, cell3.dudy, &uR, &duR, &dduR, &ddduR, fdy);
	fCubicInterp(cell1.r, cell3.r, cell1.drdy, cell3.drdy, &rR, &drR, &ddrR, &dddrR, fdy);
	fCubicInterp(cell1.v, cell3.v, cell1.dvdy, cell3.dvdy, &vR, &dvR, &ddvR, &dddvR, fdy);
	fCubicInterp(cell1.s, cell3.s, cell1.dsdy, cell3.dsdy, &sR, &dsR, &ddsR, &dddsR, fdy);

	unsigned __int16 m_old[4] = { old_dest_mult, old_dest_mult, old_dest_mult, old_dest_mult };
	unsigned __int16 m_new[4] = { new_mult, new_mult, new_mult, new_mult };

	//int u, du, ddu, dddu;
	//int v, dv, ddv, dddv;
	signed __int32 u0[1024];
	signed __int32 v0[1024];

	//void *u0;
	//void *v0;

	for (int i=0; i<_dy; i++)
	{
		uL_now = uL + i*(duL + i*(dduL + i*(ddduL)));
		rL_now = rL + i*(drL + i*(ddrL + i*(dddrL)));
		vL_now = vL + i*(dvL + i*(ddvL + i*(dddvL)));
		sL_now = sL + i*(dsL + i*(ddsL + i*(dddsL)));
		uR_now = uR + i*(duR + i*(dduR + i*(ddduR)));
		rR_now = rR + i*(drR + i*(ddrR + i*(dddrR)));
		vR_now = vR + i*(dvR + i*(ddvR + i*(dddvR)));
		sR_now = sR + i*(dsR + i*(ddsR + i*(dddsR)));

		iCubicInterp(uL_now, uR_now, rL_now, rR_now, &u0[0], &u0[1], &u0[2], &u0[3], fdx);
		iCubicInterp(vL_now, vR_now, sL_now, sR_now, &v0[0], &v0[1], &v0[2], &v0[3], fdx);
	
		if (bHighQuality)
		{
			if (old_dest_mult != 0)		// blend with old
			__asm
			{
				xor ecx, ecx

				mov esi, tex
				mov edi, dest
				add edi, dest_pos

				movq mm2, m_new
				movq mm3, m_old
				pxor mm6, mm6		// sub-u coord
				pxor mm7, mm7		// sub-v coord

				// pre-cache the old image, to consolidate stalls
				/*
				mov edx, old_dest
				mov ecx, _dx
				shr ecx, 3
				LOOP0:
					mov   eax, dword ptr [edx + ecx*8]
					dec   ecx
					jnz   LOOP0
				*/
							/*
							mov eax, dddu
							imul ecx
							add eax, ddu
							imul ecx
							add eax, du
							imul ecx
							add eax, u
							mov ebx, eax
							*/
							mov ebx, dword ptr [u0]

				ALIGN 8
				LOOP1:

					mov eax, dword ptr [v0+12]
					imul ecx
	// high quality mode: lowest SHIFTFACTOR bits of this register will be the sub-u coordinate
	movd mm6, ebx

						shr ebx, SHIFTFACTOR
					add eax, dword ptr [v0+8]
					imul ecx
						and ebx, 0x00FF
					add eax, dword ptr [v0+4]
					imul ecx
						inc ecx
					add eax, dword ptr [v0]

	// high quality mode: lowest SHIFTFACTOR bits of this register will be the sub-v coordinate
	movd mm7, eax
					shr eax, SHIFTFACTOR - 8
					and eax, 0xFF00
					or  ebx, eax

				 
	
	//bilinear texel interp:
	// mm4: 1-u
	// mm5: 1-v
	// mm6: u
	// mm7: v

	// get u- and v-blend multipliers

	psllq mm7, 32                   // ?v??|0000
	por   mm7, mm6					// ?v??|?u??
	pslld mm7, 32-SHIFTFACTOR		// v??0|u??0
	psrld mm7, 24           		// 000v|000u
	movq  mm6, mm7			
	psllw mm6, 8
	por   mm6, mm7                  // 00vv|00uu
	movq  mm7, mm6
	pslld mm7, 16
	 pxor  mm0, mm0
	por   mm6, mm7                  // vvvv|uuuu
	movq  mm7, mm6
	punpcklbw  mm6, mm0				// 0u0u|0u0u
	punpckhbw  mm7, mm0				// 0v0v|0v0v

	// get their inverses
	pcmpeqb    mm4, mm4
	psrlw      mm4, 8
	psubb      mm4, mm6
	pcmpeqb    mm5, mm5
	psrlw      mm5, 8
	psubb      mm5, mm7

    // current state:
	// mm6 ~ 0u0u|0u0u      mm4 ~ inverse
	// mm7 ~ 0v0v|0v0v      mm5 ~ inverse

		// edx <- offset of pixel to the right
		mov edx, ebx
		mov eax, ebx
		inc edx
		and edx, 0x00ff
		and eax, 0xff00
		or  edx, eax

	// blend on u (top)
	punpcklbw  mm0, dword ptr [esi+ebx*4]
	punpcklbw  mm3, dword ptr [esi+edx*4]
	psrlw      mm0, 8
	psrlw      mm3, 8
	pmullw     mm0, mm4
		// ebx <- offset of pixel below
		add ebx, 256
		and ebx, 0xffff
	pmullw     mm3, mm6
		// edx <- offset of pixel below and to the right
		add edx, 256
		and edx, 0xffff
	paddusb    mm0, mm3
	psrlw      mm0, 8

		// blend on u (bottom)
		punpcklbw  mm2, dword ptr [esi+ebx*4]
		punpcklbw  mm1, dword ptr [esi+edx*4]
		psrlw      mm2, 8
		psrlw      mm1, 8
		pmullw     mm2, mm4
		pmullw     mm1, mm6
		paddusb    mm2, mm1
		psrlw      mm2, 8

	// blend on v
	pmullw     mm2, mm7
	pmullw     mm0, mm5
	paddusb    mm0, mm2
	psrlw      mm0, 8




						// put new pixel into mm0
	//					punpcklbw  mm0, dword ptr [esi+ebx*4]  // each 'unpack' only accesses 32 bits of memory and they go into the high bytes of the 4 words in the mmx register.
							mov eax, dword ptr [u0+12]
							imul ecx
	//					psrlw      mm0, 8
							add eax, dword ptr [u0+8]

									/*
									//for alpha_src_color blending:
									movq    mm2, mm0
									psrlw   mm2, 2		// to intensify
									pcmpeqb mm3, mm3
									psrlw   mm3, 8
									psubb   mm3, mm2
									*/

						// put old pixel into mm1
						pmullw      mm0, m_new
						mov        edx, old_dest
						punpcklbw  mm1, dword ptr [edx+ecx*4-4]  // each 'unpack' only accesses 32 bits of memory.
							imul ecx

						psrlw      mm1, 8
						// multiply & add
						pmullw      mm1, m_old
							add eax, dword ptr [u0+4]
							// ---stall: 2 cycles---
						paddusw     mm0, mm1
							imul ecx
						
						// pack & store
						psrlw       mm0, 8		 // bytes: 0a0b0g0r
							add eax, dword ptr [u0]
						packuswb    mm0, mm0    // bytes: abgrabgr
							mov ebx, eax
						movd        dword ptr [edi+ecx*4-4], mm0  // store 

					mov   eax, ecx
					sub   eax, _dx
					jl    LOOP1

				EMMS
			}
			else	// paste (no blend), high-quality
			__asm
			{
				xor ecx, ecx

				mov esi, tex
				mov edi, dest
				add edi, dest_pos

				movq mm2, m_new
				movq mm3, m_old
				pxor mm6, mm6		// sub-u coord
				pxor mm7, mm7		// sub-v coord

				// pre-cache the old image, to consolidate stalls
				/*
				mov edx, old_dest
				mov ecx, _dx
				shr ecx, 3
				LOOP0:
					mov   eax, dword ptr [edx + ecx*8]
					dec   ecx
					jnz   LOOP0
				*/
							/*
							mov eax, dddu
							imul ecx
							add eax, ddu
							imul ecx
							add eax, du
							imul ecx
							add eax, u
							mov ebx, eax
							*/
							mov ebx, dword ptr [u0]

				ALIGN 8
				LOOP1c:

					mov eax, dword ptr [v0+12]
					imul ecx
	// high quality mode: lowest SHIFTFACTOR bits of this register will be the sub-u coordinate
	movd mm6, ebx

						shr ebx, SHIFTFACTOR
					add eax, dword ptr [v0+8]
					imul ecx
						and ebx, 0x00FF
					add eax, dword ptr [v0+4]
					imul ecx
						inc ecx
					add eax, dword ptr [v0]

	// high quality mode: lowest SHIFTFACTOR bits of this register will be the sub-v coordinate
	movd mm7, eax
					shr eax, SHIFTFACTOR - 8
					and eax, 0xFF00
					or  ebx, eax

				 
	
	//bilinear texel interp:
	// mm4: 1-u
	// mm5: 1-v
	// mm6: u
	// mm7: v

	// get u- and v-blend multipliers

	psllq mm7, 32                   // ?v??|0000
	por   mm7, mm6					// ?v??|?u??
	pslld mm7, 32-SHIFTFACTOR		// v??0|u??0
	psrld mm7, 24           		// 000v|000u
	movq  mm6, mm7			
	psllw mm6, 8
	por   mm6, mm7                  // 00vv|00uu
	movq  mm7, mm6
	pslld mm7, 16
	 pxor  mm0, mm0
	por   mm6, mm7                  // vvvv|uuuu
	movq  mm7, mm6
	punpcklbw  mm6, mm0				// 0u0u|0u0u
	punpckhbw  mm7, mm0				// 0v0v|0v0v

	// get their inverses
	pcmpeqb    mm4, mm4
	psrlw      mm4, 8
	psubb      mm4, mm6
	pcmpeqb    mm5, mm5
	psrlw      mm5, 8
	psubb      mm5, mm7

		// edx <- offset of pixel to the right
		mov edx, ebx
		mov eax, ebx
		inc edx
		and edx, 0x00ff
		and eax, 0xff00
		or  edx, eax

	// blend on u (top)
	punpcklbw  mm0, dword ptr [esi+ebx*4]
	punpcklbw  mm3, dword ptr [esi+edx*4]
	psrlw      mm0, 8
	psrlw      mm3, 8
	pmullw     mm0, mm4
		// ebx <- offset of pixel below
		add ebx, 256
		and ebx, 0xffff
	pmullw     mm3, mm6
		// edx <- offset of pixel below and to the right
		add edx, 256
		and edx, 0xffff
	paddusb    mm0, mm3
	psrlw      mm0, 8

		// blend on u (bottom)
		punpcklbw  mm2, dword ptr [esi+ebx*4]
		punpcklbw  mm1, dword ptr [esi+edx*4]
		psrlw      mm2, 8
		psrlw      mm1, 8
		pmullw     mm2, mm4
		pmullw     mm1, mm6
		paddusb    mm2, mm1
		psrlw      mm2, 8

	// blend on v
	pmullw     mm2, mm7
	pmullw     mm0, mm5
	paddusb    mm0, mm2
	psrlw      mm0, 8




						// put new pixel into mm0
	//					punpcklbw  mm0, dword ptr [esi+ebx*4]  // each 'unpack' only accesses 32 bits of memory and they go into the high bytes of the 4 words in the mmx register.
							mov eax, dword ptr [u0+12]
							imul ecx
	//					psrlw      mm0, 8
							add eax, dword ptr [u0+8]

									/*
									//for alpha_src_color blending:
									movq    mm2, mm0
									psrlw   mm2, 2		// to intensify
									pcmpeqb mm3, mm3
									psrlw   mm3, 8
									psubb   mm3, mm2
									*/

						// put old pixel into mm1
	//////				pmullw      mm0, m_new
						mov        edx, old_dest
	//////				punpcklbw  mm1, dword ptr [edx+ecx*4-4]  // each 'unpack' only accesses 32 bits of memory.
							imul ecx

	//////				psrlw      mm1, 8
						// multiply & add
	//////				pmullw      mm1, m_old
							add eax, dword ptr [u0+4]
							// ---stall: 2 cycles---
	//////				paddusw     mm0, mm1
							imul ecx
						
						// pack & store
	//////				psrlw       mm0, 8		 // bytes: 0a0b0g0r
							add eax, dword ptr [u0]
						packuswb    mm0, mm0    // bytes: abgrabgr
							mov ebx, eax
						movd        dword ptr [edi+ecx*4-4], mm0  // store 

					mov   eax, ecx
					sub   eax, _dx
					jl    LOOP1c

				EMMS
			}
		}
		else        // LOW QUALITY
		{
			if (old_dest_mult != 0)		// blend with old
			__asm
			{
				xor ecx, ecx

				mov esi, tex
				mov edi, dest
				add edi, dest_pos

				movq mm2, m_new
				movq mm3, m_old
				pxor mm6, mm6		// sub-u coord
				pxor mm7, mm7		// sub-v coord

				mov ebx, dword ptr [u0]

				ALIGN 8
				LOOP1b:

					mov eax, dword ptr [v0+12]
					imul ecx
						shr ebx, SHIFTFACTOR
					add eax, dword ptr [v0+8]
					imul ecx
						and ebx, 0x00FF
					add eax, dword ptr [v0+4]
					imul ecx
						inc ecx
					add eax, dword ptr [v0]
					
					shr eax, SHIFTFACTOR - 8
					and eax, 0xFF00
					or  ebx, eax

					// put new pixel into mm0
					punpcklbw  mm0, dword ptr [esi+ebx*4]  // each 'unpack' only accesses 32 bits of memory and they go into the high bytes of the 4 words in the mmx register.
						mov eax, dword ptr [u0+12]
						imul ecx
					psrlw      mm0, 8
						add eax, dword ptr [u0+8]

								/*
								//for alpha_src_color blending:
								movq    mm2, mm0
								psrlw   mm2, 2		// to intensify
								pcmpeqb mm3, mm3
								psrlw   mm3, 8
								psubb   mm3, mm2
								*/

					// put old pixel into mm1
					pmullw      mm0, m_new
					mov        edx, old_dest
					punpcklbw  mm1, dword ptr [edx+ecx*4-4]  // each 'unpack' only accesses 32 bits of memory.
						imul ecx

					psrlw      mm1, 8
					// multiply & add
					pmullw      mm1, m_old
						add eax, dword ptr [u0+4]
						// ---stall: 2 cycles---
					paddusw     mm0, mm1
						imul ecx
					
					// pack & store
					psrlw       mm0, 8		 // bytes: 0a0b0g0r
						add eax, dword ptr [u0]
					packuswb    mm0, mm0    // bytes: abgrabgr
						mov ebx, eax
					movd        dword ptr [edi+ecx*4-4], mm0  // store 

					mov   eax, ecx
					sub   eax, _dx
					jl    LOOP1b

				EMMS
			}
			else    // paste (no blend)
			__asm
			{
				xor ecx, ecx

				mov esi, tex
				mov edi, dest
				add edi, dest_pos

				movq mm2, m_new
				movq mm3, m_old
				pxor mm6, mm6		// sub-u coord
				pxor mm7, mm7		// sub-v coord

				mov ebx, dword ptr [u0]

				ALIGN 8
				LOOP1d:

					mov eax, dword ptr [v0+12]
					imul ecx
						shr ebx, SHIFTFACTOR
					add eax, dword ptr [v0+8]
					imul ecx
						and ebx, 0x00FF
					add eax, dword ptr [v0+4]
					imul ecx
						inc ecx
					add eax, dword ptr [v0]
					
					shr eax, SHIFTFACTOR - 8
					and eax, 0xFF00
					or  ebx, eax

					// put new pixel into mm0
					punpcklbw  mm0, dword ptr [esi+ebx*4]  // each 'unpack' only accesses 32 bits of memory and they go into the high bytes of the 4 words in the mmx register.
						mov eax, dword ptr [u0+12]
						imul ecx
					psrlw      mm0, 8
						add eax, dword ptr [u0+8]

								/*
								//for alpha_src_color blending:
								movq    mm2, mm0
								psrlw   mm2, 2		// to intensify
								pcmpeqb mm3, mm3
								psrlw   mm3, 8
								psubb   mm3, mm2
								*/

					// put old pixel into mm1
					//////pmullw      mm0, m_new
					mov        edx, old_dest
					//////punpcklbw  mm1, dword ptr [edx+ecx*4-4]  // each 'unpack' only accesses 32 bits of memory.
						imul ecx

					//////psrlw      mm1, 8
					// multiply & add
					//////pmullw      mm1, m_old
						add eax, dword ptr [u0+4]
						// ---stall: 2 cycles---
					//////paddusw     mm0, mm1
						imul ecx
					
					// pack & store
					//////psrlw       mm0, 8		 // bytes: 0a0b0g0r
						add eax, dword ptr [u0]
					packuswb    mm0, mm0    // bytes: abgrabgr
						mov ebx, eax
					movd        dword ptr [edi+ecx*4-4], mm0  // store 

					mov   eax, ecx
					sub   eax, _dx
					jl    LOOP1d

				EMMS
			}
		}

		dest_pos += W*4;
		old_dest += W*4;

	}

}
