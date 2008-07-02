// Particle.h: interface for the Particle class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PARTICLE_H_
#define __PARTICLE_H_


typedef struct Particle  
{
	float charge;
	float x;
	float y;
	float z;
	float oldx;
	float oldy;
	float oldz;
	float deltax;
	float deltay;
	float deltaz;
	float r;
	float g;
	float b;
	int animFrame;
} Particle;

__private_extern__ void InitParticle(Particle *p);
__private_extern__ void UpdateParticle(Particle *p);
__private_extern__ void DrawParticle(Particle *p);


#endif // __PARTICLE_H_
