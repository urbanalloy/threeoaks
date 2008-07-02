#include "gl_platform.h" // platform-specific system and OpenGL header files
#include <math.h>
#ifndef _WIN32
#	include <sys/time.h>
#	include <sys/sysctl.h>
#endif

#include "Gl_saver.h"
#include "Std.h"
#include "Smoke.h"
#include "Star.h"
#include "Spark.h"
#include "Particle.h"

// some globals
__private_extern__ global_info_t *info = NULL;
static double gStartTime = 0.0;


#ifdef _WIN32 // emulate CurrentTime() function
extern double CurrentTime(void);
#endif // _WIN32


__private_extern__ void OTSetup(void)
{
    if (gStartTime == 0.0) {
        gStartTime = CurrentTime();
    }
}


__private_extern__ double TimeInSecondsSinceStart(void)
{
	return (CurrentTime() - gStartTime);
}



//////////////////////////////////////////////////////////////////
// Do any initialization of the rendering context here, such as
// setting background colors, setting up lighting, or performing
// preliminary calculations.
__private_extern__ void GLSetupRC(void)
{
    int i,k;

	// timing setup
	OTSetup();
    info->fTime = (float)TimeInSecondsSinceStart() + info->flurryRandomSeed;
	info->fOldTime = info->fTime;
    info->optMode = OPT_MODE_SCALAR_BASE;


	// initialize particles
    for (i = 0; i < NUMSMOKEPARTICLES / 4; i++) {
        for (k = 0; k < 4; k++) {
            info->s->p[i].dead.i[k] = TRUE;
        }
    }
    
    for (i = 0; i < 12; i++) {
		info->spark[i]->mystery = 1800 * (i + 1) / 13;
        UpdateSpark(info->spark[i]);
    }
    
    // setup the defaults for OpenGL
    glDisable(GL_DEPTH_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glEnable(GL_ALPHA_TEST);
    glShadeModel(GL_FLAT);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
	
    glViewport(0, 0, (int)info->sys_glWidth, (int)info->sys_glHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, info->sys_glWidth, 0, info->sys_glHeight);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	
	glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnableClientState(GL_COLOR_ARRAY);	
    glEnableClientState(GL_VERTEX_ARRAY);	
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}


//////////////////////////////////////////////////////////////////
// Render the OpenGL Scene here. Called by the WM_PAINT message
// handler.
#include <crtdbg.h>
__private_extern__ void GLRenderScene(void)
{
    int i;
    
    info->dframe++;
    
    info->fOldTime = info->fTime;
    info->fTime = (float)TimeInSecondsSinceStart() + info->flurryRandomSeed;
    info->fDeltaTime = info->fTime - info->fOldTime;
	_RPT1(_CRT_WARN, "base code thinks last frame took %g sec\n", info->fDeltaTime);
    
    info->drag = (float) pow(0.9965, info->fDeltaTime * 85.0);
    
    for (i = 0; i < numParticles; i++) {
        UpdateParticle(info->p[i]);
    }
    UpdateStar(info->star);
    for (i = 0; i <info->numStreams; i++) {
        UpdateSpark(info->spark[i]);
    }
    
    switch(info->optMode) {
		case OPT_MODE_SCALAR_BASE:
			UpdateSmoke_ScalarBase(info->s);
			break;
		default:
			break;
    }
    
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glEnable(GL_TEXTURE_2D);
    
    switch(info->optMode) {
		case OPT_MODE_SCALAR_BASE:
		case OPT_MODE_SCALAR_FRSQRTE:
			DrawSmoke_Scalar(info->s);
			break;
		default:
			break;
    }    
    
    glDisable(GL_TEXTURE_2D);
}


__private_extern__ void GLResize(int w, int h)
{
    info->sys_glWidth = w;
    info->sys_glHeight = h;
}
