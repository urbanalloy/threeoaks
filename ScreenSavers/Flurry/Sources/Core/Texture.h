/*
 *  Texture.h
 *  AppleFlurry
 *
 *  Created by Mike Trent on Wed May 22 2002.
 *  Copyright (c) 2001 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __TEXTURE_H_
#define __TEXTURE_H_

// Texture builds the textures used by the Flurry streamer. The texture is loaded
// directly into OpenGL.


// MakeTexture is a big do-all function that creates the textures and leaves them
// ready for use.
__private_extern__ void MakeTexture(void);


#endif // __TEXTURE_H_
