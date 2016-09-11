/*************************************************************************
> File Name: FrameBuffer.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A frame buffer object, used to store the different g-buffer states of a viewport.
> Created Time: 2016/06/23
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_FRAME_BUFFER_H
#define CUBBY_FRAME_BUFFER_H

struct FrameBuffer
{
	std::string name;
	
	unsigned int diffuseTexture;
	unsigned int positionTexture;
	unsigned int normalTexture;
	unsigned int depthTexture;

	int width;
	int height;

	float viewportScale;
	GLuint fbo;
};

#endif