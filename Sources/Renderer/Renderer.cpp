/*************************************************************************
> File Name: Renderer.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: The OpenGL renderer that is an encapsulation of all the rendering functionality of the engine. A wrapper around most common OpenGL calls.
> Created Time: 2016/06/25
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <Maths/3DMaths.h>

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

	// Rendered information
	m_numRenderedVertices = 0;
	m_numRenderedFaces = 0;

	InitOpenGLExtensions();
}

Renderer::~Renderer()
{
	unsigned int i;

	// Delete the vertex arrays
	m_vertexArraysMutex.lock();
	for (i = 0; i < m_vertexArrays.size(); ++i)
	{
		delete m_vertexArrays[i];
		m_vertexArrays[i] = nullptr;
	}
	m_vertexArrays.clear();
	m_vertexArraysMutex.unlock();

	// Delete the viewports
	for (i = 0; i < m_viewports.size(); ++i)
	{
		delete m_viewports[i];
		m_viewports[i] = nullptr;
	}
	m_viewports.clear();

	// Delete the frustums
	for (i = 0; i < m_frustums.size(); ++i)
	{
		delete m_frustums[i];
		m_frustums[i] = nullptr;
	}
	m_frustums.clear();

	// Delete the materials
	for (i = 0; i < m_materials.size(); ++i)
	{
		delete m_materials[i];
		m_materials[i] = nullptr;
	}
	m_materials.clear();

	// Delete the textures
	for (i = 0; i < m_textures.size(); ++i)
	{
		delete m_textures[i];
		m_textures[i] = nullptr;
	}
	m_textures.clear();

	// Delete the lights
	for (i = 0; i < m_lights.size(); ++i)
	{
		delete m_lights[i];
		m_lights[i] = nullptr;
	}
	m_lights.clear();

	// Delete the FreeType fonts
	for (i = 0; i < m_freetypeFonts.size(); ++i)
	{
		delete m_freetypeFonts[i];
		m_freetypeFonts[i] = nullptr;
	}
	m_freetypeFonts.clear();

	// Delete the frame buffers
	for (i = 0; i < m_frameBuffers.size(); ++i)
	{
		delete m_frameBuffers[i];
		m_frameBuffers[i] = nullptr;
	}
	m_frameBuffers.clear();

	// Delete the shaders
	for (i = 0; i < m_shaders.size(); ++i)
	{
		//delete m_shaders[i];
		//m_shaders[i] = nullptr;
	}
	m_shaders.clear();

	// Delete the quadratic drawer
	gluDeleteQuadric(m_quadratic);
}

// Resize
void Renderer::ResizeWindow(int newWidth, int newHeight)
{
	m_windowWidth = newWidth;
	m_windowHeight = newHeight;
}

// Viewport
bool Renderer::CreateViewport(int bottom, int left, int width, int height, float fov, unsigned int* pID)
{
	Viewport* pViewport = new Viewport();
	Frustum* pFrustum = new Frustum();

	pViewport->bottom = bottom;
	pViewport->left = left;
	pViewport->width = width;
	pViewport->height = height;
	pViewport->fov = fov;
	pViewport->aspect = static_cast<float>(width) / static_cast<float>(height);

	// Create the perspective projection for the viewport
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(fov, pViewport->aspect, m_clipNear, m_clipFar);
	float matrix[16];
	glGetFloatv(GL_PROJECTION_MATRIX, matrix);
	pViewport->perspective = matrix;
	glPopMatrix();

	// Setup the frustum for this viewport
	pFrustum->SetFrustum(fov, pViewport->aspect, m_clipNear, m_clipFar);

	// Push this frustum onto the list
	m_frustums.push_back(pFrustum);

	// Create the orthographic projection matrix for the viewport
	float coordinateRight = 1.0f;
	float coordinateLeft = -1.0f;
	float coordinateTop = 1.0f;
	float coordinateBottom = -1.0f;

	memset(&(pViewport->orthographic), 0, sizeof(Matrix4));
	pViewport->orthographic.m_data[0] = 2.0f / (coordinateRight - coordinateLeft);
	pViewport->orthographic.m_data[5] = 2.0f / (coordinateTop - coordinateBottom);
	pViewport->orthographic.m_data[10] = -2.0f / (m_clipFar - m_clipNear);
	pViewport->orthographic.m_data[12] = -(coordinateRight + coordinateLeft) / (coordinateRight - coordinateLeft);
	pViewport->orthographic.m_data[13] = -(coordinateTop + coordinateBottom) / (coordinateTop - coordinateBottom);
	pViewport->orthographic.m_data[14] = -(m_clipFar + m_clipNear) / (m_clipFar - m_clipNear);
	pViewport->orthographic.m_data[15] = 1.0f;

	// Create the 2D projection matrix for the viewport
	coordinateRight = static_cast<float>(m_windowWidth);
	coordinateLeft = 0.0f;
	coordinateTop = static_cast<float>(m_windowHeight);
	coordinateBottom = 0.0f;

	memset(&(pViewport->projection2D), 0, sizeof(Matrix4));
	pViewport->projection2D.m_data[0] = 2.0f / (coordinateRight - coordinateLeft);
	pViewport->projection2D.m_data[5] = 2.0f / (coordinateTop - coordinateBottom);
	pViewport->projection2D.m_data[10] = -2.0f / (m_clipFar - m_clipNear);
	pViewport->projection2D.m_data[12] = -(coordinateRight + coordinateLeft) / (coordinateRight - coordinateLeft);
	pViewport->projection2D.m_data[13] = -(coordinateTop + coordinateBottom) / (coordinateTop - coordinateBottom);
	pViewport->projection2D.m_data[14] = -(m_clipFar + m_clipNear) / (m_clipFar - m_clipNear);
	pViewport->projection2D.m_data[15] = 1.0f;

	// Push this viewport onto the list
	m_viewports.push_back(pViewport);

	// Return the viewport ID
	*pID = m_viewports.size() - 1;

	return true;
}

bool Renderer::ResizeViewport(unsigned int viewportID, int bottom, int left, int width, int height, float fov)
{
	Viewport* pViewport = m_viewports[viewportID];
	Frustum* pFrustum = m_frustums[viewportID];

	pViewport->bottom = bottom;
	pViewport->left = left;
	pViewport->width = width;
	pViewport->height = height;
	pViewport->fov = fov;
	pViewport->aspect = static_cast<float>(width) / static_cast<float>(height);

	// Create the perspective projection for the viewport
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(pViewport->fov, pViewport->aspect, m_clipNear, m_clipFar);
	float matrix[16];
	glGetFloatv(GL_PROJECTION_MATRIX, matrix);
	pViewport->perspective = matrix;
	glPopMatrix();

	// Resize the frustum
	pFrustum->SetFrustum(pViewport->fov, pViewport->aspect, m_clipNear, m_clipFar);

	// Create the orthographic projection matrix for the viewport
	float coordinateRight = 1.0f;
	float coordinateLeft = -1.0f;
	float coordinateTop = 1.0f;
	float coordinateBottom = -1.0f;

	memset(&(pViewport->orthographic), 0, sizeof(Matrix4));
	pViewport->orthographic.m_data[0] = 2.0f / (coordinateRight - coordinateLeft);
	pViewport->orthographic.m_data[5] = 2.0f / (coordinateTop - coordinateBottom);
	pViewport->orthographic.m_data[10] = -2.0f / (m_clipFar - m_clipNear);
	pViewport->orthographic.m_data[12] = -(coordinateRight + coordinateLeft) / (coordinateRight - coordinateLeft);
	pViewport->orthographic.m_data[13] = -(coordinateTop + coordinateBottom) / (coordinateTop - coordinateBottom);
	pViewport->orthographic.m_data[14] = -(m_clipFar + m_clipNear) / (m_clipFar - m_clipNear);
	pViewport->orthographic.m_data[15] = 1.0f;

	// Create the 2D projection matrix for the viewport
	coordinateRight = static_cast<float>(m_windowWidth);
	coordinateLeft = 0.0f;
	coordinateTop = static_cast<float>(m_windowHeight);
	coordinateBottom = 0.0f;

	memset(&(pViewport->projection2D), 0, sizeof(Matrix4));
	pViewport->projection2D.m_data[0] = 2.0f / (coordinateRight - coordinateLeft);
	pViewport->projection2D.m_data[5] = 2.0f / (coordinateTop - coordinateBottom);
	pViewport->projection2D.m_data[10] = -2.0f / (m_clipFar - m_clipNear);
	pViewport->projection2D.m_data[12] = -(coordinateRight + coordinateLeft) / (coordinateRight - coordinateLeft);
	pViewport->projection2D.m_data[13] = -(coordinateTop + coordinateBottom) / (coordinateTop - coordinateBottom);
	pViewport->projection2D.m_data[14] = -(m_clipFar + m_clipNear) / (m_clipFar - m_clipNear);
	pViewport->projection2D.m_data[15] = 1.0f;

	return true;
}

int Renderer::GetActiveViewport() const
{
	return m_activeViewport;
}

// Render modes
void Renderer::SetRenderMode(RenderMode mode)
{
	switch (mode)
	{
	case RenderMode::WIREFRAME:
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
		break;
	case RenderMode::SOLID:
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
		break;
	case RenderMode::SHADED:
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
		break;
	case RenderMode::TEXTURED:
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
		break;
	case RenderMode::TEXTURED_LIGHTING:
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
		break;
	};
}

void Renderer::SetPrimitiveMode(PrimitiveMode mode)
{
	switch (mode)
	{
	case PrimitiveMode::POINTS:
		m_primitiveMode = GL_POINTS;
		break;
	case PrimitiveMode::LINES:
		m_primitiveMode = GL_LINES;
		break;
	case PrimitiveMode::LINELIST:
		m_primitiveMode = GL_LINE_STRIP;
		break;
	case PrimitiveMode::TRIANGLES:
		m_primitiveMode = GL_TRIANGLES;
		break;
	case PrimitiveMode::TRIANGLESTRIPS:
		m_primitiveMode = GL_TRIANGLE_STRIP;
		break;
	case PrimitiveMode::TRIANGLEFANS:
		m_primitiveMode = GL_TRIANGLE_FAN;
		break;
	case PrimitiveMode::QUADS:
		m_primitiveMode = GL_QUADS;
		break;
	}
}

void Renderer::SetCullMode(CullMode mode)
{
	m_cullMode = mode;

	switch (mode)
	{
	case CullMode::NOCULL:
		glDisable(GL_CULL_FACE);
		break;
	case CullMode::FRONT:
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		break;
	case CullMode::BACK:
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		break;
	}
}

CullMode Renderer::GetCullMode() const
{
	return m_cullMode;
}

void Renderer::SetLineWidth(float width)
{
	glLineWidth(width);
}

void Renderer::SetPointSize(float width)
{
	glPointSize(width);
}

void Renderer::SetFrontFaceDirection(FrontFaceDirection direction)
{
	if (direction == FrontFaceDirection::CW)
	{
		glFrontFace(GL_CW);
	}
	else if (direction == FrontFaceDirection::CCW)
	{
		glFrontFace(GL_CCW);
	}
}

// Projection
bool Renderer::SetProjectionMode(ProjectionMode mode, int viewPort)
{
	Viewport* pViewport = m_viewports[viewPort];
	glViewport(pViewport->left, pViewport->bottom, pViewport->width, pViewport->height);

	m_activeViewport = viewPort;

	if (mode == ProjectionMode::PERSPECTIVE)
	{
		m_projection = &(pViewport->perspective);
	}
	else if (mode == ProjectionMode::ORTHOGRAPHIC)
	{
		m_projection = &(pViewport->orthographic);
	}
	else if (mode == ProjectionMode::TWO_DIMENSION)
	{
		m_projection = &(pViewport->projection2D);
	}
	else 
	{
		return false;
	}

	SetViewProjection();

	return true;
}

void Renderer::SetViewProjection()
{
	glMatrixMode(GL_PROJECTION);
	MultiplyViewProjection();
	glMatrixMode(GL_MODELVIEW);
}

void Renderer::MultiplyViewProjection()
{
	SetWorldMatrix(m_view * (*m_projection));
}

void Renderer::SetupOrthographicProjection(float left, float right, float bottom, float top, float zNear, float zFar)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(left, right, bottom, top, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Scene
bool Renderer::ClearScene(bool pixel, bool depth, bool stencil) const
{
	GLbitfield clear(0);

	if (pixel)
	{
		clear |= GL_COLOR_BUFFER_BIT;
	}

	if (depth && m_depth)
	{
		clear |= GL_DEPTH_BUFFER_BIT;
	}

	if (stencil && m_stencil)
	{
		clear |= GL_STENCIL_BUFFER_BIT;
	}

	glClear(clear);

	return true;
}

bool Renderer::BeginScene(bool pixel, bool depth, bool stencil)
{
	ClearScene(pixel, depth, stencil);

	// Reset the renderer stat counters
	ResetRenderedStats();

	// Reset the projection and model-view matrices to be identity
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	IdentityWorldMatrix();

	// Start off with lighting and texturing disabled.
	// If these are required, they need to be set explicitly
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	return true;
}

void Renderer::EndScene()
{
	// Swap buffers
}

void Renderer::SetColorMask(bool red, bool green, bool blue, bool alpha)
{
	glColorMask(red, green, blue, alpha);
}

void Renderer::SetClearColor(float red, float green, float blue, float alpha)
{
	glClearColor(red, green, blue, alpha);
}

void Renderer::SetClearColor(Color col)
{
	glClearColor(col.GetRed(), col.GetGreen(), col.GetBlue(), col.GetAlpha());
}

// Push / Pop matrix stack
void Renderer::PushMatrix()
{
	glPushMatrix();

	m_modelStack.push_back(m_model);
}

void Renderer::PopMatrix()
{
	glPopMatrix();

	m_model = m_modelStack.back();
	m_modelStack.pop_back();
}

// Matrix manipulations
void Renderer::SetWorldMatrix(const Matrix4& matrix)
{
	float mat[16];
	matrix.GetMatrix(mat);
	glLoadMatrixf(mat);
}

void Renderer::GetModelViewMatrix(Matrix4* pMatrix)
{
	float matrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	memcpy(pMatrix->m_data, matrix, 16 * sizeof(float));
}

void Renderer::GetModelMatrix(Matrix4* pMatrix)
{
	memcpy(pMatrix->m_data, m_model.m_data, 16 * sizeof(float));
}

void Renderer::GetViewMatrix(Matrix4* pMatrix)
{
	memcpy(pMatrix->m_data, m_view.m_data, 16 * sizeof(float));
}

void Renderer::GetProjectionMatrix(Matrix4* pMatrix)
{
	float matrix[16];
	glGetFloatv(GL_PROJECTION_MATRIX, matrix);
	memcpy(pMatrix->m_data, matrix, 16 * sizeof(float));
}

void Renderer::IdentityWorldMatrix()
{
	glLoadIdentity();

	m_model.LoadIdentity();
}

void Renderer::MultiplyWorldMatrix(const Matrix4& matrix)
{
	float m[16];
	matrix.GetMatrix(m);
	glMultMatrixf(m);

	Matrix4 world(matrix);
	m_model = world * m_model;
}

void Renderer::TranslateWorldMatrix(float x, float y, float z)
{
	glTranslatef(x, y, z);

	Matrix4 translate;
	translate.SetTranslation(glm::vec3(x, y, z));
	m_model = translate * m_model;
}

void Renderer::RotateWorldMatrix(float x, float y, float z)
{
	// Possible gimbal lock?
	glRotatef(z, 0.0f, 0.0f, 1.0f);
	glRotatef(y, 0.0f, 1.0f, 0.0f);
	glRotatef(x, 1.0f, 0.0f, 0.0f);

	Matrix4 rotationX;
	Matrix4 rotationY;
	Matrix4 rotationZ;
	rotationX.SetXRotation(DegreeToRadian(x));
	rotationY.SetYRotation(DegreeToRadian(y));
	rotationZ.SetZRotation(DegreeToRadian(z));

	m_model = rotationZ * rotationY * rotationX * m_model;
}

void Renderer::ScaleWorldMatrix(float x, float y, float z)
{
	glScalef(x, y, z);

	Matrix4 scale;
	scale.SetScale(glm::vec3(x, y, z));
	m_model = scale * m_model;
}

// Texture matrix manipulations
void Renderer::SetTextureMatrix()
{
	static double modelView[16];
	static double projection[16];

	// Moving from unit cube [-1, 1] to [0, 1]
	const GLdouble bias[16] =
	{
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	};

	// Grab model-view and transformation matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);


	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);

	glLoadIdentity();
	glLoadMatrixd(bias);

	// Concatenating all matrices into one.
	glMultMatrixd(projection);
	glMultMatrixd(modelView);

	// Go back to normal matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void Renderer::PushTextureMatrix()
{
	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);
	glPushMatrix();
}

void Renderer::PopTextureMatrix()
{
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

// Scissor testing
void Renderer::EnableScissorTest(int x, int y, int width, int height)
{
	glEnable(GL_SCISSOR_TEST);
	glScissor(x, y, width, height);
}

void Renderer::DisableScissorTest()
{
	glDisable(GL_SCISSOR_TEST);
}

// Screen projection
glm::vec3 Renderer::GetWorldProjectionFromScreenCoordinates(int x, int y, float z) const
{
	double modelViewMatrix[16];
	double projMatrix[16];
	int viewport[4];
	double dX, dY, dZ, clickY;

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	clickY = static_cast<double>(m_windowHeight - y);

	gluUnProject(static_cast<double>(x), clickY, z, modelViewMatrix, projMatrix, viewport, &dX, &dY, &dZ);
	return glm::vec3(static_cast<float>(dX), static_cast<float>(dY), static_cast<float>(dZ));
}

void Renderer::GetScreenCoordinatesFromWorldPosition(glm::vec3 pos, int* x, int* y)
{
	// NOTE : Projection and camera must be set before calling this function, else you wont get 'camera-relative' results...

	GLdouble modelViewMatrix[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix);

	GLdouble projectionMatrix[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	GLdouble winX, winY, winZ;
	gluProject(pos.x, pos.y, pos.z, modelViewMatrix, projectionMatrix, viewport, &winX, &winY, &winZ);

	*x = static_cast<int>(winX);
	*y = static_cast<int>(winY);
}

// Clip planes
void Renderer::EnableClipPlane(unsigned int index, double eq1, double eq2, double eq3, double eq4)
{
	double plane[] = { eq1, eq2, eq3, eq4 };
	glClipPlane(GL_CLIP_PLANE0 + index, plane);
	glEnable(GL_CLIP_PLANE0 + index);
}

void Renderer::DisableClipPlane(unsigned int index)
{
	glDisable(GL_CLIP_PLANE0);
}

// Camera functionality
void Renderer::SetLookAtCamera(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{
	gluLookAt(position.x, position.y, position.z, target.x, target.y, target.z, up.x, up.y, up.z);
}

// Transparency
void Renderer::EnableTransparency(BlendFunction source, BlendFunction destination)
{
	glEnable(GL_BLEND);
	glBlendFunc(GetBlendEnum(source), GetBlendEnum(destination));
}

void Renderer::DisableTransparency()
{
	glDisable(GL_BLEND);
}

GLenum Renderer::GetBlendEnum(BlendFunction flag)
{
	GLenum glFlag = GL_ONE;

	switch (flag)
	{
	case BlendFunction::ONE:
		glFlag = GL_ONE;
		break;
	case BlendFunction::ZERO:
		glFlag = GL_ZERO;
		break;
	case BlendFunction::SRC_ALPHA:
		glFlag = GL_SRC_ALPHA;
		break;
	case BlendFunction::ONE_MINUS_SRC_ALPHA:
		glFlag = GL_ONE_MINUS_SRC_ALPHA;
		break;
	}

	return glFlag;
}

// Sampling
void Renderer::EnableMultiSampling()
{
	glEnable(GL_MULTISAMPLE_ARB);
}

void Renderer::DisableMultiSampling()
{
	glDisable(GL_MULTISAMPLE_ARB);
}

// Vector normalize
void Renderer::EnableVectorNormalize()
{
	glEnable(GL_NORMALIZE);
}

void Renderer::DisableVectorNormalize()
{
	glDisable(GL_NORMALIZE);
}

// Depth testing
void Renderer::EnableDepthTest(DepthTest testFunction)
{
	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GetDepthTest(testFunction));
}

void Renderer::DisableDepthTest()
{
	glDisable(GL_DEPTH_TEST);
}

GLenum Renderer::GetDepthTest(DepthTest test)
{
	GLenum glFlag = GL_NEVER;

	switch (test)
	{
	case DepthTest::NEVER:
		glFlag = GL_NEVER;
		break;
	case DepthTest::ALWAYS:
		glFlag = GL_ALWAYS;
		break;
	case DepthTest::LESS:
		glFlag = GL_LESS;
		break;
	case DepthTest::LESS_EQUAL:
		glFlag = GL_LEQUAL;
		break;
	case DepthTest::EQUAL:
		glFlag = GL_EQUAL;
		break;
	case DepthTest::GREATER_EQUAL:
		glFlag = GL_GEQUAL;
		break;
	case DepthTest::GREATER:
		glFlag = GL_GREATER;
		break;
	case DepthTest::NOT_EQUAL:
		glFlag = GL_NOTEQUAL;
		break;
	}

	return glFlag;
}

void Renderer::EnableDepthWrite()
{
	glDepthMask(GL_TRUE);
}

void Renderer::DisableDepthWrite()
{
	glDepthMask(GL_FALSE);
}

// Immediate mode
void Renderer::EnableImmediateMode(ImmediateModePrimitive mode)
{
	GLenum glMode = GL_POINTS;

	switch (mode)
	{
	case ImmediateModePrimitive::POINTS:
		glMode = GL_POINTS;
		break;
	case ImmediateModePrimitive::LINES:
		glMode = GL_LINES;
		break;
	case ImmediateModePrimitive::LINE_LOOP:
		glMode = GL_LINE_LOOP;
		break;
	case ImmediateModePrimitive::LINE_STRIP:
		glMode = GL_LINE_STRIP;
		break;
	case ImmediateModePrimitive::TRIANGLES:
		glMode = GL_TRIANGLES;
		break;
	case ImmediateModePrimitive::TRIANGLE_STRIP:
		glMode = GL_TRIANGLE_STRIP;
		break;
	case ImmediateModePrimitive::TRIANGLE_FAN:
		glMode = GL_TRIANGLE_FAN;
		break;
	case ImmediateModePrimitive::QUADS:
		glMode = GL_QUADS;
		break;
	case ImmediateModePrimitive::QUAD_STRIP:
		glMode = GL_QUAD_STRIP;
		break;
	case ImmediateModePrimitive::POLYGON:
		glMode = GL_POLYGON;
		break;
	}

	glBegin(glMode);
}

void Renderer::ImmediateVertex(float x, float y, float z)
{
	glVertex3f(x, y, z);
}

void Renderer::ImmediateVertex(int x, int y, int z)
{
	glVertex3i(x, y, z);
}

void Renderer::ImmediateNormal(float x, float y, float z)
{
	glNormal3f(x, y, z);
}

void Renderer::ImmediateNormal(int x, int y, int z)
{
	glNormal3i(x, y, z);
}

void Renderer::ImmediateTextureCoordinate(float s, float t)
{
	glTexCoord2f(s, t);
}

void Renderer::ImmediateColorAlpha(float r, float g, float b, float a)
{
	glColor4f(r, g, b, a);
}

void Renderer::DisableImmediateMode()
{
	glEnd();
}

// Drawing helpers
void Renderer::DrawLineCircle(float radius, int points) const
{
	glBegin(GL_LINE_LOOP);

	float angleRatio = DegreeToRadian(360.0f / points);
	for (float i = 0.0f; i < points; i += 1.0f)
	{
		float angle = i * angleRatio;
		glVertex3f(cos(angle) * radius, 0.0f, sin(angle) * radius);
	}

	glEnd();
}

void Renderer::DrawSphere(float radius, int slices, int stacks) const
{
	gluSphere(m_quadratic, radius, slices, stacks);
}

void Renderer::DrawBezier(Bezier3 curve, int points) const
{
	glBegin(GL_LINE_STRIP);

	float ratio = 1.0f / static_cast<float>(points);
	for (float i = 0.0f; i <= 1.0f; i += ratio)
	{
		glm::vec3 point = curve.GetInterpolatedPoint(i);

		glVertex3f(point.x, point.y, point.z);
	}

	glm::vec3 point = curve.GetInterpolatedPoint(1.0f);
	glVertex3f(point.x, point.y, point.z);

	glEnd();
}

void Renderer::DrawBezier(Bezier4 curve, int points) const
{
	glBegin(GL_LINE_STRIP);

	float ratio = 1.0f / static_cast<float>(points);
	for (float i = 0.0f; i <= 1.0f; i += ratio)
	{
		glm::vec3 point = curve.GetInterpolatedPoint(i);

		glVertex3f(point.x, point.y, point.z);
	}

	glm::vec3 point = curve.GetInterpolatedPoint(1.0f);
	glVertex3f(point.x, point.y, point.z);

	glEnd();
}

void Renderer::DrawCircleSector(float radius, float angle, int points) const
{
	glBegin(GL_LINE_LOOP);

	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(cos(angle) * radius, 0.0f, sin(angle) * radius);

	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(cos(-angle) * radius, 0.0f, sin(-angle) * radius);

	float angleRatio = DegreeToRadian(RadianToDegree(angle * 2.0f) / points);
	for (float i = 0.0f; i <= points; i += 1.0f)
	{
		float newAngle = -angle + i * angleRatio;
		glVertex3f(cos(newAngle) * radius, 0.0f, sin(newAngle) * radius);
	}

	glEnd();
}

void Renderer::DrawSphericalSector(float radius, float angle, int sectors, int points)
{
	float angleRatio = 360.0f / sectors;
	for (float i = 0.0f; i <= sectors; i += 1.0f)
	{
		float rotateAngle = i * angleRatio;
		PushMatrix();
		RotateWorldMatrix(rotateAngle, 0.0f, 0.0f);
		DrawCircleSector(radius, angle, points);
		PopMatrix();
	}
}

// Text rendering
bool Renderer::CreateFreeTypeFont(const char* fontName, int fontSize, unsigned int* pID, bool noAutoHint)
{
	FreeTypeFont* font = new FreeTypeFont();

	// Build the new FreeType font
	font->BuildFont(fontName, fontSize);

	// Push this font onto the list of fonts and return the ID
	m_freetypeFonts.push_back(font);
	*pID = m_freetypeFonts.size() - 1;

	return true;
}

bool Renderer::RenderFreeTypeText(unsigned int fontID, float x, float y, float z, Color color, float scale, const char* inText, ...)
{
	char outText[8192];
	// Pointer to list of arguments
	va_list	ap;

	if (inText == nullptr)
	{
		// Return fail if there is no text
		return false;
	}

	// Loop through variable argument list and add them to the string
	va_start(ap, inText);
	vsprintf(outText, inText, ap);
	va_end(ap);

	glColor4fv(color.GetRGBA());

	// Add on the descent value, so we don't draw letters with underhang out of bounds. (e.g - g, y, q and p)
	y -= GetFreeTypeTextDescent(fontID);

	// HACK : The descent has rounding errors and is usually off by about 1 pixel
	y -= 1;

	glPushMatrix();
	glTranslatef(x, y, 0);
	m_freetypeFonts[fontID]->DrawString(outText, scale);
	glPopMatrix();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	return true;
}

int Renderer::GetFreeTypeTextWidth(unsigned int fontID, const char* inText, ...)
{
	char outText[8192];
	va_list ap;

	if (inText == nullptr)
	{
		return 0;
	}

	// Loop through variable argument list and add them to the string
	va_start(ap, inText);
	vsprintf(outText, inText, ap);
	va_end(ap);

	return m_freetypeFonts[fontID]->GetTextWidth(outText);
}

int Renderer::GetFreeTypeTextHeight(unsigned int fontID, const char* inText, ...)
{
	return m_freetypeFonts[fontID]->GetCharHeight('a');
}

int Renderer::GetFreeTypeTextAscent(unsigned int fontID)
{
	return m_freetypeFonts[fontID]->GetAscent();
}

int Renderer::GetFreeTypeTextDescent(unsigned int fontID)
{
	return m_freetypeFonts[fontID]->GetDescent();
}

// Lighting
bool Renderer::CreateLight(const Color& ambient, const Color& diffuse, const Color& specular, glm::vec3& position, glm::vec3& direction, float exponent, float cutoff, float cAtten, float lAtten, float qAtten, bool point, bool spot, unsigned int* pID)
{
	Light* pLight = new Light();

	pLight->SetAmbient(ambient);
	pLight->SetDiffuse(diffuse);
	pLight->SetSpecular(specular);
	pLight->SetPosition(position);
	pLight->SetDirection(direction);
	pLight->SetExponent(exponent);
	pLight->SetCutoff(cutoff);
	pLight->SetConstantAttenuation(cAtten);
	pLight->SetLinearAttenuation(lAtten);
	pLight->SetQuadraticAttenuation(qAtten);
	pLight->SetPoint(point);
	pLight->SetSpotlight(spot);

	// Push the light onto the list
	m_lights.push_back(pLight);

	// Return the light ID
	*pID = m_lights.size() - 1;

	return true;
}

bool Renderer::EditLight(unsigned int id, const Color& ambient, const Color& diffuse, const Color& specular, glm::vec3& position, glm::vec3& direction, float exponent, float cutoff, float cAtten, float lAtten, float qAtten, bool point, bool spot)
{
	Light* pLight = m_lights[id];

	pLight->SetAmbient(ambient);
	pLight->SetDiffuse(diffuse);
	pLight->SetSpecular(specular);
	pLight->SetPosition(position);
	pLight->SetDirection(direction);
	pLight->SetExponent(exponent);
	pLight->SetCutoff(cutoff);
	pLight->SetConstantAttenuation(cAtten);
	pLight->SetLinearAttenuation(lAtten);
	pLight->SetQuadraticAttenuation(qAtten);
	pLight->SetPoint(point);
	pLight->SetSpotlight(spot);

	return true;
}

bool Renderer::EditLightPosition(unsigned int id, glm::vec3& position)
{
	Light* pLight = m_lights[id];

	pLight->SetPosition(position);

	return true;
}

void Renderer::DeleteLight(unsigned int id)
{
	if (m_lights[id])
	{
		delete m_lights[id];
		m_lights[id] = nullptr;
	}
}

void Renderer::EnableLight(unsigned int id, unsigned int lightNumber)
{
	if (m_lights[id])
	{
		m_lights[id]->Apply(lightNumber);
	}
}

void Renderer::DisableLight(unsigned int lightNumber) const
{
	glDisable(GL_LIGHT0 + lightNumber);
}

void Renderer::RenderLight(unsigned int id)
{
	m_lights[id]->Render();
}

Color Renderer::GetLightAmbient(unsigned int id)
{
	return m_lights[id]->GetAmbient();
}

Color Renderer::GetLightDiffuse(unsigned int id)
{
	return m_lights[id]->GetDiffuse();
}

Color Renderer::GetLightSpecular(unsigned int id)
{
	return m_lights[id]->GetSpecular();
}

glm::vec3 Renderer::GetLightPosition(unsigned int id)
{
	return m_lights[id]->GetPosition();
}

float Renderer::GetConstantAttenuation(unsigned int id)
{
	return m_lights[id]->GetConstantAttenuation();
}

float Renderer::GetLinearAttenuation(unsigned int id)
{
	return m_lights[id]->GetLinearAttenuation();
}

float Renderer::GetQuadraticAttenuation(unsigned int id)
{
	return m_lights[id]->GetQuadraticAttenuation();
}

// Materials
bool Renderer::CreateMaterial(const Color& ambient, const Color& diffuse, const Color& specular, const Color& emmisive, float specularPower, unsigned int* pID)
{
	Material* pMaterial = new Material();

	pMaterial->SetAmbient(ambient);
	pMaterial->SetDiffuse(diffuse);
	pMaterial->SetSpecular(specular);
	pMaterial->SetEmission(emmisive);
	pMaterial->SetShininess(specularPower);

	// Push the material onto the list
	m_materials.push_back(pMaterial);

	// Return the material ID
	*pID = m_materials.size() - 1;

	return true;
}

bool Renderer::EditMaterial(unsigned int id, const Color& ambient, const Color& diffuse, const Color& specular, const Color& emmisive, float specularPower)
{
	Material* pMaterial = m_materials[id];

	pMaterial->SetAmbient(ambient);
	pMaterial->SetDiffuse(diffuse);
	pMaterial->SetSpecular(specular);
	pMaterial->SetEmission(emmisive);
	pMaterial->SetShininess(specularPower);

	return true;
}

void Renderer::EnableMaterial(unsigned int id)
{
	m_materials[id]->Apply();
}

void Renderer::DeleteMaterial(unsigned int id)
{
	if (m_materials[id])
	{
		delete m_materials[id];
		m_materials[id] = nullptr;
	}
}

// Textures
bool Renderer::LoadTexture(std::string fileName, int* width, int* height, int* widthPower2, int* heightPower2, unsigned int* pID)
{
	// Check that this texture hasn't already been loaded
	for (unsigned int i = 0; i < m_textures.size(); ++i)
	{
		if (m_textures[i]->GetFileName() == fileName)
		{
			*width = m_textures[i]->GetWidth();
			*height = m_textures[i]->GetHeight();
			*widthPower2 = m_textures[i]->GetWidthPower2();
			*heightPower2 = m_textures[i]->GetHeightPower2();
			*pID = i;

			return true;
		}
	}

	// Texture hasn't already been loaded, create and load it!
	Texture* pTexture = new Texture();
	pTexture->Load(fileName, width, height, widthPower2, heightPower2, false);

	// Push the vertex array onto the list
	m_textures.push_back(pTexture);

	// Return the vertex array ID
	*pID = m_textures.size() - 1;

	return true;
}

bool Renderer::RefreshTexture(unsigned int id)
{
	Texture *pTexture = m_textures[id];

	int width, height;
	int widthPower2, heightPower2;

	pTexture->Load(pTexture->GetFileName(), &width, &height, &widthPower2, &heightPower2, true);

	return true;
}

bool Renderer::RefreshTexture(std::string fileName)
{
	for (unsigned int i = 0; i < m_textures.size(); ++i)
	{
		if (m_textures[i]->GetFileName() == fileName)
		{
			return RefreshTexture(i);
		}
	}

	return false;
}

void Renderer::BindTexture(unsigned int id)
{
	glEnable(GL_TEXTURE_2D);
	m_textures[id]->Bind();
}

void Renderer::PrepareShaderTexture(unsigned int textureIndex, unsigned int textureID) const
{
	glActiveTextureARB(GL_TEXTURE0_ARB + textureIndex);
	glUniform1iARB(textureID, textureIndex);
}

void Renderer::EmptyTextureIndex(unsigned int textureIndex) const
{
	glActiveTextureARB(GL_TEXTURE0_ARB + textureIndex);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureIndex);
}

void Renderer::DisableTexture() const
{
	glDisable(GL_TEXTURE_2D);
}

Texture* Renderer::GetTexture(unsigned int id)
{
	return m_textures[id];
}

void Renderer::BindRawTextureID(unsigned int textureID) const
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Renderer::GenerateEmptyTexture(unsigned int *pID)
{
	Texture* pTexture = new Texture();
	pTexture->GenerateEmptyTexture();

	// Push the vertex array onto the list
	m_textures.push_back(pTexture);

	// Return the vertex array ID
	*pID = m_textures.size() - 1;
}

void Renderer::SetTextureData(unsigned int id, int width, int height, unsigned char* texData)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_textures[id]->GetID());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glDisable(GL_TEXTURE_2D);
}

// Cube textures
bool Renderer::LoadCubeTexture(int* width, int* height, std::string front, std::string back, std::string top, std::string bottom, std::string left, std::string right, unsigned int* pID) const
{
	unsigned char* texDataFront = nullptr;
	unsigned char* texDataBack = nullptr;
	unsigned char* texDataTop = nullptr;
	unsigned char* texDataBottom = nullptr;
	unsigned char* texDataLeft = nullptr;
	unsigned char* texDataRight = nullptr;

	// TODO: TGA file loaded check!
	// Only support TGA cubemaps for now!
	LoadFileTGA(front.c_str(), &texDataFront, width, height, true) == 1;
	LoadFileTGA(back.c_str(), &texDataBack, width, height, true) == 1;
	LoadFileTGA(top.c_str(), &texDataTop, width, height, true) == 1;
	LoadFileTGA(bottom.c_str(), &texDataBottom, width, height, true) == 1;
	LoadFileTGA(left.c_str(), &texDataLeft, width, height, true) == 1;
	LoadFileTGA(right.c_str(), &texDataRight, width, height, true) == 1;

	GLuint id;
	glEnable(GL_TEXTURE_CUBE_MAP);
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texDataFront);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texDataBack);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texDataTop);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texDataBottom);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texDataLeft);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texDataRight);

	glDisable(GL_TEXTURE_CUBE_MAP);

	*pID = id;

	return true;
}

void Renderer::BindCubeTexture(unsigned int id) const
{
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

void Renderer::EmptyCubeTextureIndex(unsigned int textureIndex) const
{
	glActiveTextureARB(GL_TEXTURE0_ARB + textureIndex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureIndex);
	glDisable(GL_TEXTURE_CUBE_MAP);
}

void Renderer::DisableCubeTexture() const
{
	glDisable(GL_TEXTURE_CUBE_MAP);
}

// Vertex buffers
bool Renderer::CreateStaticBuffer(VertexType type, unsigned int materialID, unsigned int textureID, int nVertices, int nTextureCoordinates, int nIndices, const void* pVertices, const void* pTextureCoordinates, const unsigned int* pIndices, unsigned int* pID)
{
	VertexArray* pVertexArray = new VertexArray();

	pVertexArray->numIndices = nIndices;
	pVertexArray->numVertices = nVertices;
	pVertexArray->numTextureCoordinates = nTextureCoordinates;
	pVertexArray->materialID = materialID;
	pVertexArray->textureID = textureID;
	pVertexArray->type = type;

	// Get the correct vertex size and construct the vertex array to hold the vertices
	if (nVertices)
	{
		switch (type)
		{
		case VertexType::POSITION:
			pVertexArray->vertexSize = sizeof(PositionVertex);
			pVertexArray->pVertexArray = new float[nVertices * 3];
			break;
		case VertexType::POSITION_DIFFUSE:
			pVertexArray->vertexSize = sizeof(PositionDiffuseVertex);
			pVertexArray->pVertexArray = new float[nVertices * 6];
			break;
		case VertexType::POSITION_DIFFUSE_ALPHA:
			pVertexArray->vertexSize = sizeof(PositionDiffuseAlphaVertex);
			pVertexArray->pVertexArray = new float[nVertices * 7];
			break;
		case VertexType::POSITION_NORMAL:
			pVertexArray->vertexSize = sizeof(PositionNormalVertex);
			pVertexArray->pVertexArray = new float[nVertices * 6];
			break;
		case VertexType::POSITION_NORMAL_COLOR:
			pVertexArray->vertexSize = sizeof(PositionNormalColorVertex);
			pVertexArray->pVertexArray = new float[nVertices * 10];
			break;
		case VertexType::POSITION_NORMAL_UV:
			pVertexArray->vertexSize = sizeof(PositionNormalVertex);
			pVertexArray->pVertexArray = new float[nVertices * 6];
			pVertexArray->textureCoordinateSize = sizeof(UVCoordinate);
			pVertexArray->pTextureCoordinates = new float[nTextureCoordinates * 2];
			break;
		case VertexType::POSITION_NORMAL_UV_COLOR:
			pVertexArray->vertexSize = sizeof(PositionNormalColorVertex);
			pVertexArray->pVertexArray = new float[nVertices * 10];
			pVertexArray->textureCoordinateSize = sizeof(UVCoordinate);
			pVertexArray->pTextureCoordinates = new float[nTextureCoordinates * 2];
			break;
		}
	}

	// If we have indices, create the indices array to hold the information
	if (nIndices)
	{
		pVertexArray->pIndices = new unsigned int[nIndices];
	}

	// Copy the vertices into the vertex array
	memcpy(pVertexArray->pVertexArray, pVertices, pVertexArray->vertexSize * nVertices);

	// Copy the texture coordinates into the texture array
	memcpy(pVertexArray->pTextureCoordinates, pTextureCoordinates, pVertexArray->textureCoordinateSize * nTextureCoordinates);

	// Copy the indices into the vertex array
	memcpy(pVertexArray->pIndices, pIndices, sizeof(unsigned int) * nIndices);

	// Push the vertex array onto the list
	m_vertexArraysMutex.lock();
	m_vertexArrays.push_back(pVertexArray);

	// Return the vertex array ID
	*pID = m_vertexArrays.size() - 1;
	m_vertexArraysMutex.unlock();

	return true;
}

bool Renderer::RecreateStaticBuffer(unsigned int ID, VertexType type, unsigned int materialID, unsigned int textureID, int nVertices, int nTextureCoordinates, int nIndices, const void* pVertices, const void* pTextureCoordinates, const unsigned int* pIndices)
{
	m_vertexArraysMutex.lock();

	// Create a new vertex array
	m_vertexArrays[ID] = new VertexArray();

	// Get this already existing array pointer from the list
	VertexArray* pVertexArray = m_vertexArrays[ID];

	pVertexArray->numIndices = nIndices;
	pVertexArray->numVertices = nVertices;
	pVertexArray->numTextureCoordinates = nTextureCoordinates;
	pVertexArray->materialID = materialID;
	pVertexArray->textureID = textureID;
	pVertexArray->type = type;

	// Get the correct vertex size and construct the vertex array to hold the vertices
	if (nVertices)
	{
		switch (type)
		{
		case VertexType::POSITION:
			pVertexArray->vertexSize = sizeof(PositionVertex);
			pVertexArray->pVertexArray = new float[nVertices * 3];
			break;
		case VertexType::POSITION_DIFFUSE:
			pVertexArray->vertexSize = sizeof(PositionDiffuseVertex);
			pVertexArray->pVertexArray = new float[nVertices * 6];
			break;
		case VertexType::POSITION_DIFFUSE_ALPHA:
			pVertexArray->vertexSize = sizeof(PositionDiffuseAlphaVertex);
			pVertexArray->pVertexArray = new float[nVertices * 7];
			break;
		case VertexType::POSITION_NORMAL:
			pVertexArray->vertexSize = sizeof(PositionNormalVertex);
			pVertexArray->pVertexArray = new float[nVertices * 6];
			break;
		case VertexType::POSITION_NORMAL_COLOR:
			pVertexArray->vertexSize = sizeof(PositionNormalColorVertex);
			pVertexArray->pVertexArray = new float[nVertices * 10];
			break;
		case VertexType::POSITION_NORMAL_UV:
			pVertexArray->vertexSize = sizeof(PositionNormalVertex);
			pVertexArray->pVertexArray = new float[nVertices * 6];
			pVertexArray->textureCoordinateSize = sizeof(UVCoordinate);
			pVertexArray->pTextureCoordinates = new float[nTextureCoordinates * 2];
			break;
		case VertexType::POSITION_NORMAL_UV_COLOR:
			pVertexArray->vertexSize = sizeof(PositionNormalColorVertex);
			pVertexArray->pVertexArray = new float[nVertices * 10];
			pVertexArray->textureCoordinateSize = sizeof(UVCoordinate);
			pVertexArray->pTextureCoordinates = new float[nTextureCoordinates * 2];
			break;
		}
	}

	// If we have indices, create the indices array to hold the information
	if (nIndices)
	{
		pVertexArray->pIndices = new unsigned int[nIndices];
	}

	// Copy the vertices into the vertex array
	memcpy(pVertexArray->pVertexArray, pVertices, pVertexArray->vertexSize * nVertices);

	// Copy the texture coordinates into the texture array
	memcpy(pVertexArray->pTextureCoordinates, pTextureCoordinates, pVertexArray->textureCoordinateSize * nTextureCoordinates);

	// Copy the indices into the vertex array
	memcpy(pVertexArray->pIndices, pIndices, sizeof(unsigned int) * nIndices);

	m_vertexArraysMutex.unlock();

	return true;
}

void Renderer::DeleteStaticBuffer(unsigned int id)
{
	m_vertexArraysMutex.lock();
	if (m_vertexArrays[id])
	{
		delete m_vertexArrays[id];
		m_vertexArrays[id] = nullptr;
	}
	m_vertexArraysMutex.unlock();
}

bool Renderer::RenderStaticBuffer(unsigned int id)
{
	m_vertexArraysMutex.lock();

	if (id >= m_vertexArrays.size())
	{
		m_vertexArraysMutex.unlock();
		// We have supplied an invalid ID
		return false;
	}

	// Find the vertex array from the list
	VertexArray *pVertexArray = m_vertexArrays[id];

	bool rendered = false;
	if (pVertexArray != nullptr)
	{
		m_numRenderedVertices += pVertexArray->numVertices;
		switch (m_primitiveMode)
		{
		case GL_POINTS:
		case GL_LINES:
			m_numRenderedFaces += 0;
			break;
		case GL_TRIANGLES:
			m_numRenderedFaces += (pVertexArray->numIndices / 3);
			break;
		case GL_TRIANGLE_STRIP:
			m_numRenderedFaces += (pVertexArray->numIndices - 2);
			break;
		case GL_TRIANGLE_FAN:
			m_numRenderedFaces += (pVertexArray->numIndices - 2);
			break;
		case GL_QUADS:
			m_numRenderedFaces += (pVertexArray->numIndices / 4);
			break;
		}

		if ((pVertexArray->type != VertexType::POSITION_DIFFUSE_ALPHA) && (pVertexArray->type != VertexType::POSITION_DIFFUSE))
		{
			if (pVertexArray->materialID != -1)
			{
				m_materials[pVertexArray->materialID]->Apply();
			}
		}

		if (pVertexArray->type == VertexType::POSITION_NORMAL_UV || pVertexArray->type == VertexType::POSITION_NORMAL_UV_COLOR)
		{
			if (pVertexArray->textureID != -1)
			{
				BindTexture(pVertexArray->textureID);
			}
		}

		// Calculate the stride
		GLsizei totalStride = GetStride(pVertexArray->type);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, totalStride, pVertexArray->pVertexArray);

		if (pVertexArray->type == VertexType::POSITION_NORMAL || pVertexArray->type == VertexType::POSITION_NORMAL_UV || pVertexArray->type == VertexType::POSITION_NORMAL_UV_COLOR || pVertexArray->type == VertexType::POSITION_NORMAL_COLOR)
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, totalStride, &pVertexArray->pVertexArray[3]);
		}

		if (pVertexArray->type == VertexType::POSITION_NORMAL_UV || pVertexArray->type == VertexType::POSITION_NORMAL_UV_COLOR)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, pVertexArray->pTextureCoordinates);
		}

		if (pVertexArray->type == VertexType::POSITION_DIFFUSE_ALPHA)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_FLOAT, totalStride, &pVertexArray->pVertexArray[3]);
		}

		if (pVertexArray->type == VertexType::POSITION_DIFFUSE)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(3, GL_FLOAT, totalStride, &pVertexArray->pVertexArray[3]);
		}

		if (pVertexArray->type == VertexType::POSITION_NORMAL_UV_COLOR || pVertexArray->type == VertexType::POSITION_NORMAL_COLOR)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_FLOAT, totalStride, &pVertexArray->pVertexArray[6]);
		}

		if (pVertexArray->numIndices != 0)
		{
			glDrawElements(m_primitiveMode, pVertexArray->numIndices, GL_UNSIGNED_INT, pVertexArray->pIndices);
		}
		else
		{
			glDrawArrays(m_primitiveMode, 0, pVertexArray->numVertices);
		}

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

		rendered = true;
	}

	m_vertexArraysMutex.unlock();

	return rendered;
}

bool Renderer::RenderStaticBufferNoColor(unsigned int id)
{
	m_vertexArraysMutex.lock();

	if (id >= m_vertexArrays.size())
	{
		m_vertexArraysMutex.unlock();
		// We have supplied an invalid ID
		return false;  
	}

	// Find the vertex array from the list
	VertexArray* pVertexArray = m_vertexArrays[id];

	bool rendered = false;
	if (pVertexArray != nullptr)
	{
		m_numRenderedVertices += pVertexArray->numVertices;
		switch (m_primitiveMode)
		{
		case GL_POINTS:
		case GL_LINES:
			m_numRenderedFaces += 0;
			break;
		case GL_TRIANGLES:
			m_numRenderedFaces += (pVertexArray->numIndices / 3);
			break;
		case GL_TRIANGLE_STRIP:
			m_numRenderedFaces += (pVertexArray->numIndices - 2);
			break;
		case GL_TRIANGLE_FAN:
			m_numRenderedFaces += (pVertexArray->numIndices - 2);
			break;
		case GL_QUADS:
			m_numRenderedFaces += (pVertexArray->numIndices / 4);
			break;
		}

		if ((pVertexArray->type != VertexType::POSITION_DIFFUSE_ALPHA) && (pVertexArray->type != VertexType::POSITION_DIFFUSE))
		{
			if (pVertexArray->materialID != -1)
			{
				m_materials[pVertexArray->materialID]->Apply();
			}
		}

		if (pVertexArray->type == VertexType::POSITION_NORMAL_UV || pVertexArray->type == VertexType::POSITION_NORMAL_UV_COLOR)
		{
			if (pVertexArray->textureID != -1)
			{
				BindTexture(pVertexArray->textureID);
			}
		}

		// Calculate the stride
		GLsizei totalStride = GetStride(pVertexArray->type);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, totalStride, pVertexArray->pVertexArray);

		if (pVertexArray->type == VertexType::POSITION_NORMAL || pVertexArray->type == VertexType::POSITION_NORMAL_UV || pVertexArray->type == VertexType::POSITION_NORMAL_UV_COLOR || pVertexArray->type == VertexType::POSITION_NORMAL_COLOR)
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, totalStride, &pVertexArray->pVertexArray[3]);
		}

		if (pVertexArray->type == VertexType::POSITION_NORMAL_UV || pVertexArray->type == VertexType::POSITION_NORMAL_UV_COLOR)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, pVertexArray->pTextureCoordinates);
		}

		if (pVertexArray->numIndices != 0)
		{
			glDrawElements(m_primitiveMode, pVertexArray->numIndices, GL_UNSIGNED_INT, pVertexArray->pIndices);
		}
		else
		{
			glDrawArrays(m_primitiveMode, 0, pVertexArray->numVertices);
		}

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		rendered = true;
	}

	m_vertexArraysMutex.unlock();

	return rendered;
}

bool Renderer::RenderFromArray(VertexType type, unsigned int materialID, unsigned int textureID, int nVertices, int nTextureCoordinates, int nIndices, const void* pVertices, const void* pTextureCoordinates, const unsigned int* pIndices)
{
	if ((type != VertexType::POSITION_DIFFUSE_ALPHA) && (type != VertexType::POSITION_DIFFUSE))
	{
		if (materialID != -1)
		{
			m_materials[materialID]->Apply();
		}
	}

	if (type == VertexType::POSITION_NORMAL_UV || type == VertexType::POSITION_NORMAL_UV_COLOR)
	{
		if (textureID != -1)
		{
			BindTexture(textureID);
		}
	}

	m_numRenderedVertices += nVertices;
	switch (m_primitiveMode)
	{
	case GL_POINTS:
	case GL_LINES:
		m_numRenderedFaces += 0;
		break;
	case GL_TRIANGLES:
		m_numRenderedFaces += (nIndices / 3);
		break;
	case GL_TRIANGLE_STRIP:
		m_numRenderedFaces += (nIndices - 2);
		break;
	case GL_TRIANGLE_FAN:
		m_numRenderedFaces += (nIndices - 2);
		break;
	case GL_QUADS:
		m_numRenderedFaces += (nIndices / 4);
		break;
	}

	// Calculate the stride
	GLsizei totalStride = GetStride(type);

	float* pVertsf = static_cast<float*>(const_cast<void*>(pVertices));
	float* pTexturesf = static_cast<float*>(const_cast<void*>(pTextureCoordinates));

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, totalStride, pVertices);

	if (type == VertexType::POSITION_NORMAL || type == VertexType::POSITION_NORMAL_UV || type == VertexType::POSITION_NORMAL_UV_COLOR || type == VertexType::POSITION_NORMAL_COLOR)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, totalStride, &pVertsf[3]);
	}

	if (type == VertexType::POSITION_NORMAL_UV || type == VertexType::POSITION_NORMAL_UV_COLOR)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, &pTexturesf);
	}

	if (type == VertexType::POSITION_DIFFUSE_ALPHA)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, totalStride, &pVertsf[3]);
	}

	if (type == VertexType::POSITION_DIFFUSE)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3, GL_FLOAT, totalStride, &pVertsf[3]);
	}

	if (type == VertexType::POSITION_NORMAL_UV_COLOR || type == VertexType::POSITION_NORMAL_COLOR)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, totalStride, &pVertsf[6]);
	}

	if (nIndices != 0)
	{
		glDrawElements(m_primitiveMode, nIndices, GL_UNSIGNED_INT, pIndices);
	}
	else
	{
		glDrawArrays(m_primitiveMode, 0, nVertices);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	return true;
}

unsigned int Renderer::GetStride(VertexType type) const
{
	// Add xyz stride
	unsigned int totalStride = sizeof(float) * 3;

	// Add normals stride
	if (type == VertexType::POSITION_NORMAL || type == VertexType::POSITION_NORMAL_UV || type == VertexType::POSITION_NORMAL_UV_COLOR || type == VertexType::POSITION_NORMAL_COLOR)
	{
		totalStride += sizeof(float) * 3;
	}

	// Add color stride
	if (type == VertexType::POSITION_DIFFUSE)
	{
		totalStride += sizeof(float) * 3;
	}

	// Add color and alpha stride
	if (type == VertexType::POSITION_DIFFUSE_ALPHA || type == VertexType::POSITION_NORMAL_UV_COLOR || type == VertexType::POSITION_NORMAL_COLOR)
	{
		totalStride += sizeof(float) * 4;
	}

	return totalStride;
}

// Mesh
TriangleMesh* Renderer::CreateMesh(MeshType meshType) const
{
	TriangleMesh* pNewMesh = new TriangleMesh();

	pNewMesh->meshType = meshType;

	// Return the mesh pointer
	return pNewMesh;
}

void Renderer::ClearMesh(TriangleMesh* pMesh)
{
	pMesh->textureID = -1;
	pMesh->materialID = -1;
	
	// Delete the vertices
	for (unsigned int i = 0; i < pMesh->vertices.size(); ++i)
	{
		delete pMesh->vertices[i];
		pMesh->vertices[i] = nullptr;
	}

	// Delete the texture coordinates
	for (unsigned int i = 0; i < pMesh->textureCoordinates.size(); ++i)
	{
		delete pMesh->textureCoordinates[i];
		pMesh->textureCoordinates[i] = nullptr;
	}

	// Delete the triangles
	for (unsigned int i = 0; i < pMesh->triangles.size(); ++i)
	{
		delete pMesh->triangles[i];
		pMesh->triangles[i] = nullptr;
	}

	pMesh->vertices.clear();
	pMesh->textureCoordinates.clear();
	pMesh->triangles.clear();

	if (pMesh->staticMeshID != -1)
	{
		DeleteStaticBuffer(pMesh->staticMeshID);
	}
	pMesh->staticMeshID = -1;

	delete pMesh;
}

unsigned int Renderer::AddVertexToMesh(glm::vec3 p, glm::vec3 n, float r, float g, float b, float a, TriangleMesh* pMesh) const
{
	MeshVertex* pNewVertex = new MeshVertex();
	pNewVertex->vertexPosition[0] = p.x;
	pNewVertex->vertexPosition[1] = p.y;
	pNewVertex->vertexPosition[2] = p.z;

	pNewVertex->vertexNormals[0] = n.x;
	pNewVertex->vertexNormals[1] = n.y;
	pNewVertex->vertexNormals[2] = n.z;

	pNewVertex->vertexColor[0] = r;
	pNewVertex->vertexColor[1] = g;
	pNewVertex->vertexColor[2] = b;
	pNewVertex->vertexColor[3] = a;

	if (pMesh != nullptr)
	{
		pMesh->vertices.push_back(pNewVertex);

		unsigned int vertexID = pMesh->vertices.size() - 1;

		return vertexID;
	}

	return -1;
}

unsigned int Renderer::AddTextureCoordinatesToMesh(float s, float t, TriangleMesh* pMesh) const
{
	MeshTextureCoordinate* pNewTextureCoordinate = new MeshTextureCoordinate();
	pNewTextureCoordinate->s = s;
	pNewTextureCoordinate->t = t;

	if (pMesh != nullptr)
	{
		pMesh->textureCoordinates.push_back(pNewTextureCoordinate);

		unsigned int textureCoordinateID = pMesh->textureCoordinates.size() - 1;

		return textureCoordinateID;
	}
	
	return -1;
}

unsigned int Renderer::AddTriangleToMesh(unsigned int vertexID1, unsigned int vertexID2, unsigned int vertexID3, TriangleMesh* pMesh) const
{
	// Create the triangle
	MeshTriangle* pTri = new MeshTriangle();
	pTri->vertexIndices[0] = vertexID1;
	pTri->vertexIndices[1] = vertexID2;
	pTri->vertexIndices[2] = vertexID3;

	if (pMesh != nullptr)
	{
		pMesh->triangles.push_back(pTri);

		unsigned int triangleID = pMesh->triangles.size() - 1;

		return triangleID;
	}

	return -1;
}

void Renderer::ModifyMeshAlpha(float alpha, TriangleMesh* pMesh)
{
	m_vertexArraysMutex.lock();

	VertexArray* pArray = m_vertexArrays[pMesh->staticMeshID];

	GLsizei totalStride = GetStride(pArray->type) / 4;
	int alphaIndex = totalStride - 1;

	for (int i = 0; i < pArray->numVertices; ++i)
	{
		pArray->pVertexArray[alphaIndex] = alpha;

		alphaIndex += totalStride;
	}

	m_vertexArraysMutex.unlock();
}

void Renderer::ModifyMeshColor(float r, float g, float b, TriangleMesh* pMesh)
{
	m_vertexArraysMutex.lock();

	VertexArray* pArray = m_vertexArrays[pMesh->staticMeshID];

	GLsizei totalStride = GetStride(pArray->type) / 4;
	int rIndex = totalStride - 4;
	int gIndex = totalStride - 3;
	int bIndex = totalStride - 2;

	for (int i = 0; i < pArray->numVertices; ++i)
	{
		pArray->pVertexArray[rIndex] = r;
		pArray->pVertexArray[gIndex] = g;
		pArray->pVertexArray[bIndex] = b;

		rIndex += totalStride;
		gIndex += totalStride;
		bIndex += totalStride;
	}

	m_vertexArraysMutex.unlock();
}

void Renderer::ConvertMeshColor(float r, float g, float b, float matchR, float matchG, float matchB, TriangleMesh* pMesh)
{
	m_vertexArraysMutex.lock();

	VertexArray* pArray = m_vertexArrays[pMesh->staticMeshID];

	GLsizei totalStride = GetStride(pArray->type) / 4;
	int rIndex = totalStride - 4;
	int gIndex = totalStride - 3;
	int bIndex = totalStride - 2;

	for (int i = 0; i < pArray->numVertices; ++i)
	{
		float diffR = fabs(pArray->pVertexArray[rIndex] - matchR);
		float diffG = fabs(pArray->pVertexArray[gIndex] - matchG);
		float diffB = fabs(pArray->pVertexArray[bIndex] - matchB);
	
		if (diffR < 0.005f && diffG < 0.005f && diffB < 0.005f)
		{
			pArray->pVertexArray[rIndex] = r;
			pArray->pVertexArray[gIndex] = g;
			pArray->pVertexArray[bIndex] = b;
		}

		rIndex += totalStride;
		gIndex += totalStride;
		bIndex += totalStride;
	}

	m_vertexArraysMutex.unlock();
}

void Renderer::FinishMesh(unsigned int textureID, unsigned int materialID, TriangleMesh* pMesh)
{
	unsigned int numTriangles = pMesh->triangles.size();
	unsigned int numVertices = pMesh->vertices.size();
	unsigned int numTextureCoordinates = pMesh->textureCoordinates.size();
	unsigned int numIndices = pMesh->triangles.size() * 3;

	pMesh->materialID = materialID;
	pMesh->textureID = textureID;

	// Vertices
	PositionNormalColorVertex* meshBuffer;
	meshBuffer = new PositionNormalColorVertex[numVertices];

	for (unsigned int i = 0; i < numVertices; ++i)
	{
		meshBuffer[i].x = pMesh->vertices[i]->vertexPosition[0];
		meshBuffer[i].y = pMesh->vertices[i]->vertexPosition[1];
		meshBuffer[i].z = pMesh->vertices[i]->vertexPosition[2];

		meshBuffer[i].nx = pMesh->vertices[i]->vertexNormals[0];
		meshBuffer[i].ny = pMesh->vertices[i]->vertexNormals[1];
		meshBuffer[i].nz = pMesh->vertices[i]->vertexNormals[2];

		meshBuffer[i].r = pMesh->vertices[i]->vertexColor[0];
		meshBuffer[i].g = pMesh->vertices[i]->vertexColor[1];
		meshBuffer[i].b = pMesh->vertices[i]->vertexColor[2];
		meshBuffer[i].a = pMesh->vertices[i]->vertexColor[3];
	}

	// Texture coordinates
	UVCoordinate* textureCoordinatesBuffer;
	textureCoordinatesBuffer = new UVCoordinate[numTextureCoordinates];

	for (unsigned int i = 0; i < numTextureCoordinates; ++i)
	{
		textureCoordinatesBuffer[i].u = pMesh->textureCoordinates[i]->s;
		textureCoordinatesBuffer[i].v = pMesh->textureCoordinates[i]->t;
	}

	// Indices
	unsigned int* indicesBuffer;
	indicesBuffer = new unsigned int[numIndices];
	int indexCounter = 0;
	for (unsigned int i = 0; i < numTriangles; i++)
	{
		indicesBuffer[indexCounter] = pMesh->triangles[i]->vertexIndices[0];
		indicesBuffer[indexCounter + 1] = pMesh->triangles[i]->vertexIndices[1];
		indicesBuffer[indexCounter + 2] = pMesh->triangles[i]->vertexIndices[2];

		indexCounter += 3;
	}

	if (pMesh->meshType == MeshType::Color)
	{
		if (pMesh->staticMeshID == -1)
		{
			CreateStaticBuffer(VertexType::POSITION_NORMAL_COLOR, pMesh->materialID, -1, numVertices, 0, numIndices, meshBuffer, nullptr, indicesBuffer, &pMesh->staticMeshID);
		}
		else
		{
			RecreateStaticBuffer(pMesh->staticMeshID, VertexType::POSITION_NORMAL_COLOR, pMesh->materialID, -1, numVertices, 0, numIndices, meshBuffer, nullptr, indicesBuffer);
		}
	}
	else if (pMesh->meshType == MeshType::Textured)
	{
		if (pMesh->staticMeshID == -1)
		{
			CreateStaticBuffer(VertexType::POSITION_NORMAL_UV_COLOR, pMesh->materialID, pMesh->textureID, numVertices, numTextureCoordinates, numIndices, meshBuffer, textureCoordinatesBuffer, indicesBuffer, &pMesh->staticMeshID);
		}
		else
		{
			RecreateStaticBuffer(pMesh->staticMeshID, VertexType::POSITION_NORMAL_UV_COLOR, pMesh->materialID, pMesh->textureID, numVertices, numTextureCoordinates, numIndices, meshBuffer, textureCoordinatesBuffer, indicesBuffer);
		}
	}

	// Delete temp data
	delete[] meshBuffer;
	delete[] textureCoordinatesBuffer;
	delete[] indicesBuffer;
}

void Renderer::RenderMesh(TriangleMesh* pMesh)
{
	PushMatrix();
	
	SetPrimitiveMode(PrimitiveMode::TRIANGLES);
	if (pMesh->staticMeshID != -1)
	{
		RenderStaticBuffer(pMesh->staticMeshID);
	}

	PopMatrix();
}

void Renderer::RenderMeshNoColor(TriangleMesh* pMesh)
{
	PushMatrix();
	
	SetPrimitiveMode(PrimitiveMode::TRIANGLES);
	if (pMesh->staticMeshID != -1)
	{
		RenderStaticBufferNoColor(pMesh->staticMeshID);
	}

	PopMatrix();
}

void Renderer::GetMeshInformation(int* numVertices, int* numTriangles, TriangleMesh* pMesh) const
{
	*numVertices = static_cast<int>(pMesh->vertices.size());
	*numTriangles = static_cast<int>(pMesh->triangles.size());
}

void Renderer::StartMeshRender() const
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
}

void Renderer::EndMeshRender() const
{
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

bool Renderer::MeshStaticBufferRender(TriangleMesh* pMesh)
{
	SetPrimitiveMode(PrimitiveMode::TRIANGLES);

	return RenderStaticBuffer(pMesh->staticMeshID);
}

// Name rendering and name picking
void Renderer::InitNameStack() const
{
	glInitNames();
}

void Renderer::LoadNameOntoStack(int name) const
{
	glPushName(name);
}

void Renderer::EndNameStack() const
{
	glPopName();
}

void Renderer::StartNamePicking(unsigned int viewportID, int x, int y)
{
	ClearScene(true, true, true);

	glSelectBuffer(NAME_PICKING_BUFFER, m_selectBuffer);

	glRenderMode(GL_SELECT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	int	viewportCoords[4] = { 0, 0, 0, 0 };
	glGetIntegerv(GL_VIEWPORT, viewportCoords);
	gluPickMatrix(x, y, 3, 3, viewportCoords);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	MultiplyViewProjection();
}

int Renderer::GetPickedObject()
{
	int objectsFound;
	objectsFound = glRenderMode(GL_RENDER);

	if (objectsFound > 0)
	{
		unsigned int lowestDepth = m_selectBuffer[1];

		int selectedObject = m_selectBuffer[3];

		// Any time that we find more than one named object, we choose the one with the lowest depth. i.e Closest to the screen
		for (int i = 1; i < objectsFound; ++i)
		{
			unsigned int depth = m_selectBuffer[(i * 4) + 1];
			int name = m_selectBuffer[(i * 4) + 3];

			// Also make sure that a name of -1 doesn't ever take priority
			if (depth < lowestDepth && name != -1)
			{
				lowestDepth = m_selectBuffer[(i * 4) + 1];

				selectedObject = m_selectBuffer[(i * 4) + 3];
			}
		}

		return selectedObject;
	}

	// No objects found
	return -1;
}

// Frustum
Frustum* Renderer::GetFrustum(unsigned int frustumID)
{
	Frustum* pFrustum = m_frustums[frustumID];

	return pFrustum;
}

int Renderer::PointInFrustum(unsigned int frustumID, const glm::vec3& point)
{
	Frustum* pFrustum = m_frustums[frustumID];

	return static_cast<int>(pFrustum->PointInFrustum(point));
}

int Renderer::SphereInFrustum(unsigned int frustumID, const glm::vec3& point, float radius)
{
	Frustum* pFrustum = m_frustums[frustumID];

	return static_cast<int>(pFrustum->SphereInFrustum(point, radius));
}

int Renderer::CubeInFrustum(unsigned int frustumID, const glm::vec3& center, float x, float y, float z)
{
	Frustum* pFrustum = m_frustums[frustumID];

	return static_cast<int>(pFrustum->CubeInFrustum(center, x, y, z));
}

// Frame buffers
bool Renderer::CreateFrameBuffer(int idToResetup, bool diffuse, bool position, bool normal, bool depth, int width, int height, float viewportScale, std::string name, unsigned int* pID)
{
	FrameBuffer* pNewFrameBuffer;
	if (idToResetup == -1)
	{
		pNewFrameBuffer = new FrameBuffer();
	}
	else
	{
		pNewFrameBuffer = m_frameBuffers[idToResetup];

		glDeleteFramebuffersEXT(1, &pNewFrameBuffer->fbo);

		glDeleteTextures(1, &pNewFrameBuffer->diffuseTexture);
		glDeleteTextures(1, &pNewFrameBuffer->positionTexture);
		glDeleteTextures(1, &pNewFrameBuffer->normalTexture);
		glDeleteTextures(1, &pNewFrameBuffer->depthTexture);
	}

	pNewFrameBuffer->name = name;
	pNewFrameBuffer->diffuseTexture = -1;
	pNewFrameBuffer->positionTexture = -1;
	pNewFrameBuffer->normalTexture = -1;
	pNewFrameBuffer->depthTexture = -1;

	pNewFrameBuffer->width = width;
	pNewFrameBuffer->height = height;
	pNewFrameBuffer->viewportScale = viewportScale;

	glGenFramebuffersEXT(1, &pNewFrameBuffer->fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, pNewFrameBuffer->fbo);

	if (diffuse)
	{
		glGenTextures(1, &pNewFrameBuffer->diffuseTexture);
		glBindTexture(GL_TEXTURE_2D, pNewFrameBuffer->diffuseTexture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, static_cast<int>(width * viewportScale), static_cast<int>(height * viewportScale), 0, GL_RGBA, GL_FLOAT, nullptr);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, pNewFrameBuffer->diffuseTexture, 0);
	}

	if (position)
	{
		glGenTextures(1, &pNewFrameBuffer->positionTexture);
		glBindTexture(GL_TEXTURE_2D, pNewFrameBuffer->positionTexture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, static_cast<int>(width * viewportScale), static_cast<int>(height * viewportScale), 0, GL_RGBA, GL_FLOAT, nullptr);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, pNewFrameBuffer->positionTexture, 0);
	}

	if (normal)
	{
		glGenTextures(1, &pNewFrameBuffer->normalTexture);
		glBindTexture(GL_TEXTURE_2D, pNewFrameBuffer->normalTexture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, static_cast<int>(width * viewportScale), static_cast<int>(height * viewportScale), 0, GL_RGBA, GL_FLOAT, nullptr);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D, pNewFrameBuffer->normalTexture, 0);
	}

	if (depth)
	{
		glGenTextures(1, &pNewFrameBuffer->depthTexture);
		glBindTexture(GL_TEXTURE_2D, pNewFrameBuffer->depthTexture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glTexParameterf(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, static_cast<int>(width * viewportScale), static_cast<int>(height * viewportScale), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);

		// Instruct openGL that we won't bind a color texture with the currently binded FBO
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, pNewFrameBuffer->depthTexture, 0);
	}

	// Check if all worked fine and unbind the FBO
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		return false;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	if (idToResetup == -1)
	{
		// Push the frame buffer onto the list
		m_frameBuffers.push_back(pNewFrameBuffer);

		// Return the frame buffer ID
		*pID = m_frameBuffers.size() - 1;
	}
	else
	{
		*pID = idToResetup;
	}

	return true;
}

int Renderer::GetNumFrameBuffers() const
{
	return m_frameBuffers.size();
}

FrameBuffer* Renderer::GetFrameBuffer(std::string name)
{
	int foundIndex = -1;
	for (int i = 0; i < m_frameBuffers.size(); ++i)
	{
		if (m_frameBuffers[i]->name == name)
		{
			foundIndex = i;
		}
	}

	if (foundIndex == -1)
	{
		return nullptr;
	}

	return GetFrameBuffer(foundIndex);
}

FrameBuffer* Renderer::GetFrameBuffer(int index)
{
	return m_frameBuffers[index];
}

int Renderer::GetFrameBufferIndex(std::string name)
{
	int foundIndex = -1;
	for (int i = 0; i < m_frameBuffers.size(); ++i)
	{
		if (m_frameBuffers[i]->name == name)
		{
			foundIndex = i;
		}
	}

	return foundIndex;
}

void Renderer::StartRenderingToFrameBuffer(unsigned int frameBufferID)
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_frameBuffers[frameBufferID]->fbo);
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, static_cast<int>(m_frameBuffers[frameBufferID]->width * m_frameBuffers[frameBufferID]->viewportScale), static_cast<int>(m_frameBuffers[frameBufferID]->height * m_frameBuffers[frameBufferID]->viewportScale));

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Clear the render targets
	GLbitfield clear(0);

	if (m_frameBuffers[frameBufferID]->diffuseTexture != -1)
	{
		clear |= GL_COLOR_BUFFER_BIT;
	}
	if (m_frameBuffers[frameBufferID]->depthTexture != -1)
	{
		clear |= GL_DEPTH_BUFFER_BIT;
	}
	glClear(clear);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);

	// Specify what to render an start acquiring
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT };
	glDrawBuffers(3, buffers);
}

void Renderer::StopRenderingToFrameBuffer(unsigned int frameBufferID) const
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glPopAttrib();
}

unsigned int Renderer::GetDiffuseTextureFromFrameBuffer(unsigned int frameBufferID)
{
	return m_frameBuffers[frameBufferID]->diffuseTexture;
}

unsigned int Renderer::GetPositionTextureFromFrameBuffer(unsigned int frameBufferID)
{
	return m_frameBuffers[frameBufferID]->positionTexture;
}

unsigned int Renderer::GetNormalTextureFromFrameBuffer(unsigned int frameBufferID)
{
	return m_frameBuffers[frameBufferID]->normalTexture;
}

unsigned int Renderer::GetDepthTextureFromFrameBuffer(unsigned int frameBufferID)
{
	return m_frameBuffers[frameBufferID]->depthTexture;
}

// Rendered information
void Renderer::ResetRenderedStats()
{
	m_numRenderedVertices = 0;
	m_numRenderedFaces = 0;
}

int Renderer::GetNumRenderedVertices()
{
	return m_numRenderedVertices;
}

int Renderer::GetNumRenderedFaces()
{
	return m_numRenderedFaces;
}

// Shaders
bool Renderer::LoadGLSLShader(const char* vertexFile, const char* fragmentFile, unsigned int* pID)
{
	glShader* pShader;

	// Load the shader
	// load (and compile, link) from file
	pShader = m_shaderManager.loadfromFile(vertexFile, fragmentFile);

	if (pShader != nullptr)
	{
		// Push the vertex array onto the list
		m_shaders.push_back(pShader);

		// Return the vertex array id
		*pID = m_shaders.size() - 1;

		return true;
	}

	std::cout << "ERROR: Could not load GLSL shaders: " << vertexFile << ", " << fragmentFile << std::endl << std::flush;

	return false;
}

void Renderer::BeginGLSLShader(unsigned int shaderID)
{
	m_shaders[shaderID]->begin();
}

void Renderer::EndGLSLShader(unsigned int shaderID)
{
	m_shaders[shaderID]->end();
}

glShader* Renderer::GetShader(unsigned int shaderID)
{
	return m_shaders[shaderID];
}