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

#include <string>

#include <../Libraries/glew/include/GL/glew.h>

class FrameBuffer
{
private:
	std::string m_name;
	
	unsigned int m_diffuseTexture;
	unsigned int m_positionTexture;
	unsigned int m_normalTexture;
	unsigned int m_depthTexture;

	int m_width;
	int m_height;

	float m_viewportScale;
	GLuint m_fbo;
};

#endif