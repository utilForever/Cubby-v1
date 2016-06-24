/*************************************************************************
> File Name: Renderer.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: The OpenGL renderer that is an encapsulation of all the rendering functionality of the engine. A wrapper around most common OpenGL calls.
> Created Time: 2016/06/25
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "Renderer.h"

// OpenGL Error Check
int CheckGLErrors(char* file, int line)
{
	GLenum glErr;
	int retCode = 0;

	glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		const GLubyte* errorString = gluErrorString(glErr);

		if (errorString != nullptr)
		{
			std::cout << "OpenGL Error #" << glErr << "(" << gluErrorString(glErr) << ") " << " in File " << file << " at line: " << line << std::endl;
		}
		else
		{
			std::cout << "OpenGL Error #" << glErr << "(no message available)" << " in File " << file << " at line: " << line << std::endl;
		}

		retCode = 1;
		glErr = glGetError();
	}

	return retCode;
}

// Constructor, Destructor
Renderer::Renderer(int width, int height, int depthBits, int stencilBits) :
	m_windowWidth(width), m_windowHeight(height),
	m_stencil(false), m_depth(false),
	m_clipNear(0.1f), m_clipFar(10000.0f),
	m_primitiveMode(static_cast<unsigned int>(PrimitiveMode::TRIANGLES)),
	m_cullMode(CullMode::NOCULL),
	m_quadratic(gluNewQuadric()), m_activeViewport(-1), m_projection(nullptr)
{
	// Is depth buffer needed?
	if (depthBits > 0)
	{
		glEnable(GL_DEPTH_TEST);
		glClearDepth(1.0f);
		glDepthFunc(GL_LESS);

		m_depth = true;
	}

	// Is stencil buffer needed?
	if (stencilBits > 0)
	{
		glClearStencil(0);

		m_stencil = true;
	}

	// Set clear color to black
	SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Enable smooth shading
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Quadratic initialize
	gluQuadricNormals(m_quadratic, GLU_SMOOTH);
	gluQuadricTexture(m_quadratic, GL_TRUE);

	InitOpenGLExtensions();
}

Renderer::~Renderer()
{
	// Delete the vertex arrays
	m_vertexArraysMutex.lock();
	for (auto& vertexArray : m_vertexArrays)
	{
		delete vertexArray;
		vertexArray = nullptr;
	}
	m_vertexArrays.clear();
	m_vertexArraysMutex.unlock();

	// Delete the viewports
	for (auto& viewport : m_viewports)
	{
		delete viewport;
		viewport = nullptr;
	}
	m_viewports.clear();

	// Delete the frustums
	for (auto& frustum : m_frustums)
	{
		delete frustum;
		frustum = nullptr;
	}
	m_frustums.clear();

	// Delete the materials
	for (auto& material : m_materials)
	{
		delete material;
		material = nullptr;
	}
	m_materials.clear();

	// Delete the textures
	for (auto& texture : m_textures)
	{
		delete texture;
		texture = nullptr;
	}
	m_textures.clear();

	// Delete the lights
	for (auto& light : m_lights)
	{
		delete light;
		light = nullptr;
	}
	m_lights.clear();

	// Delete the FreeType fonts
	for (auto& freetypeFont : m_freetypeFonts)
	{
		delete freetypeFont;
		freetypeFont = nullptr;
	}
	m_freetypeFonts.clear();

	// Delete the frame buffers
	for (auto& frameBuffer : m_frameBuffers)
	{
		delete frameBuffer;
		frameBuffer = nullptr;
	}
	m_frameBuffers.clear();

	// Delete the shaders
	m_shaders.clear();

	// Delete the quadratic drawer
	gluDeleteQuadric(m_quadratic);
}