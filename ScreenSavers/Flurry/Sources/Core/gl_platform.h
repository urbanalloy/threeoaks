#ifndef __GL_PLATFORM_H_
#define __GL_PLATFORM_H_

#if 1
// for Windows:

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

#define __private_extern__ extern
#define INLINE __inline

#else
// for Mac OS X:

#include <Carbon/Carbon.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#define INLINE inline

#endif


#endif // __GL_PLATFORM_H_
