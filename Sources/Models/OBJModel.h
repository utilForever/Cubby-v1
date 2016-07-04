/*************************************************************************
> File Name: OBJModel.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Object model.
> Created Time: 2016/07/05
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_OBJ_MODEL_H
#define CUBBY_OBJ_MODEL_H

#include "BoundingBox.h"
#include "../Renderer/Renderer.h"

// OBJ Model structures
typedef struct OBJVertex
{
	glm::vec3 position;
	int massID;
	glm::vec3 massOffset;
} OBJVertex;

typedef struct OBJFace
{
	int* pIndices;    // List of vertex indices.
	int numPoints;    // Number of vertex points.
} OBJFace;

typedef struct OBJTextureCoordinate
{
	float u;
	float v;
} OBJTextureCoordinate;

// OBJ Model class
class OBJModel
{
public:
	// Constructor, Destructor
	OBJModel(Renderer* pRenderer);
	~OBJModel();

	bool Load(const char* modelFileName, const char* textureFileName);

	void CalculateBoundingBox();
	BoundingBox* GetBoundingBox();

	void Render();
	void RenderMesh();
	void RenderWireFrame();
	void RenderBoundingBox() const;

private:
	Renderer* m_pRenderer;

	std::string m_ModelFileName;
	std::string m_TextureFileName;

	// Vertices
	int m_numVertices;
	OBJVertex* m_pVertices;

	// Texture Coordinates
	int m_numTexCoordinates;
	OBJTextureCoordinate* m_pTextureCoordinates;

	// Normals
	int m_numNormals;
	glm::vec3* m_pNormals;

	// Faces
	int m_numFaces;
	OBJFace* m_pFaces;

	// Texture
	unsigned int m_texture;

	// Bounding box
	BoundingBox m_BoundingBox;
};

#endif