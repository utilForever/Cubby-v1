/*************************************************************************
> File Name: Renderer.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: The OpenGL renderer that is an encapsulation of all the rendering functionality of the engine. A wrapper around most common OpenGL calls.
> Created Time: 2016/06/25
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_RENDERER_H
#define CUBBY_RENDERER_H

#include "GLSL.h"

#include <freetype/freetypefont.h>
#include <GL/glew.h>
#include <tinythread/tinythread.h>

#include <Maths/Bezier3.h>
#include <Maths/Bezier4.h>
#include <Maths/Matrix4.h>

#include "Color.h"
#include "FrameBuffer.h"
#include "Frustum.h"
#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Viewport.h"

#pragma comment (lib, "opengl32")
#pragma comment (lib, "glu32")

int CheckGLErrors(char* file, int line);
#define CHECK_GL_ERRORS() CheckGLErrors(__FILE__, __LINE__)

enum class ProjectionMode
{
	PERSPECTIVE = 0,
	ORTHOGRAPHIC,
	TWO_DIMENSION
};

enum class RenderMode
{
	WIREFRAME = 0,
	SOLID,
	SHADED,
	TEXTURED,
	TEXTURED_LIGHTING,
};

enum class PrimitiveMode
{
	POINTS = 0,
	LINES,
	LINELIST,
	TRIANGLES,
	TRIANGLESTRIPS,
	TRIANGLEFANS,
	QUADS,
};

enum class CullMode
{
	NOCULL = 0,
	BACK,
	FRONT,
};

enum class BlendFunction
{
	ONE = 0,
	ZERO,
	ONE_MINUS_SRC_ALPHA,
	SRC_ALPHA,
};

enum class DepthTest
{
	NEVER = 0,
	ALWAYS,
	LESS,
	LESS_EQUAL,
	EQUAL,
	GREATER_EQUAL,
	GREATER,
	NOT_EQUAL,
};

enum class ImmediateModePrimitive
{
	POINTS = 0,
	LINES,
	LINE_LOOP,
	LINE_STRIP,
	TRIANGLES,
	TRIANGLE_STRIP,
	TRIANGLE_FAN,
	QUADS,
	QUAD_STRIP,
	POLYGON,
};

enum class FrontFaceDirection
{
	CW = 0,
	CCW,
};

struct PositionVertex
{
	float x, y, z;		// Position.
};

struct PositionDiffuseVertex
{
	float x, y, z;		// Position.
	float r, g, b;		// Color.
};

struct PositionDiffuseAlphaVertex
{
	float x, y, z;		// Position.
	float r, g, b;		// Color.
	float a;			// Alpha.
};

struct PositionNormalVertex
{
	float x, y, z;		// Position.
	float nx, ny, nz;	// Normal.
};

struct PositionNormalColorVertex
{
	float x, y, z;		// Position.
	float nx, ny, nz;	// Normal.
	float r, g, b;		// Color.
	float a;			// Alpha.
};

struct UVCoordinate
{
	float u, v;			// Texture coordinates.
};

class Renderer
{
public:
	// Constructor, Destructor
	Renderer(int width, int height, int depthBits, int stencilBits);
	~Renderer();

	// Resize
	void ResizeWindow(int newWith, int newHeight);

	// Viewport
	bool CreateViewport(int bottom, int left, int width, int height, float fov, unsigned int* pID);
	bool ResizeViewport(unsigned int viewportID, int bottom, int left, int width, int height, float fov);
	int GetActiveViewport() const;

	// Render modes
	void SetRenderMode(RenderMode mode);
	void SetPrimitiveMode(PrimitiveMode mode);
	void SetCullMode(CullMode mode);
	CullMode GetCullMode() const;
	void SetLineWidth(float width);
	void SetPointSize(float width);
	void SetFrontFaceDirection(FrontFaceDirection direction);

	// Projection
	bool SetProjectionMode(ProjectionMode mode, int viewport);
	void SetViewProjection();
	void MultiplyViewProjection();
	void SetupOrthographicProjection(float left, float right, float bottom, float top, float zNear, float zFar);

	// Scene
	bool ClearScene(bool pixel = true, bool depth = true, bool stencil = true) const;
	bool BeginScene(bool pixel = true, bool depth = true, bool stencil = true);
	void EndScene();
	void SetColorMask(bool red, bool green, bool blue, bool alpha);
	void SetClearColor(float red, float green, float blue, float alpha);
	void SetClearColor(Color color);

	// Push, Pop matrix stack
	void PushMatrix();
	void PopMatrix();

	// Matrix manipulations
	void SetWorldMatrix(const Matrix4& matrix);
	void GetModelViewMatrix(Matrix4* pMatrix);
	void GetModelMatrix(Matrix4* pMatrix);
	void GetViewMatrix(Matrix4* pMatrix);
	void GetProjectionMatrix(Matrix4* pMatrix);
	void IdentityWorldMatrix();
	void MultiplyWorldMatrix(const Matrix4& matrix);
	void TranslateWorldMatrix(float x, float y, float z);
	void RotateWorldMatrix(float x, float y, float z);
	void ScaleWorldMatrix(float x, float y, float z);

	// Texture matrix manipulations
	void SetTextureMatrix();
	void PushTextureMatrix();
	void PopTextureMatrix();

	// Scissor testing
	void EnableScissorTest(int x, int y, int width, int height);
	void DisableScissorTest();

	// Screen projection
	glm::vec3 GetWorldProjectionFromScreenCoordinates(int x, int y, float z) const;
	void GetScreenCoordinatesFromWorldPosition(glm::vec3 position, int* x, int* y);

	// Clip planes
	void EnableClipPlane(unsigned int index, double eq1, double eq2, double eq3, double eq4);
	void DisableClipPlane(unsigned int index);

	// Camera functionality
	void SetLookAtCamera(glm::vec3 position, glm::vec3 target, glm::vec3 up);

	// Transparency
	void EnableTransparency(BlendFunction source, BlendFunction destination);
	void DisableTransparency();
	GLenum GetBlendEnum(BlendFunction flag);

	// Sampling
	void EnableMultiSampling();
	void DisableMultiSampling();

	// Vector normalize
	void EnableVectorNormalize();
	void DisableVectorNormalize();

	// Depth testing
	void EnableDepthTest(DepthTest testFunction);
	void DisableDepthTest();
	GLenum GetDepthTest(DepthTest test);
	void EnableDepthWrite();
	void DisableDepthWrite();

	// Immediate mode
	void EnableImmediateMode(ImmediateModePrimitive mode);
	void ImmediateVertex(float x, float y, float z);
	void ImmediateVertex(int x, int y, int z);
	void ImmediateNormal(float x, float y, float z);
	void ImmediateNormal(int x, int y, int z);
	void ImmediateTextureCoordinate(float s, float t);
	void ImmediateColorAlpha(float r, float g, float b, float a);
	void DisableImmediateMode();

	// Drawing helpers
	void DrawLineCircle(float radius, int points) const;
	void DrawSphere(float radius, int slices, int stacks) const;
	void DrawBezier(Bezier3 curve, int points) const;
	void DrawBezier(Bezier4 curve, int points) const;
	void DrawCircleSector(float radius, float angle, int points) const;
	void DrawSphericalSector(float radius, float angle, int sectors, int points);

	// Text rendering
	bool CreateFreeTypeFont(const char* fontName, int fontSize, unsigned int* pID, bool noAutoHint = false);
	bool RenderFreeTypeText(unsigned int fontID, float x, float y, float z, Color color, float scale, const char *inText, ...);
	int GetFreeTypeTextWidth(unsigned int fontID, const char* inText, ...);
	int GetFreeTypeTextHeight(unsigned int fontID, const char* inText, ...);
	int GetFreeTypeTextAscent(unsigned int fontID);
	int GetFreeTypeTextDescent(unsigned int fontID);

	// Lighting
	bool CreateLight(const Color& ambient, const Color& diffuse, const Color& specular, glm::vec3& position, glm::vec3& direction, float exponent, float cutoff, float cAtten, float lAtten, float qAtten, bool point, bool spot, unsigned int* pID);
	bool EditLight(unsigned int id, const Color& ambient, const Color& diffuse, const Color& specular, glm::vec3& position, glm::vec3& direction, float exponent, float cutoff, float cAtten, float qAtten, float lAtten, bool point, bool spot);
	bool EditLightPosition(unsigned int id, glm::vec3& position);
	void DeleteLight(unsigned int id);
	void EnableLight(unsigned int id, unsigned int lightNumber);
	void DisableLight(unsigned int lightNumber) const;
	void RenderLight(unsigned int id);
	Color GetLightAmbient(unsigned int id);
	Color GetLightDiffuse(unsigned int id);
	Color GetLightSpecular(unsigned int id);
	glm::vec3 GetLightPosition(unsigned int id);
	float GetConstantAttenuation(unsigned int id);
	float GetLinearAttenuation(unsigned int id);
	float GetQuadraticAttenuation(unsigned int id);

	// Materials
	bool CreateMaterial(const Color& ambient, const Color& diffuse, const Color& specular, const Color& emmisive, float specularPower, unsigned int* pID);
	bool EditMaterial(unsigned int id, const Color& ambient, const Color& diffuse, const Color& specular, const Color& emmisive, float specularPower);
	void EnableMaterial(unsigned int id);
	void DeleteMaterial(unsigned int id);

	// Textures
	bool LoadTexture(std::string fileName, int* width, int* height, int* widthPower2, int* heightPower2, unsigned int* pID);
	bool RefreshTexture(unsigned int id);
	bool RefreshTexture(std::string fileName);
	void BindTexture(unsigned int id);
	void PrepareShaderTexture(unsigned int textureIndex, unsigned int textureID) const;
	void EmptyTextureIndex(unsigned int textureIndex) const;
	void DisableTexture() const;
	Texture* GetTexture(unsigned int id);
	void BindRawTextureID(unsigned int textureID) const;
	void GenerateEmptyTexture(unsigned int* pID);
	void SetTextureData(unsigned int id, int width, int height, unsigned char* texData);

	// Cube textures
	bool LoadCubeTexture(int* width, int* height, std::string front, std::string back, std::string top, std::string bottom, std::string left, std::string right, unsigned int* pID) const;
	void BindCubeTexture(unsigned int id) const;
	void EmptyCubeTextureIndex(unsigned int textureIndex) const;
	void DisableCubeTexture() const;

	// Vertex buffers
	bool CreateStaticBuffer(VertexType type, unsigned int materialID, unsigned int textureID, int nVertices, int nTextureCoordinates, int nIndices, const void* pVertices, const void* pTextureCoordinates, const unsigned int* pIndices, unsigned int* pID);
	bool RecreateStaticBuffer(unsigned int ID, VertexType type, unsigned int materialID, unsigned int textureID, int nVertices, int nTextureCoordinates, int nIndices, const void* pVertices, const void* pTextureCoordinates, const unsigned int* pIndices);
	void DeleteStaticBuffer(unsigned int id);
	bool RenderStaticBuffer(unsigned int id);
	bool RenderStaticBufferNoColor(unsigned int id);
	bool RenderFromArray(VertexType type, unsigned int materialID, unsigned int textureID, int nVertices, int nTextureCoordinates, int nIndices, const void* pVertices, const void* pTextureCoordinates, const unsigned int* pIndices);
	unsigned int GetStride(VertexType type) const;

	// Mesh
	TriangleMesh* CreateMesh(MeshType meshType) const;
	void ClearMesh(TriangleMesh* pMesh);
	unsigned int AddVertexToMesh(glm::vec3 p, glm::vec3 n, float r, float g, float b, float a, TriangleMesh* pMesh) const;
	unsigned int AddTextureCoordinatesToMesh(float s, float t, TriangleMesh* pMesh) const;
	unsigned int AddTriangleToMesh(unsigned int vertexID1, unsigned int vertexID2, unsigned int vertexID3, TriangleMesh* pMesh) const;
	void ModifyMeshAlpha(float alpha, TriangleMesh* pMesh);
	void ModifyMeshColor(float r, float g, float b, TriangleMesh* pMesh);
	void ConvertMeshColor(float r, float g, float b, float matchR, float matchG, float matchB, TriangleMesh* pMesh);
	void FinishMesh(unsigned int textureID, unsigned int materialID, TriangleMesh* pMesh);
	void RenderMesh(TriangleMesh* pMesh);
	void RenderMeshNoColor(TriangleMesh* pMesh);
	void GetMeshInformation(int* numVertices, int* numTriangles, TriangleMesh* pMesh) const;
	void StartMeshRender() const;
	void EndMeshRender() const;
	bool MeshStaticBufferRender(TriangleMesh* pMesh);

	// Name rendering and name picking
	void InitNameStack() const;
	void LoadNameOntoStack(int name) const;
	void EndNameStack() const;
	void StartNamePicking(unsigned int viewportID, int x, int y);
	int GetPickedObject();

	// Frustum
	Frustum* GetFrustum(unsigned int frustumID);
	int PointInFrustum(unsigned int frustumID, const glm::vec3& point);
	int SphereInFrustum(unsigned int frustumID, const glm::vec3& point, float radius);
	int CubeInFrustum(unsigned int frustumID, const glm::vec3& center, float x, float y, float z);

	// Frame buffers
	bool CreateFrameBuffer(int idToResetup, bool diffuse, bool position, bool normal, bool depth, int width, int height, float viewportScale, std::string name, unsigned int* pID);
	int GetNumFrameBuffers() const;
	FrameBuffer* GetFrameBuffer(std::string name);
	FrameBuffer* GetFrameBuffer(int index);
	int GetFrameBufferIndex(std::string name);
	void StartRenderingToFrameBuffer(unsigned int frameBufferID);
	void StopRenderingToFrameBuffer(unsigned int frameBufferID) const;
	unsigned int GetDiffuseTextureFromFrameBuffer(unsigned int frameBufferID);
	unsigned int GetPositionTextureFromFrameBuffer(unsigned int frameBufferID);
	unsigned int GetNormalTextureFromFrameBuffer(unsigned int frameBufferID);
	unsigned int GetDepthTextureFromFrameBuffer(unsigned int frameBufferID);

	// Rendered information
	void ResetRenderedStats();
	int GetNumRenderedVertices();
	int GetNumRenderedFaces();

	// Shaders
	bool LoadGLSLShader(const char* vertexFile, const char* fragmentFile, unsigned int* pID);
	void BeginGLSLShader(unsigned int shaderID);
	void EndGLSLShader(unsigned int shaderID);
	glShader* GetShader(unsigned int shaderID);
	
private:
	// Window's width and height
	int m_windowWidth;
	int m_windowHeight;

	// Stencil and depth bits
	bool m_stencil;
	bool m_depth;

	// Clipping planes
	float m_clipNear;
	float m_clipFar;

	// Primitive mode that we are currently operating in
	GLenum m_primitiveMode;

	// Cull mode
	CullMode m_cullMode;

	// Quadratic drawing
	GLUquadricObj* m_quadratic;

	// Viewports
	std::vector<Viewport*> m_viewports;
	unsigned int m_activeViewport;

	// Frustums
	std::vector<Frustum*> m_frustums;
	// NOTE: We store a frustum for each viewport,
	// therefore viewport and frustum are closely linked (See viewport functions)

	// Materials
	std::vector<Material*> m_materials;

	// Textures
	std::vector<Texture*> m_textures;

	// Lights
	std::vector<Light*> m_lights;

	// Fonts
	std::vector<FreeTypeFont*> m_freetypeFonts;

	// Vertex arrays, for storing static vertex data
	std::vector<VertexArray*> m_vertexArrays;
	tthread::mutex m_vertexArraysMutex;

	// Frame buffers
	std::vector<FrameBuffer*> m_frameBuffers;

	// Rendered information
	int m_numRenderedVertices;
	int m_numRenderedFaces;

	// Shaders
	glShaderManager m_shaderManager;
	std::vector<glShader*> m_shaders;

	// Matrices
	Matrix4* m_projection;
	Matrix4 m_view;
	Matrix4 m_model;

	// Model stack
	std::vector<Matrix4> m_modelStack;

	// Name picking
	static const int NAME_PICKING_BUFFER = 64;
	unsigned int m_selectBuffer[NAME_PICKING_BUFFER];
};

#endif