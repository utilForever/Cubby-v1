/*************************************************************************
> File Name: MS3DModel.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Milk Shape 3D Model.
> Created Time: 2016/06/26
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_MS3D_MODEL_H
#define CUBBY_MS3D_MODEL_H

// byte-align structures
#if defined(_MSC_VER) || defined(__BORLANDC__)
	#include <pshpack1.h>
	#define PACK_STRUCT
	#define PATH_MAX _MAX_PATH
#elif defined(__GNUC__)
	#define PACK_STRUCT	__attribute__((packed))
	#include <limits.h>
#else
	#error You must byte-align these structures with the appropriate compiler directives
#endif

#include "../Renderer/Renderer.h"
#include "BoundingBox.h"

// Type alias
using byte = unsigned char;
using word = unsigned short;

// MS3D File structure
struct MS3DHeader
{
	char ID[10];
	int version;
} PACK_STRUCT;

// MS3D Vertex structure
struct MS3DVertex
{
	byte flags;
	float vertex[3];
	char boneID;
	byte refCount;
} PACK_STRUCT;

// MS3D Triangle structure
struct MS3DTriangle
{
	word flags;
	word vertexIndices[3];
	float vertexNormals[3][3];
	float s[3], t[3];
	byte smoothingGroup;
	byte groupIndex;
} PACK_STRUCT;

// MS3D Material structure
struct MS3DMaterial
{
	char name[32];
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float emissive[4];
	float shininess;		// 0.0f - 128.0f
	float transparency;		// 0.0f - 1.0f
	byte mode;				// 0, 1, 2 is unused now
	char texture[128];
	char alphamap[128];
} PACK_STRUCT;

// MS3D Joint structure
struct MS3DJoint
{
	byte flags;
	char name[32];
	char parentName[32];
	float rotation[3];
	float translation[3];
	word numRotationKeyframes;
	word numTranslationKeyframes;
} PACK_STRUCT;

// MS3D Keyframe structure
struct MS3DKeyframe
{
	float time;
	float parameter[3];
} PACK_STRUCT;

#if defined(_MSC_VER) || defined(__BORLANDC__)
// End byte-align structures
#include <poppack.h>
#endif

// Vertex structure
typedef struct Vertex
{
	char boneID;
	float location[3];
} Vertex;

// Triangle structure
typedef struct Triangle
{
	float vertexNormals[3][3];
	int vertexIndices[3];
	float s[3], t[3];
} Triangle;

// Material model structure
typedef struct MaterialModel
{
	float ambient[4], diffuse[4], specular[4], emissive[4];
	float shininess;
	unsigned int texture;
	char* pTextureFileName;
} MaterialModel;

// Mesh structure
typedef struct Mesh
{
	int materialIndex;
	int numTriangles;
	int* pTriangleIndices;
} Mesh;

// Keyframe structure
typedef struct Keyframe
{
	int jointIndex;
	float time;			// In milliseconds
	float parameter[3];
} Keyframe;

// Joint structure
typedef struct Joint
{
	float localRotation[3];
	float localTranslation[3];
	Matrix4 absolute, relative;

	int numRotationKeyframes, numTranslationKeyframes;
	Keyframe* pTranslationKeyframes;
	Keyframe* pRotationKeyframes;

	int parent;

	char name[32];
} Joint;

class MS3DModel
{
public:
	// Constructor, Destructor
	MS3DModel(Renderer* pRenderer);
	~MS3DModel();

	// Load
	bool LoadModel(const char* modelFileName, bool isStatic = false);
	bool LoadTextures() const;

	// Setup
	void SetupStaticBuffer();
	void SetJointKeyframe(int jointIndex, int keyframeIndex, float time, float* parameter, bool isRotation) const;
	void SetupJoints() const;

	// Bounding box
	void CalculateBoundingBox();
	BoundingBox* GetBoundingBox();

	// Bones
	int GetBoneIndex(const char* boneName) const;
	const char* GetNameFromBoneIndex(int boneIndex) const;

	// Joints
	int GetNumJoints() const;
	Joint* GetJoint(const char* jointName) const;
	Joint* GetJoint(int index) const;

	// Rendering
	void Render(bool isMesh, bool isNormal, bool isBone, bool isBoundingBox);
	void RenderMesh() const;
	void RenderMeshStatic() const;
	void RenderNormals();
	void RenderBones() const;
	void RenderBoundingBox();

private:
	Renderer* m_pRenderer;

	// Vertices
	int m_numVertices;
	Vertex* m_pVertices;

	// Triangles
	int m_numTriangles;
	Triangle* m_pTriangles;

	// Materials
	int m_numMaterials;
	MaterialModel* m_pMaterials;

	// Meshes
	int m_numMeshes;
	Mesh* m_pMeshes;

	// Joints
	int m_numJoints;
	Joint* m_pJoints;

	// Animation FPS
	float m_animationFPS;

	// Static mesh?
	bool m_isStatic;
	unsigned int* m_staticRenderBuffers;
	
	// Bounding box
	BoundingBox m_boundingBox;

	friend class MS3DAnimator;
};

#endif